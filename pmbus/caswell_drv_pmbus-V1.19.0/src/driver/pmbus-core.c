/******************************************************************************

  CASwell(R) PMBus Linux driver
  Copyright(c) 2014 Cano Huang <cano.huang@cas-well.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

 *********************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <asm/delay.h>

#include "pmbus-ioctl.h"
#include "pmbus-core.h"

static int sensor_fmt = PMB_SENS_LINEAR_MODE;
module_param(sensor_fmt, int, S_IRUGO);

static int delay = IOCTL_DELAY;
module_param(delay, int, 0);

static struct i2c_adapter *adapter;
static struct pmbus_info_t pmbus_info[8];
struct mutex sw_mux_sem;

/*
 * Convert linear sensor values to milli- or micro-units
 * depending on sensor type.
 */
static long fmt_linear_data(uint8_t type, uint8_t *raw, uint8_t devno)
{
	int data;
	int16_t exponent;
	int32_t mantissa;
	long val;

	data = (raw[1] << 8) | raw[0];

	if ((type == PMB_VOUT_FMT) && (pmbus_info[devno].vout_mode == 0)) { /* LINEAR16 */
		exponent = pmbus_info[devno].vout_exponent;
		mantissa = data;
	} else {                                /* LINEAR11 */
		exponent = ((int16_t) data) >> 11;
		mantissa = ((int16_t) ((data & 0x7ff) << 5)) >> 5;
	}

	val = mantissa;

	if (type == PMB_SENS_MILLI_FMT) {
		val = val * 1000L;
	} else if (type == PMB_SENS_MICRO_FMT) {
		val = val * 1000000L;
	} else if (type == PMB_VOUT_FMT) {
		val = val * 1000L;
	}

	if (exponent >= 0) {
		val <<= exponent;
	} else {
		val >>= -exponent;
	}

	return val;
}

int format_raw_data(uint8_t type, uint8_t *raw, union pmb_data_u *data, uint8_t len, uint8_t devno)
{
	long val = 0;

	switch (type) {
	case PMB_NUMC_FMT:
		data->value = 0;
		memcpy(&data->value, raw, len);
		break;
	case PMB_STR_FMT:
		sprintf(data->string, "%s", raw);
		break;
	case PMB_SENS_FMT:
	case PMB_SENS_MILLI_FMT:
	case PMB_SENS_MICRO_FMT:
		if (sensor_fmt == PMB_SENS_LINEAR_MODE) {
			val = fmt_linear_data(type, raw, devno);
			data->value = val;
		}
		break;
	case PMB_VOUT_FMT:
		if (pmbus_info[devno].vout_mode == 0 || pmbus_info[devno].vout_mode == 7) {
			val = fmt_linear_data(type, raw, devno);
			data->value = val;
		}
		break;
	default:
		return -EINVAL;
	}


	return 0;
}

