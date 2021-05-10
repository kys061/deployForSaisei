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
#include <getopt.h>

#include "card_scan.h"

void check_slot_order(FILE *conf, command_info_t *command_info)
{
    char format;
    char str[80];

    /* Check file format */
    format = 'N';

    while (fscanf(conf, "%s", str) != EOF) {
        if (strcmp(str, "ORDER") == 0) {
            format = 'Y';
        }
    }

    if (format == 'N') {
        printf("Check format : slot_order_rename.cfg, ORDER <ON_BOARD> <SLOT_X> <SLOT_Y> ....\n");
        command_info_t_free(command_info);
        fclose(conf);
        exit(-1);
    }
}

void usage()
{
	printf("\n");
	printf("[VERSION]\n");
	printf("    %s\n", VERSION);
	printf("\n");
	printf("[SYNOPSIS]\n");
	printf("    card_scan <MODEL_NAME> [FEATURE] [VALUE]\n");
	printf("    MODEL_NAME: The support model name in SUPPORT_LIST\n");
	printf("\n");
	printf("    [FEATURE]:\n");
	printf("       -h  Print command usage \n");
	printf("       -r  Rename network interface immediately\n");
	printf("       -C  <CONFIG_DIR_PATH> Display information by specify configuration file path which name is <CONFIG_DIR_PATH>\n");
	printf("       -c  <CARD_NAME> Display specific information search by network card name which name is <CARD_NAME>\n");
	printf("       -s  <SLOT_X> Display specific information search by slot name which name is <SLOT_X>\n");
	printf("       -i  <NET_DEV> Display specific information search by network interface which name is <NET_DEV>\n");
	printf("       -m  <MAC> Display specific information search by MAC address which name is <MAC>\n");
	printf("       -o  <OUTPUT> Display specific information by <OUTPUT> which value follow:\n");
	printf("           <OUTPUT>: \n");
	printf("                    slot    Display all slot name\n");
	printf("                    iface   Display all network interface\n");
	printf("                    card    Display all network card name\n");
	printf("                    mac     Display all MAC address\n");
}

/* Initialization of command_info_t structure */
void command_info_t_init(command_info_t *command_info)
{
    command_info->card  = NULL;
    command_info->slot  = NULL;
    command_info->iface = NULL;
    command_info->mac   = NULL;
    command_info->output = NULL;
    command_info->gen_path = NULL;
    command_info->config_path = NULL;
    command_info->slot_order_config_path = NULL;
    command_info->if_rename = 0;
    command_info->if_config_path = 0;
    command_info->if_slot_order_config_path = 0;
    command_info->if_appear = 0;
}

