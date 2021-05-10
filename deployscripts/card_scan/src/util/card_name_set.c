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
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include "card_scan.h"

void eeprom_get(int fd, struct ethtool_eeprom *eeprom, struct ifreq *ifr, u32 offset, u32 len);
void bpid_get(int fd, struct ethtool_eeprom *eeprom, struct ifreq *ifr, net_t *net, char *bpid, u8 *card_type);
void card_name_get(char *bpid, net_t *net, u8 card_type);
void card_type_fix(u16 u16_bp_id, u8 *card_type);
void card_bpid_fix(char *bpid, net_t *net, struct ethtool_eeprom *eeprom);

static int geeprom_offset = 0;
static int geeprom_length = -1;

int geeprom_do(int fd, struct ifreq *ifr, net_t *net, slot_mapping_to_port_table_t *slot_mapping_to_port_table, int i)
{
	int		err;
	char	bpid[10] = {'\0'};
	u8		card_type = 0xff;
    struct 	ethtool_drvinfo drvinfo;
    struct 	ethtool_eeprom *eeprom;

    drvinfo.cmd = ETHTOOL_GDRVINFO;
    ifr->ifr_data = (caddr_t)&drvinfo;
    err = ioctl(fd, SIOCETHTOOL, ifr);

    if (err < 0) {
        perror("Cannot get driver information");
        return 74;
    }

    if (geeprom_length <= 0)
    	geeprom_length = drvinfo.eedump_len;

    if (drvinfo.eedump_len < geeprom_offset + geeprom_length)
        geeprom_length = drvinfo.eedump_len - geeprom_offset;

    eeprom = calloc(1, sizeof(*eeprom)+geeprom_length);
    if (!eeprom) {
    	perror("Cannot allocate memory for EEPROM data");
        return 75;
    }

    if (EEPROM_LEN > geeprom_length) {
        printf("The EEPROM length is more big\n");
        exit(-1);
    }

	bpid_get(fd, eeprom, ifr, net + i, bpid, &card_type);
	card_name_get(bpid, net + i, card_type);

    free(eeprom);

	return err;
}

void eeprom_get(int fd, struct ethtool_eeprom *eeprom, struct ifreq *ifr, u32 offset, u32 len)
{
	int err;

	eeprom->cmd = ETHTOOL_GEEPROM;
	eeprom->len = len;
	eeprom->offset = offset;
	ifr->ifr_data = (caddr_t)eeprom;

	err = ioctl(fd, SIOCETHTOOL, ifr);
    if (err < 0) {
        perror("Cannot get EEPROM data");
        exit(-1);
    }
}

unsigned int * get_ext_tbl_mapping(unsigned int *c)
{
	int i;
	static unsigned int result[5];

	for (i = 0; i < sizeof(result); i++) {
		if ( c[i] < EXT_DIGIT_UPPER_LETTER && c[i] >= EXT_DIGIT ) {
			/* 0~9 */
			result[i] = (char)((c[i] + 48));
		} else if ( c[i] < EXT_DIGIT_LOWER_LETTER && c[i] > EXT_DIGIT_UPPER_LETTER ) {
			/* A~Z */
			result[i] = (char)((c[i] - 10) + 65);
		} else {
			/* a~z */
			result[i] = (char)((c[i] - 97) + 91);
		}
	}

	return result;
}

void bpid_get(int fd, struct ethtool_eeprom *eeprom, struct ifreq *ifr, net_t *net, char *bpid, u8 *card_type)
{
	char	bp_id[10] = {'\0'};
	u16		u16_bp_id;
	u32 	offset;

	u8 rule_type;
	unsigned int ext_bp_id_digit[10];
	unsigned int *ext_bp_id;

	/* Default offset is 08h(word) or 0x10(byte)*/
    offset = 0x10;
	switch (net->device_id) {
    case INTEL_82598EB_10GAFDPNC_TYPE2:
    case INTEL_82599EB_10GDPNC_TYPE2:
    case INTEL_82599EB_10GNC:
	case INTEL_XL710:
	case INTEL_X710:
	case INTEL_XXV710:
	case INTEL_XL710_QSFP:
		/* 15h(word) or 0x2a(byte) */
        offset = 0x2a;
		break;
    }
	eeprom_get(fd, eeprom, ifr, offset, EEPROM_LEN);

    if (eeprom->data[1] == 0xfa && eeprom->data[0] == 0xfa) {
		/* Extended space for new EEPROM rule */
		/* Follow ethtool -e need to *2 be word */
		offset = 2 * (eeprom->data[3] << 8 | eeprom->data[2]);
		eeprom_get(fd, eeprom, ifr, offset, EEPROM_LEN);

		rule_type = ((eeprom->data[2] >> 3) & 0x02) | ((eeprom->data[2] >> 2) & 0x01);
		switch (rule_type) {
		case EXT_RULE1:
		case EXT_RULE2:
			if (snprintf(bp_id, sizeof(bp_id), "%02x%02x%c-%x%x%x",
											eeprom->data[9],
											eeprom->data[8],
											(char)(eeprom->data[4]),
											(eeprom->data[5] & 0xF8) >> 3,
											(eeprom->data[5] & 0x07) << 2 | (eeprom->data[6] & 0xC0) >> 6,
											(eeprom->data[6] & 0x0E) >> 1)) {
											// Workaround: avoid warning truncation for latest gcc version
											}
			strcpy(bpid, bp_id);

			/* Get card type */
			*card_type = (eeprom->data[10] & 0xf0) >> 4;
			u16_bp_id = eeprom->data[9] << 8 | eeprom->data[8];
			break;
		case EXT_RULE3:
			/* Get the 6-bits grouping data */
			ext_bp_id_digit[0] = eeprom->data[4] >> 2;
			ext_bp_id_digit[1] = ((eeprom->data[4] << 4) & 0x3F) | eeprom->data[5] >> 4;
			ext_bp_id_digit[2] = ((eeprom->data[5] << 2) & 0x3F) | eeprom->data[6] >> 6;
			ext_bp_id_digit[3] = eeprom->data[6] & 0x3F;
			ext_bp_id_digit[4] = eeprom->data[7] >> 2;
			ext_bp_id_digit[5] = eeprom->data[8] >> 3;
			ext_bp_id_digit[6] = ((eeprom->data[8] << 2) & 0x1C) | eeprom->data[9] >> 6;
			ext_bp_id_digit[7] = (eeprom->data[9] >> 1) & 0x1F;

			/* 6-bits mapping to a digit */
			ext_bp_id = get_ext_tbl_mapping(ext_bp_id_digit);

			if (snprintf(bp_id, sizeof(bp_id), "%c%c%c%c%c-%c%c%c", ext_bp_id[0], ext_bp_id[1]
															, ext_bp_id[2], ext_bp_id[3]
															, ext_bp_id[4], ext_bp_id[5]
															, ext_bp_id[6], ext_bp_id[7])) {
				// Workaround: avoid warning truncation for latest gcc version
															}
			strcpy(bpid,bp_id);
			/* Get card type */
			*card_type = (eeprom->data[10] & 0xf0) >> 4;
			break;
		case EXT_RULE4:
			/* Reserved for the futrue */
			break;
		}

	} else {
        /* Old EEPROM rule */
		if (snprintf(bp_id, sizeof(bp_id), "%02x%02x%c", eeprom->data[1], eeprom->data[0], (char)(0x30))) {
			// Workaround: avoid warning truncation for latest gcc version
		}

        strcpy(bpid, bp_id);
		card_bpid_fix(bpid, net, eeprom);

		/* Get card type */
		*card_type = (eeprom->data[2] & 0xf0) >> 4;
		u16_bp_id = eeprom->data[1] << 8 | eeprom->data[0];
		card_type_fix(u16_bp_id, card_type);
    }

	net->bpid = u16_bp_id;
}

