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
#include <string.h>

#include "card_scan.h"

void information_parse(net_t net, char *output)
{
	if (output == NULL) {
		printf("%-13s %-30s %-18s %-20s\n", net.eth_name, net.card_name, net.slot_name, net.address);
	} else if (strcmp(output, "slot") == 0) {
		printf("%s\n", net.slot_name);
	} else if (strcmp(output, "card") == 0) {
		printf("%s\n", net.card_name);
	} else if (strcmp(output, "iface") == 0) {
		printf("%s\n", net.eth_name);
	} else if (strcmp(output, "mac") == 0) {
		 printf("%s\n", net.address);
	}
}

void show_all_netports(net_t *net, int net_port_quantity)
{
	int i;

	for (i = 0; i < net_port_quantity; i++) {
		if (net[i].slot_name == NULL) {
			continue;
		}
		printf("%-13s %-30s %-18s %-20s\n", net[i].eth_name, net[i].card_name, net[i].slot_name, net[i].address);
	}
}

void sort_net_data(net_t *net, int net_port_quantity)
{
	int i, j;
	net_t temp;

	for (i = 0; i < net_port_quantity -1; i++) {
		for (j = i + 1; j < net_port_quantity; j++) {
			if ( (net[i].slot_name != NULL) && (net[j].slot_name != NULL) ) {
				if ( strcmp(net[i].slot_name, net[j].slot_name) > 0 ) {
					temp = net[i];
					net[i] = net[j];
					net[j] = temp;
				}
			}
		}
	}
}

void show(int argc, char *argv[], net_t *net, mb_t *mb, int net_port_quantity, command_info_t *command_info)
{
	int i;

	/* To sort data */
	sort_net_data(net, net_port_quantity);

	/* Show total network information */
	if (argc == 2 || (argc == 4 && command_info->config_path != NULL)) {
		show_all_netports(net, net_port_quantity);
    }

	/* Rename network ports or generate udev setting file */
	if (command_info->if_rename == 1) { 
		network_rename(argv[0], net, mb, net_port_quantity, 'r', NULL, command_info->config_path);
	}
	if (command_info->gen_path != NULL) {
		network_rename(argv[0], net, mb, net_port_quantity, 'g', command_info->gen_path, command_info->config_path);
	}
		
	/* option : -c  or -c -o  */
	if (command_info->card != NULL) {
		for (i = 0; i < net_port_quantity; i++) {
			if (net[i].slot_name == NULL) {
				continue;
			}

			if (strcmp(command_info->card, net[i].card_name) == 0) {
				if (command_info->output == NULL) {
					information_parse(net[i], command_info->output);
				} else if (strcmp(command_info->output, "slot") == 0) {
					information_parse(net[i], command_info->output); // only print one time
					break;
				} else {
					information_parse(net[i], command_info->output); // only print one time
				}
			}
		}	
	}
	
	/* option : -s  or -s -o  */
	if (command_info->slot != NULL) {
		for (i = 0; i < net_port_quantity; i++) {
			if (net[i].slot_name == NULL) {
				continue;
			}

			if (strcmp(command_info->slot, net[i].slot_name) == 0) {
				if (command_info->output == NULL) {
					information_parse(net[i], command_info->output);
				} else if (strcmp(command_info->output, "card") == 0) {
					information_parse(net[i], command_info->output); // only print one time
					break;
				} else {
					information_parse(net[i], command_info->output);
				}
			}
		}
	}

	/* option : -i  or -i -o  */
	if (command_info->iface != NULL) {
		for (i = 0; i < net_port_quantity; i++) {
			if (net[i].slot_name == NULL) {
				continue;
			}

			if (strcmp(command_info->iface, net[i].eth_name) == 0) {
				information_parse(net[i], command_info->output);
				break;
			}
		}
	}
		
	/* option : -m  or -m -o  */	
	if (command_info->mac != NULL) {
		for (i = 0; i < net_port_quantity; i++) {
			if (net[i].slot_name == NULL) {
				continue;
			}

			if (strcmp(command_info->mac, net[i].address) == 0) {
				information_parse(net[i], command_info->output);
				break;
			}
		}
	}

	/* For only -o option */
	if (argc == 4 || (command_info->config_path != NULL && argc <= 6)) {
		if (command_info->output != NULL) {	
			/* Don't duplicate card name */
			if (strcmp(command_info->output, "card") == 0) {
				printf("%s\n", net[0].card_name);
				for ( i = 0; i < net_port_quantity - 1; i++) {
					if (net[i].slot_name == NULL || net[i + 1].slot_name == NULL) {
						continue;
					}

					if (strcmp(net[i].slot_name, net[i + 1].slot_name) != 0) {
						printf("%s\n", net[i + 1].card_name);
					}
				}
			}
			else if (strcmp(command_info->output, "slot") == 0){
				printf("%s\n", net[0].slot_name);
				for ( i = 0; i < net_port_quantity - 1; i++) {
					if (net[i].slot_name == NULL || net[i + 1].slot_name == NULL) {
						continue;
					}

					if (strcmp(net[i].slot_name, net[i + 1].slot_name) != 0) {
						printf("%s\n", net[i + 1].slot_name);
					}
				}
			}
			else {
				for (i = 0; i < net_port_quantity; i++) {
					if (net[i].slot_name == NULL) {
						continue;
					}

					information_parse(net[i], command_info->output);
				}
			}
		}
	}
}