int pmbus_read_xfer(struct i2c_client *client, const struct pmbus_cmd_property_t *property, uint8_t *data)
{
	int ret = 0;

	switch (property->data_type) {
	case PMB_BYTE_DATA:
		if ((ret = i2c_smbus_read_byte_data(client, property->cmd)) >= 0) {
			data[0] = ret & 0xFF;
		}
		break;
	case PMB_WORD_DATA:
		if ((ret = i2c_smbus_read_word_data(client, property->cmd)) >= 0) {
			data[0] = ret & 0xFF;
			data[1] = (ret >> 8) & 0xFF;
		}
		break;
	case PMB_BLOCK_DATA:
		ret = i2c_smbus_read_block_data(client, property->cmd, data);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

int pmbus_write_xfer(struct i2c_client *client, const struct pmbus_cmd_property_t *property, union pmb_data_u *data)
{
	int ret = 0;

	switch (property->data_type) {
	case PMB_BYTE_DATA:
		ret = i2c_smbus_write_byte_data(client, property->cmd, data->value);
		break;
	case PMB_WORD_DATA:
		ret = i2c_smbus_write_word_data(client, property->cmd, data->value);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

int find_pmbus_property(uint8_t opcmd)
{
	int i;

	for (i = 0; i < (sizeof(pmbus_cmd_property) / (sizeof(struct pmbus_cmd_property_t))); i++) {
		if (pmbus_cmd_property[i].cmd == opcmd) {
			return i;
		}
	}

	return -EINVAL;
}

int pmbus_read_status(uint8_t devno, uint8_t opcmd, union pmb_data_u *data, uint8_t pec)
{
	int ret;
	uint8_t buf[128] = {0};
	const struct pmbus_cmd_property_t *property;
	struct i2c_client *client;

	ret = find_pmbus_property(opcmd);

	if (ret < 0) {
		goto propty_err;
	}

	property = &pmbus_cmd_property[ret];

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	client->adapter = adapter;
	if (devno == DEFAULT_DEVNO) {
		client->addr = DEFAULT_ADDRESS;
		/* Backplane devno is 7 */
		devno = 7;
	} else {
		client->addr = 0x58 + devno;
	}
	client->flags = 0;

	/* Cehck smbus PEC */
	if (pec) {
		client->flags |= I2C_CLIENT_PEC;
	} else {
		client->flags &= ~I2C_CLIENT_PEC;
	}

	ret = pmbus_read_xfer(client, property, buf);
	if (ret < 0) {
		goto xfer_err;
	}

	ret = format_raw_data(property->data_format, buf, data, property->data_len, devno);

xfer_err:
	kfree(client);
propty_err:
	return ret;
}

int pmbus_write_status(uint8_t devno, uint8_t opcmd, union pmb_data_u *data, uint8_t pec)
{
	int ret;
	const struct pmbus_cmd_property_t *property;
	struct i2c_client *client;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	client->adapter = adapter;
	if (devno == DEFAULT_DEVNO) {
		client->addr = DEFAULT_ADDRESS;
		/* Backplane devno is 7 */
		devno = 7;
	} else {
		client->addr = 0x58 + devno;
	}

	client->flags = 0;
	/* Cehck smbus PEC */
	if (pec) {
		client->flags |= I2C_CLIENT_PEC;
	} else {
		client->flags &= ~I2C_CLIENT_PEC;
	}

	/* Write page value */
	ret = find_pmbus_property(opcmd);
	if (ret < 0) {
		goto propty_err;
	}

	property = &pmbus_cmd_property[ret];

	ret = pmbus_write_xfer(client, property, data);

propty_err:
	kfree(client);
	return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
long pmbus_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
int pmbus_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	struct pmbus_iodata_t iodata;
	int ret = 0;

	if (!mutex_trylock(&sw_mux_sem)) {
		return EBUSY;
	}

	memset(&iodata, 0, sizeof(iodata));

	if (copy_from_user(&iodata, (int __user *)arg, sizeof(iodata))) {
		printk("Get user space data fail\n");
		mutex_unlock(&sw_mux_sem);
		return -EFAULT;
	}

	switch (cmd) {
	case IOCTL_PMBUS_READ_STS:
		if ((iodata.backplane == FSP_VALUE) && (iodata.devno == 7)) {
			iodata.devno = DEFAULT_DEVNO;
		}
		ret = pmbus_read_status(iodata.devno, iodata.opcmd, &iodata.data, iodata.pec);

		if (copy_to_user((int __user *)arg, &iodata, sizeof(iodata))) {
			printk("Return user space data fail\n");
			mutex_unlock(&sw_mux_sem);
			return -EFAULT;
		}
		break;
	case IOCTL_PMBUS_WRITE_STS:
		if ((iodata.backplane == FSP_VALUE) && (iodata.devno == 7)) {
			iodata.devno = DEFAULT_DEVNO;
		}
		ret = pmbus_write_status(iodata.devno, iodata.opcmd, &iodata.data, iodata.pec);
		break;
	default:
		mutex_unlock(&sw_mux_sem);
		return -ENOTTY;
	}

	if (delay > 0) {
		mdelay(delay);
	}

	mutex_unlock(&sw_mux_sem);

	return ret;
}


static struct file_operations pmbus_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
	.unlocked_ioctl = pmbus_ioctl,
#else
	.ioctl = pmbus_ioctl,
#endif
	.compat_ioctl = pmbus_ioctl,
};

static struct miscdevice pmbus_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "pmbus_ctrl",
	.fops = &pmbus_fops,
};

static int pmbus_fsp_default(int address)
{
	int ret;
	const struct pmbus_cmd_property_t *property;
	struct i2c_client *client;
	char buf[32];

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	client->adapter = adapter;
	client->addr = address;
	client->flags = 0;

	/* Get PMBus version */
	ret = find_pmbus_property(PMB_CMD_PMBUS_REVISION);
	if (ret < 0) {
		goto propty_err;
	}

	property = &pmbus_cmd_property[ret];

	ret = pmbus_read_xfer(client, property, buf);
	if (ret < 0) {
		goto xfer_err;
	}

	pmbus_info[7].version = buf[0] & 0xF;
	printk("PMBus version: 1.%d\n", pmbus_info[7].version);


	/* Get VOUT mode */
	ret = find_pmbus_property(PMB_CMD_VOUT_MODE);
	if (ret < 0) {
		goto propty_err;
	}
	property = &pmbus_cmd_property[ret];

	ret = pmbus_read_xfer(client, property, buf);
	if (ret < 0) {
		goto xfer_err;
	}

	pmbus_info[7].vout_mode = (buf[0] >> 5) & 0x07;
	if (buf[0] >> 5 == 0) {
		pmbus_info[7].vout_exponent = ((int8_t)(buf[0] << 3)) >> 3;
	} else {
		pmbus_info[7].vout_exponent = buf[0] & 0x1F;
	}


xfer_err:
propty_err:
	kfree(client);
	return ret;
}

static int pmbus_read_info(int devno)
{
	int ret;
	const struct pmbus_cmd_property_t *property;
	struct i2c_client *client;
	char buf[32];

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	client->adapter = adapter;
	client->addr = 0x58 + devno;
	client->flags = 0;

	/* Get PMBus version */
	ret = find_pmbus_property(PMB_CMD_PMBUS_REVISION);
	if (ret < 0) {
		goto propty_err;
	}

	property = &pmbus_cmd_property[ret];

	ret = pmbus_read_xfer(client, property, buf);
	if (ret < 0) {
		goto xfer_err;
	}

	pmbus_info[devno].version = buf[0] & 0xF;
	printk("PMBus version: 1.%d\n", pmbus_info[devno].version);


	/* Get VOUT mode */
	ret = find_pmbus_property(PMB_CMD_VOUT_MODE);
	if (ret < 0) {
		goto propty_err;
	}
	property = &pmbus_cmd_property[ret];

	ret = pmbus_read_xfer(client, property, buf);
	if (ret < 0) {
		goto xfer_err;
	}

	pmbus_info[devno].vout_mode = (buf[0] >> 5) & 0x07;
	if (buf[0] >> 5 == 0) {
		pmbus_info[devno].vout_exponent = ((int8_t)(buf[0] << 3)) >> 3;
	} else {
		pmbus_info[devno].vout_exponent = buf[0] & 0x1F;
	}



xfer_err:
propty_err:
	kfree(client);
	return ret;
}

static int __init pmbus_core_init(void)
{
	int i, ret, psu_item=0;
	struct i2c_adapter *tmp;

	mutex_init(&sw_mux_sem);
	for (i = 0; i <= 0xFF; i++) {
		tmp = i2c_get_adapter(i);
		if (tmp != NULL) {
			if (!strncmp("SMBus I801", tmp->name, 10)) {
				break;
			}
		}
	}

	if (i == 0x100) {
		return -ENODEV;
	}

	adapter = i2c_get_adapter(i);
	for (i = 0;i < 8; i++)
	{
		ret=pmbus_read_info(i);
		if (ret > 0) {
			psu_item++;
		}
	}

	if (!psu_item) {
		printk("[PMBus-Ctrl]: PSU modules not found.\n");
		goto err;
	}

	pmbus_fsp_default(DEFAULT_ADDRESS);

	ret = misc_register(&pmbus_dev);
	if (ret) {
		printk("[PMBus-Ctrl]: MISC device register fail. (Minor: %d)\n", pmbus_dev.minor);
		goto err;
	}

	printk("[PMBus-Ctrl]: Caswell PMBus Control Driver (%s) Initialized..., Delay= %d (msec).\n", DRV_VERSION, delay);
	return 0;
err:
	printk("[PMBus-Ctrl]: Loaded PMBUS controller drvier fail.\n");
	return -ENODEV;
}

static void __exit pmbus_core_exit(void)
{
	misc_deregister(&pmbus_dev);
}


MODULE_AUTHOR("Angus Cheng <angus.cheng@cas-well.com>");
MODULE_DESCRIPTION("CASwell PMBus Control Driver");
MODULE_LICENSE("GPL");

module_init(pmbus_core_init);
module_exit(pmbus_core_exit);

