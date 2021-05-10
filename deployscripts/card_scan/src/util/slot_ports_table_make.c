/*******************************************************************************

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "card_scan.h"

void slot_ports_table_make(slot_mapping_to_port_table_t *slot_mapping_to_port_table, net_t *net, short net_port_quantity) {
	int i;

	/* Initialization */
	slot_mapping_to_port_table->slot_name[0] = strdup("SLOT_A");
	slot_mapping_to_port_table->slot_name[1] = strdup("SLOT_B");
	slot_mapping_to_port_table->slot_name[2] = strdup("SLOT_C");
	slot_mapping_to_port_table->slot_name[3] = strdup("SLOT_D");
	slot_mapping_to_port_table->slot_name[4] = strdup("SLOT_E");

	if (slot_mapping_to_port_table->slot_name[0] == NULL || slot_mapping_to_port_table->slot_name[1] == NULL || slot_mapping_to_port_table->slot_name[2] == NULL || slot_mapping_to_port_table->slot_name[3] == NULL || slot_mapping_to_port_table->slot_name[4] == NULL) {
		printf("Not enough memory\n");
		exit(-1);
	}
	
	for (i = 0; i < SLOT_QUANTITY; i++) {
		slot_mapping_to_port_table->net_port_quantity[i] = 0;
	}
	
	/* Make table */
	for (i = 0; i < net_port_quantity; i++) {
		if (net[i].slot_name == NULL) {
			continue;
		}

		if (strcmp(net[i].slot_name, "SLOT_A") == 0) {
			(slot_mapping_to_port_table->net_port_quantity[0])++;
		} else if (strcmp(net[i].slot_name, "SLOT_B") == 0) {
			(slot_mapping_to_port_table->net_port_quantity[1])++;
		} else if (strcmp(net[i].slot_name, "SLOT_C") == 0) {
			(slot_mapping_to_port_table->net_port_quantity[2])++;
		} else if (strcmp(net[i].slot_name, "SLOT_D") == 0) {
			(slot_mapping_to_port_table->net_port_quantity[3])++;
		} else if (strcmp(net[i].slot_name, "SLOT_E") == 0) {
			(slot_mapping_to_port_table->net_port_quantity[4])++;
		}
	}
}
