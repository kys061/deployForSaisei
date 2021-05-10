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

#include <dirent.h>
#include "ethtool/ethtool-util.h"
#include <libgen.h>
#include <net/if.h>
#include "string.h"

#define VERSION				"V3.1.12" // Version
#define MB_NAME_LEN			32  // Motherboard name length
#define SLOT_NAME_LEN		15  // Slots name length
#define ETH_NAME_LEN		15  // networks interface length
#define CARD_NAME_LEN		50  // Cards name length
#define MAC_LEN				20  // MACs length
#define CONF_PATH_LEN		64  // configuration path length
#define MEM_NAME_LEN		7	// MEM name length
#define PATH_LEN			100  // general path length
#define EEPROM_LEN			16 // eeprom dump length
#define SLOT_QUANTITY		8 // slots quantity
#define MAX_BUS_INFO_NUM	10 // information number of bus of slot


#define PCI_PATH_PROC_BUS_PCI    "/proc/bus/pci"
#define PCI_PATH_SYS_BUS_PCI_DEV "/sys/bus/pci/devices"
#define PCI_PATH_FIND_ROOT_DEV   "/sys/class/pci_bus"
#define CFG_PATH                 "../../conf/mb_cfg"   // Motherboard configurations
#define SLOT_ORDER_LIST          "../../conf/rename.cfg"  // Order setting configration

/* Vendor ID and Device ID for INTEL MAC */
#define INTEL                         0x8086
#define REALTEK                       0x10ec
#define INTEL_82571EB_TYPE1_C         0x105e          // 82571EB Gigabit Ethernet Controller
#define INTEL_82571EB_TYPE2_F         0x105f          // 82571EB Gigabit Ethernet Controller
#define INTEL_82574L_GNC_TYPE1        0x10d3          // 82574L Gigabit Network Connection
#define INTEL_82575EB_GBC             0x10a9          // 82575EB Gigabit Backplane Connectio
#define INTEL_82598EB_10GAFDPNC_TYPE2 0x10e1          // 82598EB 10-Gigabit AF Dual Port Network Connection
#define INTEL_82599EB_10GDPNC_TYPE2   0x10fc          // 82598EB 10-Gigabit Dual Port Network Connection
#define INTEL_82599EB_10GNC           0x10fb          // 82599EB 10-Gigabit Network Connection
#define INTEL_82546GB_GEC_TYPE1_C     0x1079          // 82546GB Gigabit Ethernet Controller
#define INTEL_82541PI_GEC_TYPE1       0x1076          // 82541GI Gigabit Ethernet Controller

#define INTEL_XL710        0x1580       // Ethernet Controller XL710 for 40GbE Controller
#define INTEL_X710         0x1572       // Ethernet Controller X710 for 10GbE SFP+
#define INTEL_XXV710       0x158b       // Ethernet Controller XXV710 for 25GbE SFP28
#define INTEL_XL710_QSFP   0x1583       // Ethernet Controller XL710 for 40GbE QSFP+

//typedef unsigned char   u8;
//typedef unsigned short  u16;
//typedef unsigned int    u32;

/* -=- Define data structure hubel -=- */
typedef struct {
	char *slot_name[SLOT_QUANTITY];
	short net_port_quantity[SLOT_QUANTITY];
} slot_mapping_to_port_table_t;

typedef struct {
	short ext_quantity;	
	long *ext_list;
} ext_slot_t;

typedef struct {
	char *slot_name;
	long *slot_dev_id_list;   // Slot device ID for root (non-onboard)
	short slot_dev_id_quantity;  // slot device quantity by scanning
} slot_dev_t;

/* Slots bus information (PCI bridge of root port) */
typedef struct {
	char slot[SLOT_NAME_LEN];
	char end_root_bus_info[8];
	char bus_info[MAX_BUS_INFO_NUM][8];
} slot_bus_info_t;

/* Slots order */
typedef struct {
	char slot_name[SLOT_NAME_LEN];
} slot_order_t;

/* Mother board information */
typedef struct {
	char mb_name[MB_NAME_LEN];  // Mother board name
	long nb_vid;           // North bridge vendor ID
	long nb_did; 	       // North bridge device ID
	char *nb_pci_bus_info; // North bridge PCI bus information

	long sb_vid;           // South bridge vendor ID
	long sb_did; 	       // South bridge device ID
	char *sb_pci_bus_info; // South bridge PCI bus information

	long *ob_dev_list;	   // Slot device ID for root (onboard)

	short ob_dev_quantity;    // Onboard device quantity - Abel
	short ext_slot_quantity;
	
	ext_slot_t *ext_slot;
	slot_dev_t slot_dev[SLOT_QUANTITY]; // Slots devices for root port
	slot_bus_info_t slot_bus_info[SLOT_QUANTITY]; // Slots bus information for root port
	slot_bus_info_t on_board_bus_info;
	slot_order_t slot_order[SLOT_QUANTITY];
} mb_t;

