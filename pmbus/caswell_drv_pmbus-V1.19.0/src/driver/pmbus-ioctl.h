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

#include <linux/ioctl.h>

#ifndef PMBUS_IOCTL_H_
#define PMBUS_IOCTL_H_

#define MAX_COMMAND_DATA	32

#define DEVNAME "/dev/pmbus_ctrl"
#define LOCKFILE "/etc/.pmbus.lock"
#define DRV_VERSION     "V1.19.0"

enum PMB_CMD {
	PMB_CMD_PAGE = 0x00,
	PMB_CMD_VOUT_MODE = 0x20,
	PMB_CMD_STS_WORD = 0x79,
	PMB_CMD_STS_VOUT = 0x7A,
	PMB_CMD_STS_IOUT = 0x7B,
	PMB_CMD_STS_INPUT = 0x7C,
	PMB_CMD_STS_TEMP = 0x7D,
	PMB_CMD_STS_FAN12 = 0x81,
	PMB_CMD_STS_FAN34,
	PMB_CMD_READ_VIN = 0x88,
	PMB_CMD_READ_IIN = 0x89,
	PMB_CMD_READ_VCAP = 0x8A,
	PMB_CMD_READ_VOUT = 0x8B,
	PMB_CMD_READ_IOUT = 0x8C,
	PMB_CMD_READ_TEMP_1 = 0x8D,
	PMB_CMD_READ_TEMP_2 = 0x8E,
	PMB_CMD_READ_TEMP_3 = 0x8F,
	PMB_CMD_READ_FAN_SPEED_1 = 0x90,
	PMB_CMD_READ_FAN_SPEED_2 = 0x91,
	PMB_CMD_READ_FAN_SPEED_3 = 0x92,
	PMB_CMD_READ_FAN_SPEED_4 = 0x93,
	PMB_CMD_READ_DUTY_CYCLE = 0x94,
	PMB_CMD_READ_FREQUENCY = 0x95,
	PMB_CMD_READ_POUT = 0x96,
	PMB_CMD_READ_PIN = 0x97,
	PMB_CMD_PMBUS_REVISION = 0x98,
	PMB_CMD_MFR_ID = 0x99,
	PMB_CMD_MFR_MODEL = 0x9A,
	PMB_CMD_MFR_REVISION = 0x9B,
	PMB_CMD_MFR_SERIAL = 0x9E,
	PMB_CMD_MFR_FW_ID = 0xAE,
	PMB_CMD_MFR_MODEL_OPTION = 0xD0,
	PMB_CMD_MFR_FW_ID_OLD = 0xD1,
	PMB_CMD_MFR_FW_REVISION = 0xD2,
	PMB_CMD_MFR_FW_DATE = 0xD4,
	PMB_CMD_READ_3V3_VOUT = 0xE0,
	PMB_CMD_READ_5V_VOUT = 0xE3,
	PMB_CMD_READ_5VSB_VOUT = 0xE6,
	PMB_CMD_READ_3V3_IOUT = 0xF0,
	PMB_CMD_READ_5V_IOUT = 0xF1,
};

union pmb_data_u {
	long value;
	unsigned char string[MAX_COMMAND_DATA];
};

struct pmbus_iodata_t {
	unsigned short pec;
	unsigned short backplane;
	unsigned char devno;
	unsigned short opcmd;
	union pmb_data_u data;
};

#define IOC_PMB 'p'

#define IOCTL_PMBUS_READ_STS  _IOR(IOC_PMB, 1, struct pmbus_iodata_t)
#define IOCTL_PMBUS_WRITE_STS  _IOW(IOC_PMB, 2, struct pmbus_iodata_t)

#endif /* PMBUS_IOCTL_H_ */
