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
#include <dirent.h>
#include <ctype.h>

#include "card_scan.h"

char *touppe(char *lower) {
    int i;

    for(i = 0; i < MAC_LEN - 1; i++)
        lower[i] = toupper(lower[i]);
    return lower;
}

/* Initialization of device_t structure */
void device_t_init(device_t *device, int pci_dev_quantity)
{
	int i;

	for (i = 0; i < pci_dev_quantity; i++) {
		device[i].bus_root_info = NULL;
		device[i].bus_info = NULL;
		device[i].class_code = 0;
		device[i].device_id = 0;
		device[i].vendor_id = 0;
		device[i].if_net = 0;
		net_t_init(&(device[i].net));
	}
}

int dev_access(device_t **dev, mb_t *mb_cfg)
{
    FILE 	*dev_class = NULL;
    FILE 	*dev_dev_id = NULL;
    FILE 	*dev_vendor_id = NULL;
    FILE 	*dev_net_address = NULL;
    struct 	dirent **ether = NULL;
    char 	tmp[PATH_LEN] = {'\0'};
	char 	file_path[PATH_LEN] = {'\0'};
	char	eth_name[20] = {'\0'};
	int 	net_folder; // -1 is no net folder
	int		i;
    int 	j;
	int     k;
	struct dirent **namelist        = NULL;
	int pci_dev_quantity;

	DIR *dev_root_port_dir = NULL;
	struct dirent *dev_root_port_ent;
	char *str_split[3];
	char mb_root_name[PATH_LEN];
	char *ret;

	device_t *device = NULL;

    /* Get PCI device quantity */
	pci_dev_quantity = scandir(PCI_PATH_SYS_BUS_PCI_DEV, &namelist, 0, alphasort) - 2;
	*dev = (device_t *)malloc(sizeof(device_t) * pci_dev_quantity);
	device = *dev;

	if (device == NULL) {
		printf("Not enough memory\n");
		exit(-1);
	}

    /* Initialization of device_t structure */
    device_t_init(device, pci_dev_quantity);

	/* Access /sys/bus/pci/devices/ device information */
	if (pci_dev_quantity < 0) {
        perror("Can't open /sys/\n");
		exit(-1);
	}
    else
    {
        for (j = 0; j < pci_dev_quantity; j++) {
			/* Access device pci info*/
			device[j].bus_info = (char *)malloc(sizeof(char) * (strlen(namelist[j + 2]->d_name) + 1));
			
			if (device[j].bus_info == NULL) {
				printf("Not enough memory\n");
				exit(-1);
			}
			
			strcpy(device[j].bus_info, namelist[j + 2]->d_name);
			
			/* Access device class code */
			if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/class", device[j].bus_info)) {
				// Workaround: avoid warning truncation for latest gcc version
			}
			dev_class = fopen(file_path, "r");
            fscanf(dev_class, "%s", tmp); 
			device[j].class_code = strtol(tmp, NULL, 16);

            /* Access device ID */
			if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/device", device[j].bus_info)) {
				// Workaround: avoid warning truncation for latest gcc version
			}

            dev_dev_id = fopen(file_path, "r");
            fscanf(dev_dev_id, "%s", tmp);
			device[j].device_id = strtol(tmp, NULL, 16);
			
			/* Access vendor ID*/
			if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/vendor", device[j].bus_info)) {
				// Workaround: avoid warning truncation for latest gcc version
			}
            dev_vendor_id = fopen(file_path, "r");
            fscanf(dev_vendor_id, "%s", tmp);
			device[j].vendor_id = strtol(tmp, NULL, 16);
            
			/* device is network control -> class_code = 1, not network control -> class_code = 0, 131072 is network device */
            if (device[j].class_code == 131072) {
				if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/net", device[j].bus_info)) {
					// Workaround: avoid warning truncation for latest gcc version
				}
				net_folder = scandir(file_path, &ether, 0, alphasort);

				if (net_folder < 0) { // folder "net" doesn't exist, search folder "net:ethx"
					if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s", device[j].bus_info)) {
						// Workaround: avoid warning truncation for latest gcc version
					}
					net_folder = scandir(file_path, &ether, 0, alphasort);

					/* Search folder "net:ethx" */
					for (i = 0; i < net_folder; i++) {
						if (strstr(ether[i]->d_name, "net:")) {
							strtok(ether[i]->d_name, ":");
							strcpy(eth_name, strtok(NULL, ":"));
							device[j].net.eth_name = strdup(eth_name);

							if (device[j].net.eth_name == NULL) {
								printf("Not enough memory\n");
								exit(-1);
							}

							device[j].if_net = 1;
							if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/net:%s/address", device[j].bus_info, device[j].net.eth_name)) {
							// Workaround: avoid warning truncation for latest gcc version
							}
							break;
						}
					}

					/* Don't find folder "net:ethx" */
					if (i == net_folder) {
						continue;
//						printf("Please check network interface !!!\n");
//						printf("Can't open net or net:ethx in %s\n", file_path);
//						exit(-1);
					}
				}
				else {
					device[j].net.eth_name = (char *)malloc(sizeof(char) * (strlen(ether[2]->d_name) + 1));

					if (device[j].net.eth_name == NULL) {
						printf("Not enough memory\n");
						exit(-1);
					}
           			
					strcpy(device[j].net.eth_name, ether[2]->d_name); // Access network interface name
					device[j].if_net = 1; // It's network device
					if (snprintf(file_path, sizeof(file_path), PCI_PATH_SYS_BUS_PCI_DEV "/%s/net/%s/address", device[j].bus_info, device[j].net.eth_name)) {
					// Workaround: avoid warning truncation for latest gcc version
					}
				}

				dev_net_address = fopen(file_path, "r");
                fscanf(dev_net_address, "%s", tmp);
					
				device[j].net.address = (char *)malloc(sizeof(char) * (strlen(tmp) + 1));

				if (device[j].net.address == NULL) {
					printf("Not enough memory\n");
					exit(-1);
				}

				strcpy(device[j].net.address, touppe(tmp)); // Access MAC
				fclose(dev_net_address);

				device[j].net.card_name = NULL; // default
            }

            fclose(dev_class);
            fclose(dev_dev_id);
            fclose(dev_vendor_id);
        }