void card_bpid_fix(char *bpid, net_t *net, struct ethtool_eeprom *eeprom)
{
	char    bp_id[6] = {'\0'};

	switch (net->device_id) {
	case INTEL_82598EB_10GAFDPNC_TYPE2:
		/* ABN-522 */
		eeprom->data[1] = 0x52;
		eeprom->data[0] = 0x20;
		if (snprintf(bp_id, sizeof(bp_id), "%02x%02x%c", eeprom->data[1], eeprom->data[0], (char)(0x30))) {
			// Workaround: avoid warning truncation for latest gcc version
		}
		strcpy(bpid, bp_id);
		break;
	}	
}

void card_type_fix(u16 u16_bp_id, u8 *card_type)
{
	u8	bypass_num;

	/* Currently, the card scanning tool only support NIP cards/ABN484/ABN482 */
	switch (u16_bp_id) {
	case ABN484:
	case ABN482:
	case ABN522:
		/* Because NO bypass cards of NIP have incorrect information of card type, must force to set ABN card type */
		*card_type = ABN;
		break;
	default:
		bypass_num = (u16_bp_id & 0x00f0) >> 4;

		/* Because NO bypass cards of NIP have incorrect information of card type, must force to set NIP card type */
		if (bypass_num == 0) {
			*card_type = NIP_NID_NIN;
		}
	}
}

void card_name_get(char *bpid, net_t *net, u8 card_type)
{
	char card_type_str[CARD_NAME_LEN] = {'\0'};
	char card_name[CARD_NAME_LEN] = {'\0'};

	switch (card_type) {
	case PPAP_CAPB_CAPW:
		strcpy(card_type_str, "PPAP/CAPB/CAPW");
		break;
	case ABN:
		strcpy(card_type_str, "ABN");
		break;
	case NIP_NID_NIN:
		strcpy(card_type_str, "NIP/NID/NIN");
		break;
	case BPC_NIC:
		strcpy(card_type_str, "BPC/NIC");
		break;
	case CAPK:
		strcpy(card_type_str, "CAPK");
		break;
	case COB:
		strcpy(card_type_str, "COB");
		break;
	case CON:
		strcpy(card_type_str, "CON");
		break;
	case NPC:
		strcpy(card_type_str, "NPC");
		break;
	case CAPF:
		strcpy(card_type_str, "CAPF");
		break;
	case CAPT:
		strcpy(card_type_str, "CAPT");
		break;
	case CB:
		strcpy(card_type_str, "CB");
		break;
	case CCSB_CCSN:
		strcpy(card_type_str, "CCSB/CCSN");
		break;
	}

	if (snprintf(card_name, sizeof(card_name), "%s-%s", card_type_str, bpid)) {
		// Workaround: avoid warning truncation for latest gcc version
	}

	net->card_name = strdup(card_name);
}

void card_name_set(net_t *net, slot_mapping_to_port_table_t *slot_mapping_to_port_table, int net_port_quantity)
{
    struct ifreq ifr;
    int fd;
    int i;

    /* Setup our control structures. */
    for (i = 0; i < net_port_quantity; i++){
        if (net[i].slot_name == NULL) {
            continue;
        }

        if (net[i].card_name == NULL) {
            memset(&ifr, 0, sizeof(ifr));
            strcpy(ifr.ifr_name, net[i].eth_name);

            /* Open control socket for get eeprom content. */
            fd = socket(AF_INET, SOCK_DGRAM, 0);
            geeprom_do(fd, &ifr, net, slot_mapping_to_port_table, i);
        }
    }
}
