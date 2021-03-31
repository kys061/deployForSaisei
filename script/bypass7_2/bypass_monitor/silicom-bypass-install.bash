#!/bin/bash
#
# Install silicom bypass driver and enable bypass
#
# Writer : yskang(kys061@gmail.com)
# Must be run as root.
#

pci_nums=($(lspci -m | grep -i Ethernet |grep Silicom |awk '{print $1}'))

#for pci_num in $pci_num; do
#	silicom_card_name=$(cat /etc/stm/devices.csv |grep $pci_num |cut -d"," -f1 |cut -d"\"" -f2)
#done

mkdir -p /opt/stm/bypass_drivers
rm -rf /opt/stm/bypass_drivers/silicom
mkdir -p /opt/stm/bypass_drivers/silicom

#
# silicom fiber install
#
cp bp_ctl-5.0.65.1.tar.gz /opt/stm/bypass_drivers/silicom/.
cd  /opt/stm/bypass_drivers/silicom
tar xzvf bp_ctl-5.0.65.1.tar.gz
cd bp_ctl-5.0.65.1
make install
bypass_mod_installed=$(/sbin/lsmod | grep "bpctl_mod")
if [ ! -z "$bypass_mod_installed" ]; then
  rmmod bpctl_mod.ko
fi

sudo modprobe bpctl_mod.ko

for pci_num in "${pci_nums[@]}"; do
	silicom_card_name=bypass_$(cat /etc/stm/devices.csv |grep $pci_num |cut -d"," -f1 |cut -d"\"" -f2)
	if [ -e /proc/net/bypass/$silicom_card_name/bypass ]; then
		echo "off" > /proc/net/bypass/$silicom_card_name/bypass
		echo "on" > /proc/net/bypass/$silicom_card_name/dis_bypass
		echo "off" > /proc/net/bypass/$silicom_card_name/bypass_pwup
		echo "on" > /proc/net/bypass/$silicom_card_name/bypass_pwoff
	fi
done
#echo "off" > /proc/net/bypass/bypass_p6p3/bypass
#echo "on" > /proc/net/bypass/bypass_p6p3/dis_bypass
#echo "off" > /proc/net/bypass/bypass_p6p3/bypass_pwup
#echo "on" > /proc/net/bypass/bypass_p6p3/bypass_pwoff