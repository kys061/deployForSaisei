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

#ifndef PMBUS_CRTL_H_
#define PMBUS_CRTL_H_

#define PMBUS_CONF "/etc/cas-well/psu/pmbus.conf"

#ifndef EBUSY
#define EBUSY           16      /* Device or resource busy */
#endif

struct pmbus_conf_t {
	unsigned char module_present_mask;
	unsigned char sens_fmt;
	unsigned char page_fmt;
	unsigned char fsp_default_power;
	unsigned char fw_id_version;
	unsigned char vout_fmt;
	unsigned char psu_display;
	unsigned char mfr_info;
	unsigned char vol_source;
	unsigned char sup_vol_mask;
	unsigned char sup_word_mask;
	unsigned char sup_vout_mask;
	unsigned char sup_iout_mask;
	unsigned char sup_vin_mask;
	unsigned char sup_iin_mask;
	unsigned char sup_temp_mask[3];
	unsigned char sup_fan_mask[4];
	unsigned char sup_pout_mask;
	unsigned char sup_pin_mask;
	unsigned short sup_status_word_mask;
	unsigned char sup_status_vout_mask;
	unsigned char sup_status_iout_mask;
	unsigned char sup_status_input_mask;
	unsigned char sup_status_temp_mask;
	unsigned short sup_status_fan_mask;
};

#endif /* PMBUS_CRTL_H_ */
