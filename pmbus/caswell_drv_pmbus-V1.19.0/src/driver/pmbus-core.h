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

#ifndef PMBUS_CORE_H_
#define PMBUS_CORE_H_


#define DEFAULT_ADDRESS   0x25
#define DEFAULT_DEVNO     10
#define FSP_VALUE         1        /*  Represent 3Y default address value  */
#define IOCTL_DELAY       0        /*  0 msec default delay  */

enum PMB_VOUT_MODE {
	PMB_VOUT_LINEAR,
	PMB_VOUT_VID,
	PMB_VOUT_DIRECT,
};

struct pmbus_info_t {
	uint8_t version;
	uint8_t vout_mode;
	int vout_exponent;
};

enum PMB_DATA_TYPE {
	PMB_BYTE_DATA,
	PMB_WORD_DATA,
	PMB_BLOCK_DATA,
};

enum PMB_DATA_FORMAT {
	PMB_NONE_FMT,
	PMB_NUMC_FMT,
	PMB_STR_FMT,
	PMB_SENS_FMT,
	PMB_SENS_MILLI_FMT,
	PMB_SENS_MICRO_FMT,
	PMB_VOUT_FMT,
};

enum PMB_SENS_FMT_MODE {
	PMB_SENS_LINEAR_MODE,
	PMB_SENS_DIRECT_MODE,
};

struct pmbus_cmd_property_t {
	uint8_t cmd;
	uint8_t data_type;
	uint8_t data_len;
	uint8_t data_format;
	uint8_t ioctl_opcmd;
};

#define PMBUS_PRTY(c, t, l, f)   	\
	.cmd = c,                		\
	.data_type = t,                 \
	.data_len = l,					\
	.data_format = f

static const struct pmbus_cmd_property_t pmbus_cmd_property[] = {
	{ PMBUS_PRTY(PMB_CMD_PAGE, PMB_BYTE_DATA, 1, PMB_NONE_FMT) },
	{ PMBUS_PRTY(PMB_CMD_VOUT_MODE, PMB_BYTE_DATA, 1, PMB_NONE_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_WORD, PMB_WORD_DATA, 2, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_VOUT, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_IOUT, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_INPUT, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_TEMP, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_FAN12, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_STS_FAN34, PMB_BYTE_DATA, 1, PMB_NUMC_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_VIN, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_IIN, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_VCAP, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_VOUT, PMB_WORD_DATA, 2, PMB_VOUT_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_IOUT, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_TEMP_1, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_TEMP_2, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_TEMP_3, PMB_WORD_DATA, 2, PMB_SENS_MILLI_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_FAN_SPEED_1, PMB_WORD_DATA, 2, PMB_SENS_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_FAN_SPEED_2, PMB_WORD_DATA, 2, PMB_SENS_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_FAN_SPEED_3, PMB_WORD_DATA, 2, PMB_SENS_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_FAN_SPEED_4, PMB_WORD_DATA, 2, PMB_SENS_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_POUT, PMB_WORD_DATA, 2, PMB_SENS_MICRO_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_PIN, PMB_WORD_DATA, 2, PMB_SENS_MICRO_FMT) },
	{ PMBUS_PRTY(PMB_CMD_PMBUS_REVISION, PMB_BYTE_DATA, 1, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_ID, PMB_BLOCK_DATA, 8, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_MODEL, PMB_BLOCK_DATA, 8, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_REVISION, PMB_BLOCK_DATA, 8, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_SERIAL, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_MODEL_OPTION, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_FW_ID, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_FW_ID_OLD, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_FW_REVISION, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_MFR_FW_DATE, PMB_BLOCK_DATA, 16, PMB_STR_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_3V3_VOUT, PMB_WORD_DATA, 2, PMB_VOUT_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_5V_VOUT, PMB_WORD_DATA, 2, PMB_VOUT_FMT) },
	{ PMBUS_PRTY(PMB_CMD_READ_5VSB_VOUT, PMB_WORD_DATA, 2, PMB_VOUT_FMT) },
};

/* init_MUTEX macro was removed in 2.6.37 */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 36) && !defined(init_MUTEX)
#define init_MUTEX(sem) sema_init(sem, 1)
#endif

#endif /* PMBUS_CORE_H_ */
