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

#include <stdio.h>
#include <stdlib.h>

#include "card_scan.h"

/* Initialization of device_t structure */
void net_t_init(net_t *net)
{
	net->bus_root_info = NULL;
	net->address	= NULL;
	net->eth_name	= NULL;
	net->slot_name  = NULL;
	net->card_name  = NULL;
	net->device_id  = 0;
	net->vendor_id  = 0;
	net->bpid		= 0;
}

int net_num_get(device_t *device, int pci_dev_quantity)
{
	int i;
	int net_port_quantity = 0;

	for (i = 0; i < pci_dev_quantity; i++) {
		if (device[i].if_net == 1) {
			(net_port_quantity)++;
		}
	}

	return net_port_quantity;
}

void net_table_init(net_t **net_dev, int net_port_quantity)
{
    int i;
    net_t *net = NULL;

	*net_dev = (net_t *)malloc(sizeof(net_t) * net_port_quantity);
	net = *net_dev;

    if (net == NULL) {
        printf("Not enough memory\n");
        exit(-1);
    }

    for (i = 0; i < net_port_quantity; i++) {
        net_t_init(net + i);
    }
}

void net_table_make(device_t *device, net_t *net, int pci_dev_quantity, int net_port_quantity, mb_t *mb)
{
	int i, j, k, index;

	/* Creat network table */
	index = 0;

	for (j = 0; j < pci_dev_quantity; j++) {
		if (device[j].if_net == 1) {
			if (device[j].net.slot_name == NULL) {
				continue;
			}
			if (strcmp(device[j].net.slot_name, "ON_BOARD") == 0) {
					net[index].eth_name = strdup(device[j].net.eth_name);
					net[index].address = strdup(device[j].net.address);
					net[index].slot_name = strdup(device[j].net.slot_name);
					net[index].device_id = device[j].device_id;
					net[index].vendor_id = device[j].vendor_id;

					if (net[index].eth_name == NULL || net[index].address == NULL || net[index].slot_name == NULL) {
						printf("Not enough memory\n");
						exit(-1);
					}
					if (device[j].net.card_name != NULL) {
						net[index].card_name = strdup(device[j].net.card_name);
						if (net[index].card_name == NULL) {
							printf("Not enough memory\n");
							exit(-1);
						}
					}
					else {
						net[index].card_name = NULL;
					}
					index++;
			}
		}
	}

	/* index starts at no ON_BOARD */
	for (i = 0; i < MAX_BUS_INFO_NUM; i++) {
		for (j = 0; j < pci_dev_quantity; j++) {
			if (device[j].if_net == 1) {
				if (device[j].net.slot_name == NULL) {
					continue;
				}
				if (strcmp(device[j].net.slot_name, "ON_BOARD") != 0) {
					for (k = 0; k < SLOT_QUANTITY; k++) {
						if (mb->slot_bus_info[k].bus_info[i] == NULL || device[j].bus_root_info == NULL) {
							continue;
						}
						if (strcmp(mb->slot_bus_info[k].bus_info[i], device[j].bus_root_info) == 0) {
							net[index].bus_root_info = device[j].bus_root_info;
							net[index].eth_name = strdup(device[j].net.eth_name);
							net[index].address = strdup(device[j].net.address);
							net[index].slot_name = strdup(device[j].net.slot_name);
							net[index].device_id = device[j].device_id;
							net[index].vendor_id = device[j].vendor_id;
							if (net[index].eth_name == NULL || net[index].address == NULL || net[index].slot_name == NULL) {
								printf("Not enough memory\n");
								exit(-1);
							}
							if (device[j].net.card_name != NULL) {
								net[index].card_name = strdup(device[j].net.card_name);
								if (net[index].card_name == NULL) {
									printf("Not enough memory\n");
									exit(-1);
								}
							}
							else {
								net[index].card_name = NULL;
							}
							index++;
						}
					}
				}
			}
		}
	}
}
