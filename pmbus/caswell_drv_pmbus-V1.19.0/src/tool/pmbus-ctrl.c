/******************************************************************************

  CASwell(R) PMBus Linux tool
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pmbus-ioctl.h>

#include "pmbus-ctrl.h"

//int pec_flag = 0;

void print_help(char *name)
{
	printf("CASwell PMBus Control Uitlity (%s)\n", DRV_VERSION);
	printf("Usage: %s [Option] [-m <Module Number>]\n", name);
	printf("\t-d <PMBus configuration file>\t\t: The path of PMBus configuration file. The default is /etc/cas-well/psu/pmbus.conf\n");
	printf("\t-c\t\t: Enable Hardware's PEC status\n");
	printf("\t-w\t\t: Show word status\n");
	printf("\t-v\t\t: Show output voltage\n");
	printf("\t-V\t\t: Show input voltage\n");
	printf("\t-i\t\t: Show output current\n");
	printf("\t-I\t\t: Show input current\n");
	printf("\t-t\t\t: Show temperature\n");
	printf("\t-f\t\t: Show fan speed\n");
	printf("\t-p\t\t: Show output power\n");
	printf("\t-P\t\t: Show input power\n");
	printf("\t-M\t\t: Show mfr information\n");
	printf("\t-m\t\t: Show information of specific module <0-7>\n");
	printf("\t-h\t\t: Show this message\n");

	exit(1);
}

int pmbus_read_sts(int fd, struct pmbus_iodata_t *pmbus_iodata)
{
	int ret;

retry:
	usleep(40000); /* Waiting 40 msec for retry */
	ret = ioctl(fd, IOCTL_PMBUS_READ_STS, pmbus_iodata);
	if (ret == EBUSY) {
		goto retry;
	}

	return ret;
}

int pmbus_write_sts(int fd, struct pmbus_iodata_t *pmbus_iodata)
{
	int ret;

retry:
	usleep(40000); /* Waiting 40 msec for retry */
	ret = ioctl(fd, IOCTL_PMBUS_WRITE_STS, pmbus_iodata);
	if (ret == EBUSY) {
		goto retry;
	}

	return ret;
}

char module_support(char *conf)
{
	char *tmp;
	char val = 0;

	tmp = strtok(conf, ",");
	val |= 1 << atoi(tmp);
	while ((tmp = strtok(NULL, ",")) != NULL) {
		val |= 1 << atoi(tmp);
	}

	return val;
}

int load_conf(struct pmbus_conf_t *pmbus_conf, char *file)
{
	int ret = 0;
	FILE *fd;
	char line[128];
	char *param, *content, *tmp;

	fd = fopen(file, "r");
	if (fd == NULL) {
		printf("File not found: %s\n", file);
		return 1;
	}

	while (fgets(line, 128, fd) != NULL) {
		if (line[0] == '#' || line[0] == 0x20 || line[0] == 0xA) {
			continue;
		}

		param = strtok(line, "=");

		if (!strcmp("MODULE_PRESENT_MASK", param)) {
			pmbus_conf->module_present_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SENS_FMT", param)) {
			pmbus_conf->sens_fmt = atoi(strtok(NULL, "="));
		} else if (!strcmp("PAGE_FMT", param)) {
			pmbus_conf->page_fmt = atoi(strtok(NULL, "="));
		} else if (!strcmp("FSP_DEFAULT_POWER", param)) {
			pmbus_conf->fsp_default_power = atoi(strtok(NULL, "="));
		} else if (!strcmp("FW_ID_VERSION", param)) {
			pmbus_conf->fw_id_version = atoi(strtok(NULL, "="));
		} else if (!strcmp("VOUT_FMT", param)) {
			pmbus_conf->vout_fmt = atoi(strtok(NULL, "="));
		} else if (!strcmp("PSU_DISPLAY", param)) {
			pmbus_conf->psu_display = atoi(strtok(NULL, "="));
		} else if (!strcmp("VOLTAGE_SOURCE", param)) {
			pmbus_conf->vol_source = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("MFR_INFO", param)) {
			pmbus_conf->mfr_info = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_WORD_MASK", param)) {
			pmbus_conf->sup_word_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_VOUT_MASK", param)) {
			pmbus_conf->sup_vout_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_IOUT_MASK", param)) {
			pmbus_conf->sup_iout_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_VIN_MASK", param)) {
			pmbus_conf->sup_vin_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_IIN_MASK", param)) {
			pmbus_conf->sup_iin_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_TEMP1_MASK", param)) {
			pmbus_conf->sup_temp_mask[0] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_TEMP2_MASK", param)) {
			pmbus_conf->sup_temp_mask[1] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_TEMP3_MASK", param)) {
			pmbus_conf->sup_temp_mask[2] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_FAN1_MASK", param)) {
			pmbus_conf->sup_fan_mask[0] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_FAN2_MASK", param)) {
			pmbus_conf->sup_fan_mask[1] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_FAN3_MASK", param)) {
			pmbus_conf->sup_fan_mask[2] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_FAN4_MASK", param)) {
			pmbus_conf->sup_fan_mask[3] = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_POUT_MASK", param)) {
			pmbus_conf->sup_pout_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_PIN_MASK", param)) {
			pmbus_conf->sup_pin_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_WORD_MASK", param)) {
			pmbus_conf->sup_status_word_mask = (short)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_VOUT_MASK", param)) {
			pmbus_conf->sup_status_vout_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_IOUT_MASK", param)) {
			pmbus_conf->sup_status_iout_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_INPUT_MASK", param)) {
			pmbus_conf->sup_status_input_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_TEMP_MASK", param)) {
			pmbus_conf->sup_status_temp_mask = (char)strtol(strtok(NULL, "="), NULL, 16);
		} else if (!strcmp("SUPPORT_STATUS_FAN_MASK", param)) {
			pmbus_conf->sup_status_fan_mask = (short)strtol(strtok(NULL, "="), NULL, 16);
		} else {
			printf("Parameter not supported: %s\n", param);
			ret = 1;
			goto err;
		}
	}

err:
	fclose(fd);
	return ret;
}

