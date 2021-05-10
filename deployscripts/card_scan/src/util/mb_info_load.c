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
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include "card_scan.h"

void format_check(mb_t *mb, char *exe_dir, char *parameter, char *input, FILE *conf);
static void comment_del(char *str);
static void cfg_bdf_get(mb_t *mb_cfg, FILE *cfg, FILE *cfg_order);
static void set_slot_order(char *buff, mb_t *mb_cfg);

/* Initialization of slot_bus_info_t structure */
void slot_bus_info_t_init(slot_bus_info_t *slot_bus_info, slot_bus_info_t *on_board_bus_info)
{
	int i;
	int j;

	for (i = 0; i < SLOT_QUANTITY; i++) {
		memset(slot_bus_info[i].slot, '\0', SLOT_NAME_LEN);
		memset(slot_bus_info[i].end_root_bus_info, '\0', 8);

		for (j = 0; j < MAX_BUS_INFO_NUM; j++) {
			memset(slot_bus_info[i].bus_info[j], '\0', 8);
		}
	}

	memset(on_board_bus_info->slot, '\0', SLOT_NAME_LEN);
	for (j = 0; j < MAX_BUS_INFO_NUM; j++) {
		memset(on_board_bus_info->bus_info[j], '\0', 8);
	}
}

/* Initialization of mb_t_init structure */
void mb_t_init(mb_t *mb) {
	mb->nb_vid  = 0;
	mb->nb_did  = 0;
	mb->sb_vid  = 0;
	mb->sb_did  = 0;
	mb->ob_dev_quantity     = 0;
	mb->ext_slot_quantity   = 0;
	mb->nb_pci_bus_info = NULL;
	mb->sb_pci_bus_info = NULL;
	mb->ob_dev_list     = NULL;
	mb->ext_slot        = NULL;
}

void slot_order_t_init(slot_order_t *slot_order)
{
	int i;
	for (i = 0; i < SLOT_QUANTITY; i++) {
		memset(slot_order[i].slot_name, '\0', SLOT_NAME_LEN);
	}
}