void command_parse(char *exe, int argc, char *argv[], command_info_t *command_info)
{
	struct dirent **namelist 	= NULL;
	char *exe_dir 				= NULL;
	int n 						= 0;
	char c;
	int i;
	int file_num;
	int exist = FALSE; // Mother board exist -> TRUE , mother board doesn't exist -> FALSE
	char cfg_relative_path[CONF_PATH_LEN] = {'\0'};

	command_info_t_init(command_info);
	
	/* Command characters is between 11 and 2 */
	if (argc > 9 || argc < 2) {
		usage();
		exit(-1);
	}

	/* Avoid -<option>xx */
	for (i = 2; i < argc; i++) {
		if ((argv[i][0] ==  '-') && (strlen(argv[i]) > 2)) {
			usage();
			exit(-1);
		}	
	}

	/* Path can't be a folder */
	for (i = 0; i < argc - 1; i++) {
		if (strcmp(argv[i], "-g") == 0) {
			if (scandir(argv[i + 1], &namelist, 0, alphasort) > 0) {
				usage();
				exit(-1);
			}
		}
	}
	
	opterr = 0;	
    while ((c = getopt(argc, argv, "hc:s:i:m:o:g:C:r")) != -1) {	
		switch (c) {
        case 'h':
			usage();
			exit(-1);
		case 'c':
			command_info->card = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->card, optarg);
			n++;
			break;
		case 's':
			command_info->slot = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->slot, optarg);
			n++;
			break;
		case 'i':
			command_info->iface = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->iface, optarg);
			n++;
			break;
		case 'm':
			command_info->mac = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->mac, optarg);
			n++;
			break;
		case 'o':
			command_info->output = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->output, optarg);
			break;
		case 'g':
			command_info->gen_path = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->gen_path, optarg);
			n++;
			break;
		case 'C':
			command_info->config_path = (char *)malloc(strlen(optarg) + 1);
			strcpy(command_info->config_path, optarg);
			break;
        case 'r':
			command_info->if_rename = 1;
			n++;
			break;
        case '?':
			usage();
			command_info_t_free(command_info);
			exit(-1);
        }
    }

	if (optind != (argc - 1)) {
		usage();
		command_info_t_free(command_info);
		exit(-1);
	}
	
	/* The two options doesn't same exist, but -o, -C are exceptions */
	if (n > 1) {
		usage();
		command_info_t_free(command_info);
		exit(-1);
	}

	if (command_info->config_path == NULL) { // default configuration path
		exe_dir = strdup(argv[0]); // executive folder
		if (exe_dir == NULL) {
			printf("Not enough memory\n");
			command_info_t_free(command_info);
			exit(-1);
		}
		
		if (snprintf(cfg_relative_path, sizeof(cfg_relative_path), "%s/%s", dirname(exe_dir), CFG_PATH)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		file_num = scandir(cfg_relative_path, &namelist, 0, alphasort); // In conf folder file numbers
	
		/* Check mother board configuration existance */
		exist = FALSE;
		for (i = 2; i < file_num; i++) {
			if (strcmp(argv[optind], namelist[i]->d_name) == 0 ) {
				exist = TRUE; // Mother board exist
				break;
			}
		}

		if (exist == FALSE) {
			printf("Can't find %s motherboard configuration.\n", argv[optind]);

			printf("Is ");
			for (i = 2; i < file_num - 1; i++) {
				printf("%s, ", namelist[i]->d_name);
			}
			printf("or %s?\n", namelist[i]->d_name);
			command_info_t_free(command_info);
			free(exe_dir);
			exe_dir = NULL;
			exit(-1);
		}

		free(exe_dir);
	} else { // specify configuration path 
		if (snprintf(cfg_relative_path, sizeof(cfg_relative_path), "%s/mb_cfg", command_info->config_path)) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		file_num = scandir(cfg_relative_path, &namelist, 0, alphasort); // In conf folder file numbers
		
		if (file_num < 0) {
			printf("Path %s is wrong\n", command_info->config_path);
			exit(-1);
		}
		
		exist = FALSE;
		for (i = 2; i < file_num; i++) {
			if (strcmp(argv[optind], namelist[i]->d_name) == 0 ) {
				exist = TRUE; // Mother board exist
				break;
			}
		}

		if (exist == FALSE) {
			printf("The %s is invalid motherboard.\n", argv[optind]);
			command_info_t_free(command_info);
			free(exe_dir);
			exe_dir = NULL;
			exit(-1);
		}
	}
	
	if (command_info->output != NULL) {
		/* If -o is not slot, card, mac and iface, then exit */
		if ((strcmp(command_info->output, "slot") != 0) && (strcmp(command_info->output, "card") != 0) && (strcmp(command_info->output, "mac") != 0) && (strcmp(command_info->output, "iface") != 0)) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}

		/* Invalid option : -s * -o slot */
		if ((command_info->slot != NULL) && (strcmp(command_info->output, "slot") == 0)) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}
		
		/* Invalid option : -c * -o card */
		if ((command_info->card != NULL) && (strcmp(command_info->output, "card") == 0)) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}
		
		/* Invalid option : -m * -o mac */
		if ((command_info->mac != NULL) && (strcmp(command_info->output, "mac") == 0)) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}
		
		/* Invalid option : -i * -o iface */
		if ((command_info->iface != NULL) && (strcmp(command_info->output, "iface") == 0)) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}

		/* Invalid option : -r * -o xxxx */
		if (command_info->gen_path != NULL) {
			usage();
			command_info_t_free(command_info);
			exit(-1);
		}
	}
}
