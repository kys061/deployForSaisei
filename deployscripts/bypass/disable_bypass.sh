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

fiber_segment1_in_use=$(cat /etc/stmfiles/files/scripts/deployconfig.txt |egrep "^#" -v |grep segment1 |awk -F: '{print  $2}')
fiber_segment2_in_use=$(cat /etc/stmfiles/files/scripts/deployconfig.txt |egrep "^#" -v |grep segment2 |awk -F: '{print  $2}')
scripts_path="/etc/stmfiles/files/scripts/"
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

function disable_portwell_bypass()
{
  # first bump
  # copper
  if [ -d /sys/class/bypass/g3bp0 ]; then
    cd /sys/class/bypass/g3bp0
    bypass_status=$(cat bypass)
    if [ "$bypass_status" != "n" ]; then
      echo "Disabling bypass on bump1" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo 1 >func
      echo n >bypass
    fi
  fi
  # fiber ( 2: bypass, 0: normal(inline))
  #Set to Normal: inline(0)
  #Set to Open: open(1), it drops packet from in bypass ethernet port
  #Set to Bypass: bypass(2)
  for fiber_bump1_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment1_in_use"); do
    if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6} ]; then
      cd /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/
      bypass_status=$(cat ${fiber_bump1_slot_bypass:6})
      if [ "$bypass_status" != "0" ]; then
        echo "Disabling bypass on bump1" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 0 > ${fiber_bump1_slot_bypass:6}
      fi
    fi
  done
  # second bump
  # copper
  if [ -d /sys/class/bypass/g3bp1 ]; then
    cd /sys/class/bypass/g3bp1
    bypass_status=$(cat bypass)
    if [ "$bypass_status" != "n" ]; then
      echo "Disabling bypass on bump2" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo 1 >func
      echo n >bypass
    fi
  fi
  # fiber
  for fiber_bump2_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment2_in_use"); do
    if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6} ]; then
      cd /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/
      bypass_status=$(cat ${fiber_bump2_slot_bypass:6})
      if [ "$bypass_status" != "0" ]; then
        echo "Disabling bypass on bump2" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 0 > ${fiber_bump2_slot_bypass:6}
      fi
    fi
  done
}

echo "=== Start disable_bypass.sh ===" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
bypass_module_check
disable_portwell_bypass
echo "==============================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log