void mb_info_load(char *exe, char *mb_name, mb_t *mb, command_info_t *command_info) {
	char path_mb[CONF_PATH_LEN] = {'\0'};
	char path_order[CONF_PATH_LEN] = {'\0'};
	FILE *conf = NULL;
	FILE *conf_order = NULL;
	char *exe_dir = NULL;
	char dir_name[CONF_PATH_LEN] = {'\0'};

	/* Initialization of mb_t structure */
    mb_t_init(mb);

	strncpy(mb->mb_name, mb_name, MB_NAME_LEN);

	if (command_info->config_path == NULL) {
    	exe_dir = strdup(exe); // executive folder
   		if (exe_dir == NULL) {
       		printf("Not enough memory\n");
       		mb_t_free(mb);
       		exit(-1);
    	}

		sprintf(dir_name, "%s", dirname(exe_dir));
		if (snprintf(path_mb, sizeof(path_mb), "%s/%s/%s", dir_name, CFG_PATH, mb_name)) {
			// Workaround: avoid warning truncation for latest gcc version
		}

		if (snprintf(path_order, sizeof(path_order), "%s/%s", dir_name, SLOT_ORDER_LIST)) {
			// Workaround: avoid warning truncation for latest gcc version
		}

		free(exe_dir);
		exe_dir = NULL;
    } else { //specify the configuration path
		if (snprintf(path_mb, sizeof(path_mb), "%s/mb_cfg/%s", command_info->config_path, mb_name)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		if (snprintf(path_order, sizeof(path_order), "%s/slot_order.cfg", command_info->config_path)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
    }
	
	conf = fopen(path_mb, "r");
	/* Mother board configuration can't open */
	if (conf == NULL) {
		printf("%s can't open\n", path_mb);
		mb_t_free(mb);
		exit(-1);
	}

	/* Slot order setting */
	conf_order = fopen(path_order, "r");

	cfg_bdf_get(mb, conf, conf_order);
	
	fclose(conf);
}

void format_check(mb_t *mb, char *exe_dir, char *parameter, char *input, FILE *conf) {
	if (input[0] != '0' || input[1] != 'x') {
		printf("Check configure : %s\n", parameter);
		mb_t_free(mb);
		fclose(conf);
		exit(-1);
	}
}

static void comment_del(char *str)
{
	int comment_start = 0;
	int comment_end = 0;
	int i;
	int j;
	int k;

	for (i = 0; i <= strlen(str); i++) {
		if ((str[i] == '/') && (str[i + 1] == '/')) {
			str[i] = '\0';
		break;
		}
	}

	for (i = 0; i <= strlen(str); i++) {
		if ((str[i] == '/') && (str[i + 1] == '*')) {
			comment_start = i;
			for (j = comment_start; j <= strlen(str); j++) {
				if ((str[j] == '*') && (str[j + 1] == '/')) {
					comment_end = j + 1;
					for (k = comment_start; k <= comment_end; k++) {
						str[k] = ' ';
					}
					i = comment_end;
					break;
				}
			}
		}
	}
}

static void cfg_bdf_get(mb_t *mb_cfg, FILE *cfg, FILE *cfg_order)
{   
	char *str_ptr = NULL;   
	char str[150] = {'\0'};   
	int	cfg_idx = 0;   
	int root_port_idx = 0; 
	int end_root;   
	int flag_order_cfg = 0;

	slot_bus_info_t_init(mb_cfg->slot_bus_info, &(mb_cfg->on_board_bus_info));
	slot_order_t_init(mb_cfg->slot_order);

	cfg_idx = 0;

	if (cfg_order) {
		while (fgets(str, sizeof(str), cfg_order)) {
			comment_del(str);
			if (strstr(str, "ORDER")) {
				set_slot_order(str, mb_cfg);
				break;
			}
		}
		fclose(cfg_order);
	} else {
		flag_order_cfg = 1;
	}

	while (fgets(str, sizeof(str), cfg)) {
		comment_del(str);

		if (flag_order_cfg) {
			if (strstr(str, "ORDER")) {
				set_slot_order(str, mb_cfg);
				continue;
			}
		}

		if (strstr(str, "SLOT_")) {
			end_root = 0;  
			str_ptr = strtok(str, " ");   
			strncpy(mb_cfg->slot_bus_info[cfg_idx].slot, str, SLOT_NAME_LEN - 1);

			root_port_idx = 0;  

			while ((str_ptr = strtok(NULL, " "))) {
				if (end_root == 0) {
					strncpy(mb_cfg->slot_bus_info[cfg_idx].end_root_bus_info, str_ptr, 7);
					end_root = 1;
				} else {
					strncpy(mb_cfg->slot_bus_info[cfg_idx].bus_info[root_port_idx], str_ptr, 7);
					root_port_idx++;
				}
			}
			cfg_idx++;
		}

		if (strstr(str, "ON_BOARD")) {
			str_ptr = strtok(str, " ");
			strncpy(mb_cfg->on_board_bus_info.slot, str, SLOT_NAME_LEN - 1);

			root_port_idx = 0;

			while ((str_ptr = strtok(NULL, " "))) {
				strncpy(mb_cfg->on_board_bus_info.bus_info[root_port_idx], str_ptr, 7);
				root_port_idx++;
			}
			cfg_idx++;
		}
		memset(str, '\0', sizeof(str));
	}
}

static void set_slot_order(char *buff, mb_t *mb_cfg)
{
	int idx = 0;
	char *str_ptr = NULL;

	buff = strtok(buff, "\n");
	str_ptr = strtok(buff, " ");
	/* Skip first string "ORDER" in configuration */
	str_ptr = strtok(NULL, " ");

	while (str_ptr != NULL) {
		strcpy(mb_cfg->slot_order[idx].slot_name, str_ptr);
		str_ptr = strtok(NULL, " ");
		idx++;
	}
}
