#!/bin/bash
shopt -s expand_aliases
alias zz=/opt/stm/target/c.sh
# Control on
/opt/stm/target/c.sh -c PUT running police_flows 1
# Create required Access Control Lists - one for non IP traffic
/opt/stm/target/c.sh POST acls/ name not-ip
/opt/stm/target/c.sh POST acls/not-ip/entries/ name 10 ethertype 0
/opt/stm/target/c.sh POST acls/ name all-ip
/opt/stm/target/c.sh POST acls/all-ip/entries/ name 10
# Create the Egress Flow Classes used as Bandwidth paritions in the Egress Policy Map
/opt/stm/target/c.sh POST efcs/ name all-ip
/opt/stm/target/c.sh POST efcs/ name not-ip
# Create the Ingress Flow Classes or Classifiers
/opt/stm/target/c.sh POST ifcs/ name all-ip acl all-ip efc all-ip
/opt/stm/target/c.sh POST ifcs/ name not-ip acl not-ip efc not-ip
# Create the Egress Policy Map and add the policies to create the bandwidth partitions
/opt/stm/target/c.sh POST epms/ name epm
/opt/stm/target/c.sh POST epms/epm/policies/ name all-ip rate 90000 host_eq true
/opt/stm/target/c.sh POST epms/epm/policies/ name not-ip rate 10000 ass true

/opt/stm/target/c.sh POST epms/ name epmphy
/opt/stm/target/c.sh POST epms/epmphy/policies/ name all-ip rate 90000 host_eq true
/opt/stm/target/c.sh POST epms/epmphy/policies/ name not-ip rate 10000 ass true

# Create the Ingress Policy Map and add the classification policies to handle all traffic
/opt/stm/target/c.sh POST ipms/ name ipm
/opt/stm/target/c.sh POST ipms/ipm/policies/ name all-ip sequence 10000 reverse 1 rpf 1
/opt/stm/target/c.sh POST ipms/ipm/policies/ name not-ip sequence 11000 reverse 1 rpf 1

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
/opt/stm/target/c.sh POST interfaces/ name ethseg1-ext type eth_socket egress_policy_map epmphy rate 1000000 system_interface stm5
/opt/stm/target/c.sh POST interfaces/ name ethseg1-int type eth_socket egress_policy_map epmphy rate 1000000 system_interface stm6 internal true
/opt/stm/target/c.sh PUT interfaces/ethseg1-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg1-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg1-ext peer ethseg1-int

/opt/stm/target/c.sh POST interfaces/ name ethseg2-ext type eth_socket egress_policy_map epmphy rate 1000000 system_interface stm7
/opt/stm/target/c.sh POST interfaces/ name ethseg2-int type eth_socket egress_policy_map epmphy rate 1000000 system_interface stm8 internal true
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg2-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/ethseg2-ext peer ethseg2-int

/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg1-ext type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg1-ext tag 100
/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg1-int type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg1-int tag 100 internal true
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-ext ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-int ingress_policy_map ipm
/opt/stm/target/c.sh PUT interfaces/vlan-ethseg1-ext peer vlan-ethseg1-int

/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg2-ext type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg2-ext tag 100
/opt/stm/target/c.sh POST interfaces/ name vlan-ethseg2-int type vlan egress_policy_map epmphy rate 1000000 outer_interface ethseg2-int tag 100 internal true
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