#if 0
		// Find device root port for onboard
		for (i = 0; i < pci_dev_quantity; i++) {
			memset(tmp, '\0', PATH_LEN);
			strncpy(tmp, device[i].bus_info, 7);
			snprintf(file_path, sizeof(file_path), PCI_PATH_FIND_ROOT_DEV"/%s/device", tmp);
			if ((dev_root_port_dir = opendir(file_path)) != NULL) {
				while ((dev_root_port_ent = readdir(dev_root_port_dir)) != NULL) {
					for (j = 0; j < MAX_BUS_INFO_NUM; j++) {
						if (mb_cfg->on_board_bus_info.bus_info[j] == NULL) {
							continue;
						}
						memset(tmp, '\0', PATH_LEN);
						strcpy(tmp, mb_cfg->on_board_bus_info.bus_info[j]);
						str_split[0] = strtok(tmp, "/");
						str_split[1] = strtok(NULL, "/");
						if (str_split[0] != NULL && str_split[1] != NULL) {
							memset(mb_root_name, '\0', PATH_LEN);
							strcpy(mb_root_name, str_split[0]);
							strcat(mb_root_name, ":");
							strcat(mb_root_name, str_split[1]);
							ret = strstr(dev_root_port_ent->d_name, mb_root_name);
							if (ret != NULL) {
								device[i].bus_root_info = (char*)malloc(sizeof(char) * (strlen(ret)));
								strcpy(device[i].bus_root_info, mb_cfg->on_board_bus_info.bus_info[j]);
							}
							/* Special case for CAR-3080 */
							else if (strcmp(mb_cfg->mb_name, "CAR3080") == 0) {
								if (ret != NULL && strlen(ret) == 7) {
									device[i].bus_root_info = (char*)malloc(sizeof(char) * (strlen(ret)));
									strcpy(device[i].bus_root_info, mb_cfg->on_board_bus_info.bus_info[j]);
								}
							}
						}
					}
				}
			}
		}
#endif

		// Find device root port for slot
		for (i = 0; i < pci_dev_quantity; i++) {
			memset(tmp, '\0', PATH_LEN);
			strncpy(tmp, device[i].bus_info, 7);
			if (snprintf(file_path, sizeof(file_path), PCI_PATH_FIND_ROOT_DEV"/%s/device", tmp)) {
				// Workaround: avoid warning truncation for latest gcc version
			}

			if ((dev_root_port_dir = opendir(file_path)) != NULL) {
				while ((dev_root_port_ent = readdir(dev_root_port_dir)) != NULL) {
					for (k = 0; k < SLOT_QUANTITY; k++) {
						for (j = 0; j < MAX_BUS_INFO_NUM; j++) {
							if (mb_cfg->slot_bus_info[k].bus_info[j] == NULL) {
								continue;
							}
							memset(tmp, '\0', PATH_LEN);
							strcpy(tmp, mb_cfg->slot_bus_info[k].bus_info[j]);
							str_split[0] = strtok(tmp, "/");
							str_split[1] = strtok(NULL, "/");
							if (str_split[0] != NULL && str_split[1] != NULL) {
								memset(mb_root_name, '\0', PATH_LEN);
								strcpy(mb_root_name, str_split[0]);
								strcat(mb_root_name, ":");
								strcat(mb_root_name, str_split[1]);
								ret = strstr(dev_root_port_ent->d_name, mb_root_name);
								if (ret != NULL) {
									device[i].bus_root_info = (char*)malloc(sizeof(char) * (strlen(ret)));
									strcpy(device[i].bus_root_info, mb_cfg->slot_bus_info[k].bus_info[j]);
								}
							}
						}
					}
				}
			}
		}
	}

	return pci_dev_quantity;
}

