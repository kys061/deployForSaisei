/******************************************************************************

  CASwell(R) Network Interface Card Scanning Linux utility
  Copyright(c) 2017 Ekko Chang <Ekko.Chang@cas-well.com>
  Copyright(c) 2013 Abel Wu <Abel.Wu@cas-well.com>

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

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <net/if.h>

#include "card_scan.h"

int main(int argc, char *argv[])
{
	device_t *device	= NULL;
	net_t *net			= NULL;
	slot_mapping_to_port_table_t slot_mapping_to_port_table;
	mb_t mb;
	command_info_t command_info;
	int pci_dev_quantity; // PCI device quantity
	int net_port_quantity; // Network ports quantity
	
	/* Parse command */
	command_parse(argv[0], argc, argv, &command_info);
	
	/* Load information of mother board for configuration, argv[optind] is mother board name */
	mb_info_load(argv[0], argv[optind], &mb, &command_info);
	/* Access /sys/bus/pci/devices/ device information */
	pci_dev_quantity = dev_access(&device, &mb);
	
	/* Set netports slots name */
	slot_name_set(device, &mb, pci_dev_quantity);
	
	/* Creat network devices table from Device structure */
	net_port_quantity = net_num_get(device, pci_dev_quantity);
	net_table_init(&net, net_port_quantity);
	net_table_make(device, net, pci_dev_quantity, net_port_quantity, &mb);
		
	/* Creat table of slot mapping network ports */
	slot_ports_table_make(&slot_mapping_to_port_table, net, net_port_quantity);
	
	/* Set card name */
	card_name_set(net, &slot_mapping_to_port_table, net_port_quantity);

	/* Show result */
	show(argc, argv, net, &mb, net_port_quantity, &command_info);

	/* Free memory */
	mem_free(device, pci_dev_quantity, net, net_port_quantity, &mb, &command_info);
	
	return 0;
}