int set_page(int fd, struct pmbus_iodata_t *pmbus_iodata, char devno, int page)
{
	int ret = 0;

	pmbus_iodata->devno = devno;
	pmbus_iodata->data.value = page;
	pmbus_iodata->opcmd = PMB_CMD_PAGE;
	pmbus_iodata->pec = 0;

	ret = pmbus_write_sts(fd, pmbus_iodata);
	if (ret < 0) {
		printf("Failed to set page\n");
	}

	usleep(10000);

	return ret;

}

int clr_page(int fd, struct pmbus_conf_t *pmbus_conf, char devno, int page)
{
	int ret = 0;

	struct pmbus_iodata_t pmbus_iodata;

	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.data.value = page;
	pmbus_iodata.opcmd = PMB_CMD_PAGE;
	pmbus_iodata.pec = 0;

	ret = pmbus_write_sts(fd, &pmbus_iodata);
	if (ret < 0) {
		printf("Failed to clear page\n");
	}

	usleep(10000);

	return ret;
}

int get_5vsbvoltage(int fd, struct pmbus_conf_t *pmbus_conf, struct pmbus_iodata_t *pmbus_iodata, char devno, char pec_flag)
{
	int ret = 0;

	if (pmbus_conf->page_fmt == 1) {
		ret = set_page(fd, pmbus_iodata, devno, 0x20);

		if (ret < 0) {
			goto err;
		}

		pmbus_iodata->opcmd = PMB_CMD_READ_VOUT;
	} else {
		pmbus_iodata->opcmd = PMB_CMD_READ_5VSB_VOUT;
	}
	pmbus_iodata->devno = devno;
	pmbus_iodata->pec = pec_flag;

	ret = pmbus_read_sts(fd, pmbus_iodata);
	if (ret < 0) {
		printf("Failed to read 5VSB\n");
		goto err;
	}


err:
	return ret;
}

int get_3p3voltage(int fd, struct pmbus_conf_t *pmbus_conf, struct pmbus_iodata_t *pmbus_iodata, char devno, char pec_flag)
{
	int ret = 0;

	if (pmbus_conf->page_fmt == 1) {
		ret = set_page(fd, pmbus_iodata, devno, 0x11);

		if (ret < 0) {
			goto err;
		}

		pmbus_iodata->opcmd = PMB_CMD_READ_VOUT;
	} else {
		pmbus_iodata->opcmd = PMB_CMD_READ_3V3_VOUT;
	}

	pmbus_iodata->devno = devno;
	pmbus_iodata->pec = pec_flag;

	ret = pmbus_read_sts(fd, pmbus_iodata);
	if (ret < 0) {
		printf("Failed to read 3.3V\n");
		goto err;
	}


err:
	return ret;
}

