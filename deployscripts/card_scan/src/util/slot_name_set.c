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
#include <dirent.h>
#include "string.h"
#include "card_scan.h"

static u32 class_code_get(char *pci_conf_path)
{
	FILE *pci_conf = fopen(pci_conf_path, "rb");
	u32 class_code = 0xffffff;

	if (pci_conf != NULL) {
		fseek(pci_conf, 9, SEEK_CUR);
		fread(&class_code, sizeof(u8), 3, pci_conf);
		fclose(pci_conf);
	} else {
		printf("Can't open %s, please check.\n", pci_conf_path);
		exit(-1);
	}

	return class_code;
}

static u8 bus_existing_check(device_t *device, int pci_dev_quantity, char *bus_info)
{
	u8 ret = FALSE;
	char bus[8] = {'\0'};
	int i;

	for (i = 0; i < pci_dev_quantity; i++) {
		strcpy(bus, strstr(device[i].bus_info, ":") + 1);
		bus[2] = '/';

		if (strcmp(bus_info, bus) == 0) {
			ret = TRUE;
			break;
		}
	}

	return ret;
}

static void down_stream_bus_path_get(char *pci_conf_path, char *down_stream_bus_path)
{
	FILE *pci_conf = fopen(pci_conf_path, "rb");
	char down_stream_bus_path_tmp[PATH_LEN] = {'\0'};
	u8 down_stream_bus = 0xff;

	if (pci_conf != NULL) {
		fseek(pci_conf, 25, SEEK_CUR);
		fread(&down_stream_bus, sizeof(u8), 1, pci_conf); // Access down stream bus
		fclose(pci_conf);

		if (snprintf(down_stream_bus_path_tmp, sizeof(down_stream_bus_path_tmp), PCI_PATH_PROC_BUS_PCI "/%02x", down_stream_bus)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		strcpy(down_stream_bus_path, down_stream_bus_path_tmp);
	} else {
		printf("Can't open_ %s, please check.\n", pci_conf_path);
		exit(-1);
	}
}

static u8 matched_bus_path_get(char *end_root_bus_info, char *bus_info, char *matched_bus_path)
{
	u8 ret = FALSE;
	struct dirent **down_strean_dir = NULL;
	char pci_path[PATH_LEN] = {'\0'};
	char down_stream_bus_path[PATH_LEN] = {'\0'};
	int bus_dev_num = 0;
	int i;
	u32 class_code = 0xffffff;

	down_stream_bus_path_get(end_root_bus_info, down_stream_bus_path);
	bus_dev_num = scandir(down_stream_bus_path, &down_strean_dir, 0, alphasort);

	if (bus_dev_num > 0) {
		for (i = 2; i < bus_dev_num; i++) {
			if (snprintf(pci_path, sizeof(pci_path), "%s/%s", down_stream_bus_path, down_strean_dir[i]->d_name)) {
				// Workaround: avoid warning truncation for latest gcc version
			}

			class_code = class_code_get(pci_path);

			if (class_code == PCI_BRIDGE) {
				if (strcmp(down_strean_dir[i]->d_name, bus_info) == 0) {
					strcpy(matched_bus_path, pci_path);
					ret = TRUE;
					break;
				} else {
					ret = matched_bus_path_get(pci_path, bus_info, matched_bus_path);
					if (ret) {
						break;
					}
				}
			} else if (class_code == ETH_CONTROLLER) {
				continue;
			}
		}
	} else {
		ret = FALSE;
	}

	return ret;
}


/* ---------------------------------------- Set netports slot name for modules ------------------------------ */
void net_slot_set(char *pci_path, device_t *device, mb_t *mb, int pci_dev_quantity, char *slot_name)
{
	struct dirent **down_strean_dir = NULL;
	FILE *pci_conf_space = NULL;
	unsigned char down_stream_bus, class_code;
	char down_stream_bus_path[PATH_LEN];
	char pci_path_tmp[PATH_LEN];
	char class_code_tmp[3];
	char bus_info[PATH_LEN];
	int i, j;
	int file_num;
	char tmp[PATH_LEN];

	if (class_code_get(pci_path) == ETH_CONTROLLER) {
		/* If device is a network device, set its slot name */
		strcpy(tmp, pci_path + 14);
		tmp[2] = ':';
		if (snprintf(bus_info, sizeof(bus_info), "0000:%s", tmp)) {
			// Workaround: avoid warning truncation for latest gcc version
		}

		for (j = 0; j < pci_dev_quantity; j++) {
			if (strcmp(bus_info, device[j].bus_info) == 0) {
				device[j].net.slot_name = strdup(slot_name);
				if (device[j].net.slot_name == NULL) {
					printf("Not enough memory\n");
					mb_t_free(mb);
					device_t_free(device, pci_dev_quantity);
					exit(-1);
				}

				if (strcmp(device[j].net.slot_name, "ON_BOARD") == 0) {
					device[j].net.card_name = strdup(mb->mb_name);

					if (device[j].net.card_name == NULL) {
						printf("Not enough memory\n");
							mb_t_free(mb);
							device_t_free(device, pci_dev_quantity);
							exit(-1);
					}
				}
			}
		}
	} else {
		/* If device is not a network device, search network device from down stream bus */
		/* Access down stream bus under slot (non-onboard) */
		strcpy(pci_path_tmp, pci_path);
		pci_conf_space = fopen(pci_path_tmp, "rb");
		fseek(pci_conf_space, 25, SEEK_CUR);
		fread(&down_stream_bus, sizeof(unsigned char ), 1, pci_conf_space); // Access down stream bus
		fclose(pci_conf_space);

		/* Avoid that the program is crashed. */
		if (down_stream_bus == 0x00) {
			return;
		}

		if (snprintf(down_stream_bus_path, sizeof(down_stream_bus_path), PCI_PATH_PROC_BUS_PCI "/%02x", down_stream_bus)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		file_num = scandir(down_stream_bus_path, &down_strean_dir, 0, alphasort); // Each bus file number

		if (file_num > 0) {
			for (i = 2; i < file_num; i++) {
				if (snprintf(pci_path_tmp, sizeof(pci_path_tmp), "%s/%s", down_stream_bus_path, down_strean_dir[i]->d_name)) {// Workaround: avoid warning truncation for latest gcc version
}
				pci_conf_space = fopen(pci_path_tmp, "rb");
				fseek(pci_conf_space, 11, SEEK_CUR);
				fread(&class_code, sizeof(unsigned char ), 1, pci_conf_space);
				if (snprintf(class_code_tmp, sizeof(class_code_tmp), "%02x", class_code)) {
					// Workaround: avoid warning truncation for latest gcc version
				}
				fclose(pci_conf_space);

				/* Set slot name (non-noboard) */
				if (strcmp(class_code_tmp, "10") == 0) { // "10" --> Network and computing encryption device have no down steam bus
					continue;
				} else if (strcmp(class_code_tmp, "02") == 0) {
					strcpy(tmp, pci_path_tmp + 14);
					tmp[2] = ':';
					if (snprintf(bus_info, sizeof(bus_info), "0000:%s", tmp)) {
						// Workaround: avoid warning truncation for latest gcc version
					}

					for (j = 0; j < pci_dev_quantity; j++) {
						if (strcmp(bus_info, device[j].bus_info) == 0) {
							device[j].net.slot_name = strdup(slot_name);
							if (device[j].net.slot_name == NULL) {
								printf("Not enough memory\n");
								mb_t_free(mb);
								device_t_free(device, pci_dev_quantity);
								exit(-1);
							}

							if (strcmp(device[j].net.slot_name, "ON_BOARD") == 0) {
								device[j].net.card_name = strdup(mb->mb_name);

								if (device[j].net.card_name == NULL) {
									printf("Not enough memory\n");
									mb_t_free(mb);
									device_t_free(device, pci_dev_quantity);
									exit(-1);
								}
							}
						}
					}
					continue;
				}
				net_slot_set(pci_path_tmp, device, mb, pci_dev_quantity,  slot_name);
			}
		}
	}
}

/* -------------------------------------- Netports assigned slot name ------------------------------------ */
void slot_name_set(device_t *device, mb_t *mb, int pci_dev_quantity)
{
	int  i, j, k;
	char bus[100] = {'\0'};
	char matched_bus_path[100] = {'\0'};
	char pci_path[PATH_LEN] = {'\0'};
	u8 ret = FALSE;
	
	/* Set netports slot name for slot (non-onboard) */
    for (k = 0; k < SLOT_QUANTITY; k++) {
		if (mb->slot_bus_info[k].slot[0] != '\0') {
			if (strcmp(mb->slot_bus_info[k].end_root_bus_info, "00/00.0") == 0) {
				for (i = 0; i < MAX_BUS_INFO_NUM; i++) {
					for (j = 0; j < pci_dev_quantity; j++) {
						strcpy(bus, strstr(device[j].bus_info, ":") + 1);
						bus[2] = '/';
						
						if (strcmp(mb->slot_bus_info[k].bus_info[i], bus) == 0) {
							if (snprintf(pci_path, sizeof(pci_path), PCI_PATH_PROC_BUS_PCI "/%s", bus)) {
								// Workaround: avoid warning truncation for latest gcc version
							}
							net_slot_set(pci_path, device, mb, pci_dev_quantity, mb->slot_bus_info[k].slot);
						}
					}
				}
			} else {
				/* Support multiple slots which own a single root port but it is necessary that the "device.function" is fixed. */
				if (bus_existing_check(device, pci_dev_quantity, mb->slot_bus_info[k].end_root_bus_info)) {
					for (i = 0; i < MAX_BUS_INFO_NUM; i++) {
						if (snprintf(pci_path, sizeof(pci_path), PCI_PATH_PROC_BUS_PCI "/%s", mb->slot_bus_info[k].end_root_bus_info)) {
							// Workaround: avoid warning truncation for latest gcc version
						}
						ret = matched_bus_path_get(pci_path, mb->slot_bus_info[k].bus_info[i], matched_bus_path);

						if (ret) {
							net_slot_set(matched_bus_path, device, mb, pci_dev_quantity, mb->slot_bus_info[k].slot);
						}
					}
				}
			}
		}
	}

	/* Set netports slot name for onboard */
		if (mb->on_board_bus_info.slot[0] != '\0') {
			for (i = 0; i < MAX_BUS_INFO_NUM; i++) {
				for (j = 0; j < pci_dev_quantity; j++) {
					strcpy(bus, strstr(device[j].bus_info, ":") + 1);
					bus[2] = '/';

					if (strcmp(mb->on_board_bus_info.bus_info[i], bus) == 0) {
						if (snprintf(pci_path, sizeof(pci_path), PCI_PATH_PROC_BUS_PCI "/%s", bus)) {
							// Workaround: avoid warning truncation for latest gcc version
						}
						net_slot_set(pci_path, device, mb, pci_dev_quantity, mb->on_board_bus_info.slot);
					}
				}
			}
		}
}
