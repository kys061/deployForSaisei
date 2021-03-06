#!/bin/bash
shopt -s expand_aliases
alias zz=/opt/stm/target/c.sh

# control on
/opt/stm/target/c.sh -c PUT running police_flows 1

# Create required Access Control Lists - one for non IP traffic
/opt/stm/target/c.sh POST acls/ name ALL-L2
/opt/stm/target/c.sh POST acls/ALL-L2/entries/ name 10 ethertype 0
/opt/stm/target/c.sh POST acls/ name ALL-L3
/opt/stm/target/c.sh POST acls/ALL-L3/entries/ name 10
# Create the Egress Flow Classes used as Bandwidth paritions in the Egress Policy Map
/opt/stm/target/c.sh POST efcs/ name ALL-L3
/opt/stm/target/c.sh POST efcs/ name ALL-L2
# Create the Ingress Flow Classes or Classifiers
/opt/stm/target/c.sh POST ifcs/ name ALL-L3 acl ALL-L3 efc ALL-L3
/opt/stm/target/c.sh POST ifcs/ name ALL-L2 acl ALL-L2 efc ALL-L2
# Create the Egress Policy Map and add the policies to create the bandwidth partitions
/opt/stm/target/c.sh POST epms/ name epm
/opt/stm/target/c.sh POST epms/epm/policies/ name ALL-L3 rate 9500 host_eq true
/opt/stm/target/c.sh POST epms/epm/policies/ name ALL-L2 rate 1000 ass true

/opt/stm/target/c.sh POST epms/ name epmphy
/opt/stm/target/c.sh POST epms/epmphy/policies/ name ALL-L3 rate 9500 host_eq true
/opt/stm/target/c.sh POST epms/epmphy/policies/ name ALL-L2 rate 1000 ass true

# Create the Ingress Policy Map and add the classification policies to handle all traffic
/opt/stm/target/c.sh POST ipms/ name ipm
/opt/stm/target/c.sh POST ipms/ipm/policies/ name ALL-L3 sequence 10000 reverse 1 rpf 0
/opt/stm/target/c.sh POST ipms/ipm/policies/ name ALL-L2 sequence 11000 reverse 1 rpf 0

### POLICY 03 - police FOR VIPUSERS ##
/opt/stm/target/c.sh POST acls/ name VIPUSERS
/opt/stm/target/c.sh POST acls/VIPUSERS/entries/ name 10.255.255.253_32 dest_subnet 10.255.255.253/32 symmetric 1
/opt/stm/target/c.sh POST efcs/ name VIPUSERS
/opt/stm/target/c.sh POST ifcs/ name VIPUSERS acl VIPUSERS efc VIPUSERS
/opt/stm/target/c.sh POST epms/epm/policies/ name VIPUSERS rate 5000 mult 1 assured 1 host_eq 0
/opt/stm/target/c.sh POST epms/epmphy/policies/ name VIPUSERS rate 5000 mult 1 assured 1 host_eq 0
/opt/stm/target/c.sh POST ipms/ipm/policies/ name VIPUSERS sequence 2000 no_police 1 reverse 1 rpf 0

#POLICY  - GURANTEE BANDWIDTH FOR KEY APPLICATIONS ##
/opt/stm/target/c.sh POST app_groups/ name KEYAPPS
/opt/stm/target/c.sh POST app_groups/KEYAPPS/applications/ name sip
/opt/stm/target/c.sh POST efcs/ name KEYAPPS
/opt/stm/target/c.sh POST ifcs/ name KEYAPPS acl ALL-L3 efc KEYAPPS req_groups KEYAPPS
/opt/stm/target/c.sh POST epms/epm/policies/ name KEYAPPS rate 2000 mult 1 assured 1 host_eq 0
/opt/stm/target/c.sh POST epms/epmphy/policies/ name KEYAPPS rate 2000 mult 1 assured 1 host_eq 0
/opt/stm/target/c.sh POST ipms/ipm/policies/ name KEYAPPS sequence 1000 no_police 1 reverse 1 rpf 0



## Create LAG interfaces
#/opt/stm/target/c.sh POST interfaces/ name aggint-ext type lag egress_policy_map epm
#/opt/stm/target/c.sh POST interfaces/ name aggint-int type lag egress_policy_map epm internal true
#/opt/stm/target/c.sh PUT interfaces/aggint-ext ingress_policy_map ipm
#/opt/stm/target/c.sh PUT interfaces/aggint-int ingress_policy_map ipm
#/opt/stm/target/c.sh PUT interfaces/aggint-ext peer aggint-int
#
#/opt/stm/target/c.sh POST interfaces/ name vlan-aggint-ext type lag egress_policy_map epm
#/opt/stm/target/c.sh POST interfaces/ name vlan-aggint-int type lag egress_policy_map epm internal true
#/opt/stm/target/c.sh PUT interfaces/vlan-aggint-ext ingress_policy_map ipm
#/opt/stm/target/c.sh PUT interfaces/vlan-aggint-int ingress_policy_map ipm
#/opt/stm/target/c.sh PUT interfaces/vlan-aggint-ext peer vlan-aggint-int


# Now create the Interfaces, link to the policy maps and enable
/opt/stm/target/c.sh POST interfaces/ name ethseg1-ext type eth egress_policy_map epmphy rate 1000000 system_interface stm1
/opt/stm/target/c.sh POST interfaces/ name ethseg1-int type eth egress_policy_map epmphy rate 1000000 system_interface stm2 internal true
/opt/stm/target/c.sh PUT interfaces/ethseg1-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg1-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg1-ext peer ethseg1-int
/opt/stm/target/c.sh PUT interfaces/ethseg1-ext shape_output true
/opt/stm/target/c.sh PUT interfaces/ethseg1-int shape_output true

/opt/stm/target/c.sh POST interfaces/ name ethseg2-ext type eth egress_policy_map epmphy rate 1000000 system_interface stm3
/opt/stm/target/c.sh POST interfaces/ name ethseg2-int type eth egress_policy_map epmphy rate 1000000 system_interface stm4 internal true
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg2-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext peer ethseg2-int
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext shape_output true
/opt/stm/target/c.sh PUT interfaces/ethseg2-int shape_output true

/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg1-ext type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg1-ext tag any
/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg1-int type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg1-int tag any internal true
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-ext peer vlan-ethseg1-int

/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg2-ext type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg2-ext tag any
/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg2-int type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg2-int tag any internal true
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg2-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg2-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg2-ext peer vlan-ethseg2-int

#/opt/stm/target/c.sh PUT interfaces/aggint-ext state enabled
#/opt/stm/target/c.sh PUT interfaces/aggint-int state enabled
#/opt/stm/target/c.sh PUT interfaces/vlan-aggint-ext state enabled
#/opt/stm/target/c.sh PUT interfaces/vlan-aggint-int state enabled

/opt/stm/target/c.sh PUT interfaces/ethseg1-ext state enabled
/opt/stm/target/c.sh PUT interfaces/ethseg1-int state enabled
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext state enabled
/opt/stm/target/c.sh PUT interfaces/ethseg2-int state enabled

/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-ext state enabled
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-int state enabled
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg2-ext state enabled
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg2-int state enabled

/opt/stm/target/c.sh -r PUT /rest/top/configurations/running save_partition both save_config true