int get_5voltage(int fd, struct pmbus_conf_t *pmbus_conf, struct pmbus_iodata_t *pmbus_iodata, char devno, char pec_flag)
{
	int ret = 0;

	if (pmbus_conf->page_fmt == 1) {
		ret = set_page(fd, pmbus_iodata, devno, 0x10);

		if (ret < 0) {
			goto err;
		}

		pmbus_iodata->opcmd = PMB_CMD_READ_VOUT;
	} else {
		pmbus_iodata->opcmd = PMB_CMD_READ_5V_VOUT;
	}

	pmbus_iodata->devno = devno;
	pmbus_iodata->pec = pec_flag;

	ret = pmbus_read_sts(fd, pmbus_iodata);
	if (ret < 0) {
		printf("Failed to read 5V\n");
		goto err;
	}


err:
	return ret;
}

int get_12voltage(int fd, struct pmbus_conf_t *pmbus_conf, struct pmbus_iodata_t *pmbus_iodata, char devno, char pec_flag)
{
	int ret = 0;

	if (pmbus_conf->page_fmt == 1) {
		ret = set_page(fd, pmbus_iodata, devno, 0x00);

		if (ret < 0) {
			goto err;
		}
	}

	pmbus_iodata->devno = devno;
	pmbus_iodata->opcmd = PMB_CMD_READ_VOUT;
	pmbus_iodata->pec = pec_flag;

	ret = pmbus_read_sts(fd, pmbus_iodata);
	if (ret < 0) {
		printf("Failed to read 12V\n");
		goto err;
	}


err:
	return ret;
}

