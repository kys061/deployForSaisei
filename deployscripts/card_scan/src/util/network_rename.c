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
#include <ctype.h>

#include "card_scan.h"

char *tolwr(char *upr) {
    int i;

    for(i = 0; i < MAC_LEN - 1; i++)
        upr[i] = tolower(upr[i]);
    return upr;
}

void network_rename(char *exe, net_t *net, mb_t *mb, int net_port_quantity, char status, char *gen_path, char *slot_order_config_path) {
	FILE *udev_set = NULL;
	char first_rename[ETH_NAME_LEN];
    char second_rename[ETH_NAME_LEN];
	int idx;
    int i, j;

	/* Only generate udev configuration */
	if (status == 'g') {
		udev_set = fopen(gen_path, "w");
	}
	else {
		printf("Starting network rename ...\n");
	}

	/* First rename for temp name */
	idx = 0;
	for (i = 0; i < SLOT_QUANTITY; i++) {
		for (j = 0; j < net_port_quantity; j++) {
			if (net[j].slot_name == NULL) {
				continue;
			}
			if (strcmp(mb->slot_order[i].slot_name, net[j].slot_name) == 0) {
				switch (status) {
				case 'r':
					ip(NULL, net[j].eth_name, 2); // down (NULL, "ethx", 2)
					if (snprintf(first_rename, sizeof(first_rename), "%s@%d", net[j].eth_name, idx++)) {
						// Workaround: avoid warning truncation for latest gcc version
					}
					ip(net[j].eth_name, first_rename, 3); // rename ("old_name", "new_name", 3)
					strcpy(net[j].eth_name, first_rename);
					break;
				case 'g':
					if (snprintf(first_rename, sizeof(first_rename), "%s@%d", net[j].eth_name, idx++)) {
						// Workaround: avoid warning truncation for latest gcc version
					}
					strcpy(net[j].eth_name, first_rename);
					break;
				}
			}
		}
	}

	/* Second rename for real name */
	idx = 0;
	for (i = 0; i < SLOT_QUANTITY; i++) {
		for (j = 0; j < net_port_quantity; j++) {
			if (net[j].slot_name == NULL) {
				continue;
			}
			if (strcmp(mb->slot_order[i].slot_name, net[j].slot_name) == 0) {
				if (snprintf(second_rename, sizeof(second_rename), "eth%d", idx++)) {
					// Workaround: avoid warning truncation for latest gcc version
				}

				switch (status) {
				case 'r':
					ip(net[j].eth_name, second_rename, 3); // rename ("old_name", "new_name", 3)
					strcpy(net[j].eth_name, second_rename);
					ip(net[j].eth_name, NULL, 1); // up ("ethx", NULL, 1)
					break;
				case 'g':
					strcpy(net[j].eth_name, second_rename);
					fprintf(udev_set, "\"SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"%s\", ATTR{dev_id}==\"0x0\", ATTR{type}==\"1\", KERNEL==\"eth*\", NAME=\"%s\"\n", tolwr(net[j].address), net[j].eth_name);
					break;
				}
			}
		}
	}

	if (status == 'g') {
		fclose(udev_set);
		printf("Complete\n");
	}
	else {
		printf("Complete\n");
	}
}