/* Network information */
typedef struct {
	char *bus_root_info;
    char *address;
    char *eth_name;
    char *slot_name;
    char *card_name;
    long device_id;
    long vendor_id;
	u16 bpid;
} net_t;

/* PCI device information */
typedef struct {
    char *bus_info;   // PCI information
	char *bus_root_info;
    long class_code;  // class code
    long device_id;   // device ID
    long vendor_id;   // vender ID
    net_t net;
    short if_net;
} device_t;

/* Command information */
typedef struct {
	char *card; // Card name
	char *slot; // Slot name
	char *iface; // netaork interface
	char *mac; // MAC
	char *output; // output format
	char *gen_path; // udev setting path
	char *config_path; // motherboard config path
	char *slot_order_config_path; // slot order config path
	short if_rename; // 0 --> non-rename, 1 --> rename
	short if_config_path; // 0 --> default, 1 --> specify path
	short if_slot_order_config_path; // 0 --> default, 1 --> specify path
	short if_appear; // -C, -S: 0 --> Don't appear at the same time, 1 --> appear at the same time
} command_info_t;

typedef enum {
	FALSE,
	TRUE
} decision_t;

/* PCI device class code */
typedef enum {
	ETH_CONTROLLER = 0x020000,
	PCI_BRIDGE = 0x060400,
	UNSIGNED_CLASS_CODE = 0xffffff
} pci_dev_class_code_t;

/* ABN card list */
typedef enum {
	ABN484 = 0x4840,
	ABN482 = 0x4820,
	ABN522 = 0x5220
} abn_list_t;

/* Card type list */
typedef enum {
	PPAP_CAPB_CAPW = 0x00,
	ABN = 0x01,
	NIP_NID_NIN = 0x02,
	BPC_NIC = 0x03,
	CAPK = 0x04,
	COB = 0x05,
	CON = 0x06,
	NPC = 0x07,
	CAPF = 0x08,
	CAPT = 0x09,
	CB = 0x0A,
	CCSB_CCSN = 0x0B
} card_type_t;

/* Extend table of 5-bits and 6-bits mapping to a digit */
typedef enum {
	EXT_DIGIT = 0x00,
	EXT_DIGIT_UPPER_LETTER = 0x0A,
	EXT_DIGIT_LOWER_LETTER = 0x24
} mapping_tbl_t;

/* Extension mapping rule type */
typedef enum {
	EXT_RULE1 = 0,
	EXT_RULE2 = 1,
	EXT_RULE3 = 2,
	EXT_RULE4 = 4
} ext_rule_type_t;

int dev_access(device_t **dev, mb_t *mb);
void network_rename(char *exe, net_t *net, mb_t *mb, int net_port_quantity, char status, char *gen_path, char *slot_order_config_path);
void net_table_make(device_t *device, net_t *net, int pci_dev_quantity, int net_port_quantity, mb_t *mb);
void slot_name_set(device_t *device, mb_t *mb, int pci_dev_quantity);
void net_slot_set(char *pci_path, device_t *device, mb_t *mb, int pci_dev_quantity, char *slot_name);
void slot_ports_table_make(slot_mapping_to_port_table_t *slot_mapping_to_port_table, net_t *net, short net_port_quantity);
int eeprom_dump(struct ethtool_drvinfo *info, struct ethtool_eeprom *ee, int j, net_t *net, slot_mapping_to_port_table_t *slot_mapping_to_port_table);
int geeprom_do(int fd, struct ifreq *ifr, net_t *net, slot_mapping_to_port_table_t *slot_mapping_to_port_table, int i);
void ip(char *net_up, char *net_down, int status);
void mb_info_load(char *exe, char *mb_name, mb_t *mb, command_info_t *command_info);
void mb_t_free(mb_t *mb);
void slot_dev_t_free(slot_dev_t *slot_dev);
void ext_slot_t_free(ext_slot_t *ext_slot, short ext_slot_quantity);
void command_info_t_free(command_info_t *command_info);
void device_t_free(device_t *device, int pci_dev_quantity);
void net_t_free(net_t *net);
void mb_t_init(mb_t *mb);
int net_num_get(device_t *device, int pci_dev_quantity);
void net_table_init(net_t **net_dev, int net_port_quantity);
void command_parse(char *exe, int argc, char *argv[], command_info_t *command_info);
void show(int argc, char *argv[], net_t *net, mb_t *mb, int net_port_quantity, command_info_t *command_info);
void card_name_set(net_t *net, slot_mapping_to_port_table_t *slot_mapping_to_port_table, int net_port_quantity);
void net_t_init(net_t *net);
void mem_free(device_t *device, int pci_dev_quantity, net_t *net, int net_port_quantity, mb_t *mb, command_info_t *command_info);
void slot_order_t_init(slot_order_t *slot_order);