int show_vout_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{

	int i, ret = 0;
	char *type;

	struct pmbus_iodata_t pmbus_iodata;

	memset(&pmbus_iodata, 0, sizeof(pmbus_iodata));

	if (pmbus_conf->sup_vout_mask & (1 << devno)) {
		if (devno == 7) {
			pmbus_iodata.backplane = pmbus_conf->fsp_default_power;

			for (i = 0; i < 4; i++) {
				if (pmbus_conf-> vol_source & (1 << i)) {
					if (i == 0) {
						ret = get_12voltage(fd, pmbus_conf, &pmbus_iodata, devno, pec_flag);
						type = "12V";
					}
					if (i == 1) {
						ret = get_5voltage(fd, pmbus_conf, &pmbus_iodata, devno, pec_flag);
						type = "5V";
					}
					if (i == 2) {
						ret = get_3p3voltage(fd, pmbus_conf, &pmbus_iodata, devno, pec_flag);
						type = "3.3V";
					}
					if (i == 3) {
						ret = get_5vsbvoltage(fd, pmbus_conf, &pmbus_iodata, devno, pec_flag);
						type = "5VSB";
					}

					printf("Output Voltage (%s): ", type);
					if (ret == 0) {
						printf("%s%d.%03d (V)\n",
							(pmbus_iodata.data.value < 0) ? "-" : "",
							abs(pmbus_iodata.data.value) / 1000,
							abs(pmbus_iodata.data.value) % 1000);
					} else {
						printf("Failed to access\n");
					}
					if (pmbus_conf->page_fmt) {
						clr_page(fd, pmbus_conf, devno, 0x00);
					}
				}
			}
		} else {
			ret = get_12voltage(fd, pmbus_conf, &pmbus_iodata, devno, pec_flag);
			type = "12V";
			printf("Output Voltage (%s): ", type);
			if (ret == 0) {
				printf("%s%d.%03d (V)\n",
					(pmbus_iodata.data.value < 0) ? "-" : "",
					abs(pmbus_iodata.data.value) / 1000,
					abs(pmbus_iodata.data.value) % 1000);
			} else {
				printf("Failed to access\n");
			}
			if (pmbus_conf->page_fmt) {
				clr_page(fd, pmbus_conf, devno, 0x00);
			}
		}
	} else {
		printf("Output Voltage: Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_vin_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("Input Voltage: ");
	if (pmbus_conf->sup_vin_mask & (1 << devno)) {

		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_READ_VIN;
		pmbus_iodata.pec = pec_flag;

		ret = pmbus_read_sts(fd, &pmbus_iodata);
		if (ret == 0) {
			printf("%s%d.%d (V)\n",
			(pmbus_iodata.data.value < 0) ? "-" : "",
			abs(pmbus_iodata.data.value) / 1000,
			abs(pmbus_iodata.data.value) % 1000);
		} else {
			printf("Failed to access\n");
		}
	} else {
		printf("Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_iout_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("Output Current: ");
	if (pmbus_conf->sup_iout_mask & (1 << devno)) {

		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_READ_IOUT;
		pmbus_iodata.pec = pec_flag;

		ret = pmbus_read_sts(fd, &pmbus_iodata);
		if (ret == 0) {
			printf("%s%d.%d (A)\n",
			(pmbus_iodata.data.value < 0) ? "-" : "",
			abs(pmbus_iodata.data.value) / 1000,
			abs(pmbus_iodata.data.value) % 1000);
		} else {
			printf("Failed to access\n");
		}
	} else {
		printf("Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_iin_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("Input Current: ");
	if (pmbus_conf->sup_iin_mask & (1 << devno)) {

		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_READ_IIN;
		pmbus_iodata.pec = pec_flag;

		ret = pmbus_read_sts(fd, &pmbus_iodata);
		if (ret == 0) {
			printf("%s%d.%d (A)\n",
			(pmbus_iodata.data.value < 0) ? "-" : "",
			abs(pmbus_iodata.data.value) / 1000,
			abs(pmbus_iodata.data.value) % 1000);
		} else {
			printf("Failed to access\n");
		}
	} else {
		printf("Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_temp_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	for (i = 0; i < 3; i++) {
		if (pmbus_conf->sup_temp_mask[i] & (1 << devno)) {
			printf("Temperature %d: ", (i + 1));
			count++;

			pmbus_iodata.devno = devno;
			pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
			pmbus_iodata.opcmd = PMB_CMD_READ_TEMP_1 + i;
			pmbus_iodata.pec = pec_flag;

			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				printf("%s%d.%d (Cel.)\n",
				(pmbus_iodata.data.value < 0) ? "-" : "",
				abs(pmbus_iodata.data.value) / 1000,
				abs(pmbus_iodata.data.value) % 1000);
			} else {
				printf("Failed to access\n");
			}
		}
	}
	if (count == 0) {
		printf("Temperature sensor : Not support\n");
	}

	printf("\n");
	return ret;
}

int show_fan_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	for (i = 0; i < 4; i++) {
		if (pmbus_conf->sup_fan_mask[i] & (1 << devno)) {
			printf("Fan Speed %d: ", (i + 1));
			count++;

			pmbus_iodata.devno = devno;
			pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
			pmbus_iodata.opcmd = PMB_CMD_READ_FAN_SPEED_1 + i;
			pmbus_iodata.pec = pec_flag;

			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				printf("%ld (PRM)\n", pmbus_iodata.data.value);
			} else {
				printf("Failed to access\n");
			}
		}
	}
	if (count == 0) {
		printf("Fan Speed : Not support\n");
	}

	printf("\n");
	return ret;
}

int show_pout_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("Output Power: ");
	if (pmbus_conf->sup_pout_mask & (1 << devno)) {

		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_READ_POUT;
		pmbus_iodata.pec = pec_flag;

		ret = pmbus_read_sts(fd, &pmbus_iodata);
		if (ret == 0) {
			printf("%s%d.%d (W)\n",
			(pmbus_iodata.data.value < 0) ? "-" : "",
			abs(pmbus_iodata.data.value) / 1000000,
			abs(pmbus_iodata.data.value) % 1000000);
		} else {
			printf("Failed to access\n");
		}
	} else {
		printf("Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_pin_value(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("Input Power: ");
	if (pmbus_conf->sup_pin_mask & (1 << devno)) {

		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_READ_PIN;
		pmbus_iodata.pec = pec_flag;

		ret = pmbus_read_sts(fd, &pmbus_iodata);
		if (ret == 0) {
			printf("%s%d.%d (W)\n",
			(pmbus_iodata.data.value < 0) ? "-" : "",
			abs(pmbus_iodata.data.value) / 1000000,
			abs(pmbus_iodata.data.value) % 1000000);
		} else {
			printf("Failed to access\n");
		}
	} else {
		printf("Not Supported\n");
	}

	printf("\n");
	return ret;
}

int show_mfr_id(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_ID: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_ID;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_model(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_MODEL: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_MODEL;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_revision(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_REVISION: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_REVISION;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_model_option(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_MODEL_OPTION: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_MODEL_OPTION;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	printf("\n");
	return ret;
}

int show_mfr_fwid(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_FW_ID: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.pec = pec_flag;

	if (pmbus_conf->fw_id_version == 0) {
		pmbus_iodata.opcmd = PMB_CMD_MFR_FW_ID_OLD;
	} else {
		pmbus_iodata.opcmd = PMB_CMD_MFR_FW_ID;
	}

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_fw_revision(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_FW_REVISION: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_FW_REVISION;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_fwdate(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_FW_DATE: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_FW_DATE;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_mfr_serial(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("MFR_SERIAL: ");
	pmbus_iodata.devno = devno;
	pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
	pmbus_iodata.opcmd = PMB_CMD_MFR_SERIAL;
	pmbus_iodata.pec = pec_flag;

	ret = pmbus_read_sts(fd, &pmbus_iodata);

	if (ret == 0) {
		printf("%s\n",pmbus_iodata.data.string);
	} else {
		printf("Failed to access\n");
	}

	return ret;
}

int show_word_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	printf("WORD Status: ");
	if (pmbus_conf->sup_word_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_WORD;
		pmbus_iodata.pec = pec_flag;

		if (pmbus_conf->sup_status_word_mask & 0xFFFF) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 15; i > 0; i--) {
					if (pmbus_conf->sup_status_word_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count++;
						if (i == 13) {
							printf("\tInput Status Fault or Warning - %s\n",
								(pmbus_iodata.data.value & 0x2000) ? "Yes" : "No");
						}
						if (i == 11) {
							printf("\tPOWER_GOOD signal is negated - %s\n",
								(pmbus_iodata.data.value & 0x0800) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tPower Not Being enabled - %s\n",
								(pmbus_iodata.data.value & 0x0040) ? "Yes" : "No");
						}
						if (i == 3) {
							printf("\tInput Under Voltage Fault Occurred - %s\n",
								(pmbus_iodata.data.value & 0x0008) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	} else {
		printf("Not Supported\n");
	}
out:
	printf("\n");
	return ret;
}

int show_vout_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_vout_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_VOUT;
		pmbus_iodata.pec = pec_flag;

		printf("VOUT Status: ");
		if (pmbus_conf->sup_status_vout_mask & 0xF0) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 7; i > 3; i--) {
					if (pmbus_conf->sup_status_vout_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count++;
						if (i == 7) {
							printf("\tVOUT Over Voltage Fault - %s\n",
								(pmbus_iodata.data.value & 0x80) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tVOUT Over Voltage Warning - %s\n",
								(pmbus_iodata.data.value & 0x40) ? "Yes" : "No");
						}
						if (i == 5) {
							printf("\tVOUT Under Voltage Warning - %s\n",
								(pmbus_iodata.data.value & 0x20) ? "Yes" : "No");
						}
						if (i == 4) {
							printf("\tVOUT Under Voltage Fault - %s\n",
								(pmbus_iodata.data.value & 0x10) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_vin_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_vin_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_INPUT;
		pmbus_iodata.pec = pec_flag;

		printf("VIN Status: ");
		if (pmbus_conf->sup_status_input_mask & 0xF0) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 7; i > 3; i--) {
					if (pmbus_conf->sup_status_input_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count++;
						if (i == 7) {
							printf("\tVIN Over Voltage Fault - %s\n",
								(pmbus_iodata.data.value & 0x80) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tVIN Over Voltage Warning - %s\n",
								(pmbus_iodata.data.value & 0x40) ? "Yes" : "No");
						}
						if (i == 5) {
							printf("\tVIN Under Voltage Warning - %s\n",
								(pmbus_iodata.data.value & 0x20) ? "Yes" : "No");
						}
						if (i == 4) {
							printf("\tVIN Under Voltage Fault - %s\n",
								(pmbus_iodata.data.value & 0x10) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_iout_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_iout_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_IOUT;
		pmbus_iodata.pec = pec_flag;

		printf("IOUT Status: ");
		if (pmbus_conf->sup_status_iout_mask & 0xF0) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 7; i > 3; i--) {
					if (pmbus_conf->sup_status_iout_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 7) {
							printf("\tIOUT Over Current Fault - %s\n",
								(pmbus_iodata.data.value & 0x80) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tIOUT Over Current and Low Voltage Shutdown Fault - %s\n",
								(pmbus_iodata.data.value & 0x40) ? "Yes" : "No");
						}
						if (i == 5) {
							printf("\tIOUT Over Current Warning - %s\n",
								(pmbus_iodata.data.value & 0x20) ? "Yes" : "No");
						}
						if (i == 4) {
							printf("\tIOUT Under Voltage Fault - %s\n",
								(pmbus_iodata.data.value & 0x10) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_iin_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_iin_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_INPUT;
		pmbus_iodata.pec = pec_flag;

		printf("IIN Status: ");
		if (pmbus_conf->sup_status_input_mask & 0x06) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 2; i > 0; i--) {
					if (pmbus_conf->sup_status_input_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 2) {
							printf("\tIIN Over Current Fault - %s\n",
								(pmbus_iodata.data.value & 0x04) ? "Yes" : "No");
						}
						if (i == 1) {
							printf("\tIIN Over Current Warning - %s\n",
								(pmbus_iodata.data.value & 0x02) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_temp_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_temp_mask[0] & (1 << devno) ||
		(pmbus_conf->sup_temp_mask[1] & (1 << devno)) ||
		(pmbus_conf->sup_temp_mask[2] & (1 << devno))) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_TEMP;
		pmbus_iodata.pec = pec_flag;

		printf("Temperature Status: ");
		if (pmbus_conf->sup_status_temp_mask & 0xF0) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 7; i > 3; i--) {
					if (pmbus_conf->sup_status_temp_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 7) {
							printf("\tOver Temperature Fault - %s\n",
								(pmbus_iodata.data.value & 0x80) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tOver Temperature Warning - %s\n",
								(pmbus_iodata.data.value & 0x40) ? "Yes" : "No");
						}
						if (i == 5) {
							printf("\tUnder Temperature Warning - %s\n",
								(pmbus_iodata.data.value & 0x20) ? "Yes" : "No");
						}
						if (i == 4) {
							printf("\tUnder Temperature Fault - %s\n",
								(pmbus_iodata.data.value & 0x10) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}

		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_fan_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_fan_mask[0] & (1 << devno) ||
		(pmbus_conf->sup_fan_mask[1] & (1 << devno)) ||
		(pmbus_conf->sup_fan_mask[2] & (1 << devno)) ||
		(pmbus_conf->sup_fan_mask[3] & (1 << devno))) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_FAN12;
		pmbus_iodata.pec = pec_flag;

		printf("Fan Status: ");

		if (pmbus_conf->sup_status_fan_mask & 0xFC) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 7; i > 1; i--) {
					if (pmbus_conf->sup_status_fan_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 7) {
							printf("\tFan 1 Fault - %s\n",
								(pmbus_iodata.data.value & 0x80) ? "Yes" : "No");
						}
						if (i == 6) {
							printf("\tFan 2 Fault - %s\n",
								(pmbus_iodata.data.value & 0x40) ? "Yes" : "No");
						}
						if (i == 5) {
							printf("\tFan 1 Warning - %s\n",
								(pmbus_iodata.data.value & 0x20) ? "Yes" : "No");
						}
						if (i == 4) {
							printf("\tFan 2 Waring - %s\n",
								(pmbus_iodata.data.value & 0x10) ? "Yes" : "No");
						}
						if (i == 3) {
							printf("\tFan 1 Speed Overridden - %s\n",
								(pmbus_iodata.data.value & 0x08) ? "Yes" : "No");
						}
						if (i == 2) {
							printf("\tFan 2 Speed Overridden - %s\n",
								(pmbus_iodata.data.value & 0x04) ? "Yes" : "No");
						}
					}
				}
			}
		}
		if (pmbus_conf->sup_status_fan_mask & 0xFC00) {
			pmbus_iodata.opcmd = PMB_CMD_STS_FAN34;
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 15; i > 9; i--) {
					if (pmbus_conf->sup_status_fan_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 15) {
							printf("\tFan 3 Fault - %s\n",
								(pmbus_iodata.data.value & 0x8000) ? "Yes" : "No");
						}
						if (i == 14) {
							printf("\tFan 4 Fault - %s\n",
								(pmbus_iodata.data.value & 0x4000) ? "Yes" : "No");
						}
						if (i == 13) {
							printf("\tFan 3 Warning - %s\n",
								(pmbus_iodata.data.value & 0x2000) ? "Yes" : "No");
						}
						if (i == 12) {
							printf("\tFan 4 Waring - %s\n",
								(pmbus_iodata.data.value & 0x1000) ? "Yes" : "No");
						}
						if (i == 11) {
							printf("\tFan 3 Speed Overridden - %s\n",
								(pmbus_iodata.data.value & 0x0800) ? "Yes" : "No");
						}
						if (i == 10) {
							printf("\tFan 4 Speed Overridden - %s\n",
								(pmbus_iodata.data.value & 0x0400) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}
		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_pout_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_pout_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_IOUT;
		pmbus_iodata.pec = pec_flag;

		printf("POUT Status: ");
		if (pmbus_conf->sup_status_iout_mask & 0x3) {
			ret = pmbus_read_sts(fd, &pmbus_iodata);
			if (ret == 0) {
				for (i = 1; i >= 0; i--) {
					if (pmbus_conf->sup_status_iout_mask & (1 << i)) {
						if (count == 0) {
							printf("\n");
						}
						count ++;
						if (i == 1) {
							printf("\tPOUT Over Power Fault - %s\n",
								(pmbus_iodata.data.value & 0x02) ? "Yes" : "No");
						}
						if (i == 0) {
							printf("\tPOUT Over Power Warning - %s\n",
								(pmbus_iodata.data.value & 0x01) ? "Yes" : "No");
						}
					}
				}
			} else {
				printf("Error to get status.\n");
				goto out;
			}
		}

		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int show_pin_status(int fd, struct pmbus_conf_t *pmbus_conf, char devno, char pec_flag)
{
	int ret = 0, i = 0, count = 0;
	struct pmbus_iodata_t pmbus_iodata;

	if (pmbus_conf->sup_pin_mask & (1 << devno)) {
		pmbus_iodata.devno = devno;
		pmbus_iodata.backplane = pmbus_conf->fsp_default_power;
		pmbus_iodata.opcmd = PMB_CMD_STS_INPUT;
		pmbus_iodata.pec = pec_flag;

		printf("PIN Status: ");
		if (pmbus_conf->sup_status_input_mask & 0x1) {
				ret = pmbus_read_sts(fd, &pmbus_iodata);
				if (ret == 0) {
					if (count == 0) {
						printf("\n");
					}
					count ++;
					printf("\tPIN Over Power Warning - %s\n",
						(pmbus_iodata.data.value & 0x01) ? "Yes" : "No");
				}
		}

		if (count == 0) {
			printf("Not support\n");
		}
	}

out:
	printf("\n");
	return ret;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int module_number = 0, flag_count = 0;
	int ret, fd, i, j;
	char c;
	char pec_flag = 0, module_flag = 0, word_flag = 0, vout_flag = 0, vin_flag = 0, iout_flag = 0, iin_flag = 0, temp_flag = 0, fan_flag = 0, pout_flag = 0, pin_flag = 0, mfr_flag = 0;
	char conf[128];
	struct pmbus_conf_t pmbus_conf;
	struct pmbus_iodata_t pmbus_iodata;

	sprintf(conf, "%s", PMBUS_CONF);

	while ((c = getopt(argc, argv, "d:m:cwvViItfpPMh")) != -1) {
		switch (c) {
		case 'd':
			sprintf(conf, "%s", optarg);
			break;
		case 'm':
			module_flag = 1;
			module_number = atoi(optarg);
			break;
		case 'c':
			pec_flag = 1;
			break;
		case 'w':
			word_flag = 1;
			flag_count++;
			break;
		case 'v':
			vout_flag = 1;
			flag_count++;
			break;
		case 'V':
			vin_flag = 1;
			flag_count++;
			break;
		case 'i':
			iout_flag = 1;
			flag_count++;
			break;
		case 'I':
			iin_flag = 1;
			flag_count++;
			break;
		case 't':
			temp_flag = 1;
			flag_count++;
			break;
		case 'f':
			fan_flag = 1;
			flag_count++;
			break;
		case 'p':
			pout_flag = 1;
			flag_count++;
			break;
		case 'P':
			pin_flag = 1;
			flag_count++;
			break;
		case 'M':
			mfr_flag = 1;
			flag_count++;
			break;
		case 'h':
		default:
			print_help(argv[0]);
		}
	}

	memset(&pmbus_conf, 0, sizeof(struct pmbus_conf_t));

	ret = load_conf(&pmbus_conf, conf);
	if (ret) {
		exit(1);
	}

#ifdef DEBUG
	printf("module_present=%x\n", pmbus_conf.module_present_mask);
	printf("sens_fmt=%x\n", pmbus_conf.sens_fmt);
	printf("vout_fmt=%x\n", pmbus_conf.vout_fmt);
	printf("sup_vout=%x\n", pmbus_conf.sup_vout_mask);
	printf("sup_iout=%x\n", pmbus_conf.sup_iout_mask);
	printf("sup_vin=%x\n", pmbus_conf.sup_vin_mask);
	printf("sup_iin=%x\n", pmbus_conf.sup_iin_mask);
	printf("sup_temp1=%x\n", pmbus_conf.sup_temp_mask[0]);
	printf("sup_temp2=%x\n", pmbus_conf.sup_temp_mask[1]);
	printf("sup_temp3=%x\n", pmbus_conf.sup_temp_mask[2]);
	printf("sup_fan1=%x\n", pmbus_conf.sup_fan_mask[0]);
	printf("sup_fan2=%x\n", pmbus_conf.sup_fan_mask[1]);
	printf("sup_fan3=%x\n", pmbus_conf.sup_fan_mask[2]);
	printf("sup_fan4=%x\n", pmbus_conf.sup_fan_mask[3]);
	printf("sup_pout=%x\n", pmbus_conf.sup_pout_mask);
	printf("sup_pin=%x\n", pmbus_conf.sup_pin_mask);
#endif

	fd = open(DEVNAME, O_RDWR);
	if (fd < 0) {
		printf("Can't open %s\n", DEVNAME);
		return 1;
	}

retry:
	if ((fp = fopen(LOCKFILE, "w+")) == 0) {
		printf("Can't open %s\n", LOCKFILE);
		return 1;
	}

	ret = flock(fileno(fp), LOCK_EX | LOCK_NB);
	if (ret < 0) {
		usleep(5000);
		fclose(fp);
		goto retry;
	}

	for (i = 0; i < 8; i++) {

		if (module_flag && i != module_number) {
			continue;
		}

		if (!flag_count) {
			goto err;
		}

		if (pmbus_conf.module_present_mask & (1 << i)) {
			if (i == 7) {
				printf("[Backplane]\n\n");
			} else {
				printf("[Module %d]\n\n", i);
				/* Radware power "M1P2-5420V4V"
				 * change module sequence */
				if (pmbus_conf.psu_display == 1) {
					if (i == 0) {
						i = i + 1;
					} else if (i == 1) {
						i = i - 1;
					}
				}
			}

			if (pec_flag) {
				printf("Hardware's PEC: Enabled\n\n");
			}

			if (word_flag) {
				ret = show_word_status(fd, &pmbus_conf, i, pec_flag);
			}

			if (vout_flag) {
				ret = show_vout_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_vout_status(fd, &pmbus_conf, i, 0);
			}

			if (vin_flag) {
				ret = show_vin_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_vin_status(fd, &pmbus_conf, i, 0);
			}

			if (iout_flag) {
				ret = show_iout_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_iout_status(fd, &pmbus_conf, i, 0);
			}

			if (iin_flag) {
				ret = show_iin_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_iin_status(fd, &pmbus_conf, i, 0);
			}

			if (temp_flag) {
				ret = show_temp_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_temp_status(fd, &pmbus_conf, i, 0);
			}

			if (fan_flag) {
				ret = show_fan_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_fan_status(fd, &pmbus_conf, i, 0);
			}

			if (pout_flag) {
				ret = show_pout_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_pout_status(fd, &pmbus_conf, i, 0);
			}

			if (pin_flag) {
				ret = show_pin_value(fd, &pmbus_conf, i, pec_flag);
				ret = show_pin_status(fd, &pmbus_conf, i, 0);
			}

			if (mfr_flag) {
				for (j = 0; j < 8; j++) {
					if (pmbus_conf.mfr_info & (1 << j)) {
						switch (j) {
						case 0:
							ret = show_mfr_id(fd, &pmbus_conf, i, 0);
							break;
						case 1:
							ret = show_mfr_model(fd, &pmbus_conf, i, 0);
							break;
						case 2:
							ret = show_mfr_revision(fd, &pmbus_conf, i, 0);
							break;
						case 3:
							ret = show_mfr_model_option(fd, &pmbus_conf, i, 0);
							break;
						case 4:
							ret = show_mfr_fwid(fd, &pmbus_conf, i, 0);
							break;
						case 5:
							ret = show_mfr_fw_revision(fd, &pmbus_conf, i, 0);
							break;
						case 6:
							ret = show_mfr_fwdate(fd, &pmbus_conf, i, 0);
							break;
						case 7:
							ret = show_mfr_serial(fd, &pmbus_conf, i, 0);
							break;
						default:
							goto err;
						}
					}
				}
				printf("\n");
			}
			/* Radware power "M1P2-5420V4V"
			 * change module sequence */
			if (pmbus_conf.psu_display == 1) {
				if (i == 1) {
					i = 0;
				} else if (i == 0) {
					i = 1;
				}
			}

		}
	}

err:
	ret = flock(fileno(fp), LOCK_UN);
	fclose(fp);
	close(fd);
	return ret;
}
