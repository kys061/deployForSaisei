#!/bin/bash
#
#####################################
# Last Date : 2020.3. 28            #
# Writer : yskang(kys061@gmail.com) #
#####################################
#
# Must be run as root.
#
killall -9 bypass_portwell_monitor.sh >>/var/log/stm_bypass.log 2>&1
scripts_path="/etc/stmfiles/files/scripts/"

fiber_segment1_in_use=$(cat /etc/stmfiles/files/scripts/deployconfig.txt |egrep "^#" -v |grep segment1 |awk -F: '{print  $2}')
fiber_segment2_in_use=$(cat /etc/stmfiles/files/scripts/deployconfig.txt |egrep "^#" -v |grep segment2 |awk -F: '{print  $2}')
hw_model=$(cat ${scripts_path}deployconfig.txt |egrep "^#" -v |grep model |awk -F: '{print  $2}')
if [ $hw_model == "fc4000" ]; then
  board="COSD304"
elif [ $hw_model == "fc2000" ]; then
  board="CAR3070"
else
  borad="None"
fi

function bypass_module_check()
{
  # copper
  copper_bypass_mod_installed=$(/sbin/lsmod | grep "caswell_bpgen3")
  if [ -z "$copper_bypass_mod_installed" ]; then
    cd /opt/stm/bypass_drivers/portwell_kr/src/driver
    insmod caswell-bpgen3.ko
  fi
  # fiber
  if [ -d /opt/stm/bypass_drivers/portwell_fiber/driver ]; then
    cd /opt/stm/bypass_drivers/portwell_fiber/driver

    network_bypass=$(lsmod | grep network_bypass | awk '{ print $1 }')
    i2c_i801=$(lsmod | grep i2c_i801 | awk '{ print $1 }')

    if [ -z "$i2c_i801" ]; then
      modprobe i2c-i801
    fi
    if [ -z "$network_bypass" ]; then
      insmod network-bypass.ko board=$board
    fi
  fi
}

function enable_portwell_bypass()
{
  # first bump
  #copper
  if [ -d /sys/class/bypass/g3bp0 ]; then
    cd /sys/class/bypass/g3bp0
    bump1_bypass_status=$(cat bypass)
    if [ "$bump1_bypass_status" != "b" ]; then
      echo "Enabling bypasses on bump1 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo b > bypass
      echo 1 > nextboot
      echo 1 > bpe
    fi
  fi
  # fiber ( 2: bypass, 0: normal(inline))

  for fiber_bump1_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment1_in_use"); do
    if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6} ]; then
      cd /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/
      bump1_bypass_fiber_status=$(cat ${fiber_bump1_slot_bypass:6})
      if [ "$bump1_bypass_fiber_status" != "2" ]; then
        echo "Enabling bypasses on bump1 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 2 > ${fiber_bump1_slot_bypass:6}
        echo 1 > nextboot0
        echo 1 > nextboot1
        echo 1 > bpe0
        echo 1 > bpe1
      fi
    fi
  done

  # second bump
  # copper
  if [ -d /sys/class/bypass/g3bp1 ]; then
    cd /sys/class/bypass/g3bp1
    bump2_bypass_status=$(cat bypass)
    if [ "$bump2_bypass_status" != "b" ]; then
      echo "Enabling bypasses on bump2 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo b > bypass
      echo 1 > nextboot
    fi
  fi
  # fiber  
  for fiber_bump2_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment2_in_use"); do
    if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6} ]; then
      cd /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/
      bump2_bypass_fiber_status=$(cat ${fiber_bump2_slot_bypass:6})
      if [ "$bump2_bypass_fiber_status" != "2" ]; then
        echo "Enabling bypasses on bump2 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 2 > ${fiber_bump2_slot_bypass:6}
        echo 1 > nextboot0
        echo 1 > nextboot1
        echo 1 > bpe0
        echo 1 > bpe1
      fi
    fi
  done
}

echo "=== Start enable_bypass.sh ===" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
bypass_module_check
enable_portwell_bypass
echo "==============================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
