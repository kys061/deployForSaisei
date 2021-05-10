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
#include "card_scan.h"

/* Free mb_t structure */
void mb_t_free(mb_t *mb) {
//	slot_dev_t_free(mb->slot_dev);
//	ext_slot_t_free(mb->ext_slot, mb->ext_slot_quantity);
	free(mb->nb_pci_bus_info);
	free(mb->sb_pci_bus_info);
	free(mb->ext_slot);
	free(mb->ob_dev_list);

	mb->nb_pci_bus_info = NULL;
	mb->sb_pci_bus_info = NULL;
	mb->ext_slot = NULL;
	mb->ob_dev_list = NULL;
}

/* Free slot_dev_t structure */
void slot_dev_t_free(slot_dev_t *slot_dev) {
	int i;

	for (i = 0; i < SLOT_QUANTITY; i++) {
		if (slot_dev[i].slot_name != NULL) {
			free(slot_dev[i].slot_dev_id_list);
			free(slot_dev[i].slot_name);

			slot_dev[i].slot_dev_id_list = NULL;
			slot_dev[i].slot_name = NULL;
		}
	}
}

/* Free ext_slot_t structure */
void ext_slot_t_free(ext_slot_t *ext_slot, short ext_slot_quantity) {	
	int i;

	for (i = 0; i < ext_slot_quantity; i++) {
		free(ext_slot[i].ext_list);
		ext_slot[i].ext_list = NULL;
	}
}

/* Free command_info_t structure */
void command_info_t_free(command_info_t *command_info) {
	free(command_info->card);
	free(command_info->slot);
	free(command_info->iface);
	free(command_info->mac);
	free(command_info->output);
	free(command_info->gen_path);
	free(command_info->config_path);
	free(command_info->slot_order_config_path);

	command_info->card  = NULL;
	command_info->slot  = NULL;
	command_info->iface = NULL;
	command_info->mac   = NULL;
	command_info->output   = NULL;
	command_info->gen_path = NULL;
	command_info->config_path = NULL;
	command_info->slot_order_config_path = NULL;
}

/* Free device_t structure */
void device_t_free(device_t *device, int pci_dev_quantity) {
	int i;

	for (i = 0; i < pci_dev_quantity; i++) {
		free(device[i].bus_info);
		free(device[i].bus_root_info);
		device[i].bus_info = NULL;
		device[i].bus_root_info = NULL;
		net_t_free(&(device[i].net));
	}
}

/* Free net_t structure */
void net_t_free(net_t *net) {
	free(net->bus_root_info);
	free(net->address);
	free(net->eth_name);
	free(net->slot_name);
	free(net->card_name);

	net->bus_root_info = NULL;
	net->address = NULL;
	net->eth_name = NULL;
	net->slot_name = NULL;
	net->card_name = NULL;
}

void mem_free(device_t *device, int pci_dev_quantity, net_t *net, int net_port_quantity, mb_t *mb, command_info_t *command_info)
{
	int i;

	/* Free mb_t structure */
	mb_t_free(mb);

	/* Free slot_dev_t structure */
	device_t_free(device, pci_dev_quantity);
	free(device);

    /* Free net_t_ structure */
	for (i = 0; i < net_port_quantity; i++) {
		net_t_free(net + i);
	}

	free(net);

	/* Free command_info_t structure */
	command_info_t_free(command_info);
}
 
