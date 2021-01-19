#!/bin/bash
#
#####################################
# Last Date : 2020.3. 28            #
# Writer : yskang(kys061@gmail.com) #
#####################################
#
# Must be run as root.
#
bump1_operstatus="down"
bump2_operstatus="down"
stm_operstatus="down"
em1_adminstatus="down"
p1p1_adminstatus="down"
eth0_adminstatus="down"
eth1_adminstatus="down"
eth2_adminstatus="down"
eth3_adminstatus="down"
eth4_adminstatus="down"
eth5_adminstatus="down"
dumping_core="false"

# for 7.3
pos_realint=11
pos_pciaddr=11
pos_virt_index=11

# for 7.2
#pos_realint=10
#pos_pciaddr=10
#pos_virt_index=10

bump_type="cooper"
bypass_type="gen3"
model_type="small"
stm_status="false"
id="cli_admin"
pass="cli_admin"

# /sys/class/misc/caswell_bpgen2/slot1
# if use fiber use slot number, if use copper use None
# slot number : |_0_| |_1_| |_2_| |_3_|
fiber_segment1_in_use=$(cat deployconfig.txt |egrep "^#" -v |grep segment1 |awk -F: '{print  $2}')
fiber_segment2_in_use=$(cat deployconfig.txt |egrep "^#" -v |grep segment2 |awk -F: '{print  $2}')
#COSD304 : 2u 10cores
#CAR3070 : 2u 4cores
hw_model=$(cat deployconfig.txt |egrep "^#" -v |grep model |awk -F: '{print  $2}')
if [ $hw_model == "fc4000" ]; then
  board="COSD304"
elif [ $hw_model == "fc2000" ]; then
  board="CAR3070"
else
  borad="None"
fi

sleep 10
# check stm and make system_virt_real_device.csv until stm is up
# virt : the name that users want to make
# real : the name that stm makes
# must be interface ordered in /etc/stm/system_virt_real_device.csv : seg1-ext,int, seg2-ext,int
while ! $stm_status; do
  if echo 'show interfaces' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | egrep '(Socket|ethernet)' >/dev/null 2>&1; then
    version=$(echo 'show version' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | awk '{print $1}' | egrep 'V[0-9]+\.[0-9]+' -o)
    model_type=$(echo 'show parameter model' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost |grep model | awk '{print $3}')
    if [ $version == "V7.1" ]; then
      if [ $model_type == "tiny" ]; then
        bump_count=$(echo 'show interfaces' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | wc -l)
      else
        bump_count=$(echo 'show interfaces' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | wc -l)
      fi
      stm_status='true'
      echo 'virt,real' >/etc/stm/system_virt_real_device.csv
      if [ $model_type == "tiny" ]; then
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
      else
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
      fi
      if [ ! -e /etc/stm/system_virt_real_device.csv ]; then
        echo 'virt,real' >/etc/stm/system_virt_real_device.csv
        if [ $model_type == "tiny" ]; then
          echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
        else
          echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
        fi
        if [ $? -eq 1 ]; then
          sleep 10
          echo 'virt,real' >/etc/stm/system_virt_real_device.csv
          if [ $model_type == "tiny" ]; then
                  echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
          else
                  echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $1 "," $11; fflush()}' >>/etc/stm/system_virt_real_device.csv
          fi
        fi
      fi
    else
      if [ $model_type == "tiny" ]; then
        bump_count=$(echo 'show interfaces' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | wc -l)
      else
        bump_count=$(echo 'show interfaces' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | wc -l)
      fi
      stm_status='true'
      echo 'virt,real' >/etc/stm/system_virt_real_device.csv
      if [ $model_type == "tiny" ]; then
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep external | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 1 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep internal | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 1 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep external | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 2 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep internal | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 2 {print}' >>/etc/stm/system_virt_real_device.csv
      else
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep external | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 1 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep internal | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 1 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep external | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 2 {print}' >>/etc/stm/system_virt_real_device.csv
        echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep internal | awk '{print $1 "," $'$pos_realint'; fflush()}' | awk 'FNR == 2 {print}' >>/etc/stm/system_virt_real_device.csv
      fi
    fi
  else
    echo "stm setup is not enabled or not running.." | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
  fi
  sleep 3
done

#
# get real port (stmx)
#
function get_real_ports() {
virt_port_index=()
virt_port=()
real_port=()

if [ $version == "V7.1" ]; then
  realint_count=$(snmpwalk -v 2c -c public localhost ifIndex | egrep 'INTEGER: [0-9]$' | wc -l)
else
  realint_count=$(echo 'show interfaces select interface system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $10}' | wc -l)
fi

for ((i = 0; i < $realint_count; i++)); do
  if [ $i == 0 ]; then
    if [ $version == "V7.1" ]; then
      virt_port_index[$i]=$(snmpwalk -v 2c -c public localhost ifIndex | egrep 'INTEGER: [0-9]$' | awk 'FNR == 1 {print}' | cut -d " " -f1 | rev | cut -d "." -f1)
    else
      virt_port_index[$i]=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 1 {print}')
    fi
    if [[ $virt_port_index[$i] == '' ]]; then
      virt_port[$i]=''
      real_port[$i]=''
    else
      if [ $version == "V7.1" ]; then
        virt_port[$i]=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 ifName.$virt_port_1_index | rev | cut -d " " -f1 | rev | fgrep -m 1 -v "." | tail -n 1)
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      else
        virt_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep virt -v | cut -d',' -f1 | awk 'FNR == 1 {print}')
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      fi
    fi
  fi
  if [ $i == 1 ]; then
    if [ $version == "V7.1" ]; then
      virt_port_index[$i]=$(snmpwalk -v 2c -c public localhost ifIndex | egrep 'INTEGER: [0-9]$' | awk 'FNR == 1 {print}' | cut -d " " -f1 | rev | cut -d "." -f1)
    else
      virt_port_index[$i]=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 1 {print}')
    fi
    if [[ $virt_port_index[$i] == '' ]]; then
      virt_port[$i]=''
      real_port[$i]=''
    else
      if [ $version == "V7.1" ]; then
        virt_port[$i]=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 ifName.$virt_port_1_index | rev | cut -d " " -f1 | rev | fgrep -m 1 -v "." | tail -n 1)
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      else
        virt_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep virt -v | cut -d',' -f1 | awk 'FNR == 1 {print}')
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      fi
    fi
  fi
  if [ $i == 2 ]; then
    if [ $version == "V7.1" ]; then
      virt_port_index[$i]=$(snmpwalk -v 2c -c public localhost ifIndex | egrep 'INTEGER: [0-9]$' | awk 'FNR == 1 {print}' | cut -d " " -f1 | rev | cut -d "." -f1)
    else
      virt_port_index[$i]=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 1 {print}')
    fi
    if [[ $virt_port_index[$i] == '' ]]; then
      virt_port[$i]=''
      real_port[$i]=''
    else
      if [ $version == "V7.1" ]; then
        virt_port[$i]=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 ifName.$virt_port_1_index | rev | cut -d " " -f1 | rev | fgrep -m 1 -v "." | tail -n 1)
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      else
        virt_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep virt -v | cut -d',' -f1 | awk 'FNR == 1 {print}')
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      fi
    fi
  fi
  if [ $i == 3 ]; then
    if [ $version == "V7.1" ]; then
      virt_port_index[$i]=$(snmpwalk -v 2c -c public localhost ifIndex | egrep 'INTEGER: [0-9]$' | awk 'FNR == 1 {print}' | cut -d " " -f1 | rev | cut -d "." -f1)
    else
      virt_port_index[$i]=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 1 {print}')
    fi
    if [[ $virt_port_index[$i] == '' ]]; then
      virt_port[$i]=''
      real_port[$i]=''
    else
      if [ $version == "V7.1" ]; then
        virt_port[$i]=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 ifName.$virt_port_1_index | rev | cut -d " " -f1 | rev | fgrep -m 1 -v "." | tail -n 1)
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      else
        virt_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep virt -v | cut -d',' -f1 | awk 'FNR == 1 {print}')
        real_port[$i]=$(cat /etc/stm/system_virt_real_device.csv | grep $virt_port_1 | cut -d "," -f2)
      fi
    fi
  fi
done 
}

#
# check bump's interface type (cooper or fiber)
#
function check_bump_type() {
# check bump type
# get pci address of each bump
if [ $version == "V7.1" ]; then
  real_port_1_pci=$(cat /etc/stm/system_devices.csv | grep "$real_port_1\$" | awk -F"," '{print $2}' | cut -d":" -f2,3)
  real_port_3_pci=$(cat /etc/stm/system_devices.csv | grep "$real_port_3\$" | awk -F"," '{print $2}' | cut -d":" -f2,3)
else
  real_port_1_pci=$(cat /etc/stm/devices.csv | grep "$real_port_1" | awk -F"," '{print $3}' | cut -d"\"" -f2 | cut -d":" -f2,3)
  if [ ! -z $real_port_3 ]; then
    real_port_3_pci=$(cat /etc/stm/devices.csv | grep "$real_port_3" | awk -F"," '{print $3}' | cut -d"\"" -f2 | cut -d":" -f2,3)
  else
    real_port_3_pci=""
  fi
fi
# check bump type
if [ -z $real_port_1_pci ]; then
  bump_type="None"
else
  bump_type=$(lspci | grep "$real_port_1_pci" | grep Fiber -o | awk 'FNR == 1 {print}')
  if [ -z $bump_type ]; then
    bump_type=$(lspci | grep "$real_port_1_pci" | grep SFP -o | awk 'FNR == 1 {print}')
  fi
  
  if [ "$bump_type" = "Fiber" ]; then
    bump_type="1Gfiber"
  elif [ "$bump_type" = "SFP" ]; then
    bump_type="10Gfiber"
  else
    bump_type="cooper"
  fi
fi


if [ -z $real_port_3_pci ]; then
  bump2_type="None"
else
  bump2_type=$(lspci | grep "$real_port_3_pci" | grep Fiber -o | awk 'FNR == 1 {print}')
  if [ -z $bump2_type ]; then
          bump2_type=$(lspci | grep "$real_port_3_pci" | grep SFP -o | awk 'FNR == 1 {print}')
  fi

  if [ "$bump2_type" = "Fiber" ]; then
    bump2_type="1Gfiber"
  elif [ "$bump2_type" = "SFP" ]; then
    bump2_type="10Gfiber"
  else
    bump2_type="cooper"
  fi
fi
}

#
# check interface is enabled
#
function check_model_type_enabled() {
if [ $version == "V7.1" ]; then
  if [ $model_type == "tiny" ]; then
    if [ $1 -eq 1 ] && [ $2 -eq 1 ]; then
      bitw_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_1 | awk '{print $4}')
    fi
    if [ $1 -eq 1 ] && [ $2 -eq 2 ]; then
      bitw_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_2 | awk '{print $4}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 1 ]; then
      bitw2_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_3 | awk '{print $4}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 2 ]; then
      bitw2_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_4 | awk '{print $4}')
    fi
  else
    if [ $1 -eq 1 ] && [ $2 -eq 1 ]; then
      bitw_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_1 | awk '{print $4}')
    fi
    if [ $1 -eq 1 ] && [ $2 -eq 2 ]; then
      bitw_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_2 | awk '{print $4}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 1 ]; then
      bitw2_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_3 | awk '{print $4}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 2 ]; then
      bitw2_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_4 | awk '{print $4}')
    fi
  fi
else
  if [ $model_type == "tiny" ]; then
    if [ $1 -eq 1 ] && [ $2 -eq 1 ]; then
      bitw_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_1 | awk '{print $6}')
    fi
    if [ $1 -eq 1 ] && [ $2 -eq 2 ]; then
      bitw_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_2 | awk '{print $6}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 1 ]; then
      bitw2_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_3 | awk '{print $6}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 2 ]; then
      bitw2_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep Socket | grep $real_port_4 | awk '{print $6}')
    fi
  else
    if [ $1 -eq 1 ] && [ $2 -eq 1 ]; then
      bitw_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_1 | awk '{print $6}')
    fi
    if [ $1 -eq 1 ] && [ $2 -eq 2 ]; then
      bitw_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_2 | awk '{print $6}')
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 1 ]; then
      bitw2_port_1_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_3 | awk '{print $6}')
      if [ -z $bitw2_port_1_enable ]; then
        bitw2_port_1_enable='None'
      fi
    fi
    if [ $1 -eq 2 ] && [ $2 -eq 2 ]; then
      bitw2_port_2_enable=$(echo 'show interfaces select system_interface' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | grep $real_port_4 | awk '{print $6}')
      if [ -z $bitw2_port_2_enable ]; then
        bitw2_port_2_enable='None'
      fi
    fi
  fi
fi
}

#
# check bump's status (default:two)
# TODO: chang variable to array.
#
function check_bumps() {
bitw_port=()
bitw2_port=()
bitw3_port=()
bitw4_port=()
# bitw_port_1=$virt_port_1
# bitw_port_2=$virt_port_2
# port3=$virt_port_3
# port4=$virt_port_4
bitw_port[0]=$virt_port[0]
bitw_port[1]=$virt_port[1]
# bitw2_port[0]=$virt_port[2]
# bitw2_port[1]=$virt_port[3]
# check if each port is diff or not
# if [ ! -z $port3 ] && [ ! -z $port4 ] && [ "$port3" != "$bitw_port_1" ] && [ "$port3" != "$bitw_port_2" ] && [ "$port4" != "$bitw_port_1" ] && [ "$port4" != "$bitw_port_2" ]; then
if [ $realint_count -eq 4 ]; then
  bitw2_port[0]=$virt_port[2]
  bitw2_port[1]=$virt_port[3]
  seg_count=2
elif [ $realint_count -eq 6 ]; then
  bitw2_port[0]=$virt_port[2]
  bitw2_port[1]=$virt_port[3]
  bitw3_port[0]=$virt_port[4]
  bitw3_port[1]=$virt_port[5]
  seg_count=3
elif [ $realint_count -eq 8 ]; then
  bitw2_port[0]=$virt_port[2]
  bitw2_port[1]=$virt_port[3]
  bitw3_port[0]=$virt_port[4]
  bitw3_port[1]=$virt_port[5]
  bitw4_port[0]=$virt_port[6]
  bitw4_port[1]=$virt_port[7]
  seg_count=4
else
  seg_count=1
fi
# TODO: add bump3 and bump4
# get bump1's port1 adminstatus and enabled.
if [ ! -z $bitw_port_1 ]; then
  if [ $version == "V7.1" ]; then
    bitw_port_1_index=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 $bitw_port_1 | cut -d " " -f1 | rev | cut -d "." -f1)
    bitw_port_1_adminstatus=$(snmpget -v 2c -c public localhost ifAdminStatus.$bitw_port_1_index | cut -d" " -f4 | awk -F"(" '{print $1}')
    check_model_type_enabled 1 1
    bitw_port_1_pci=$(cat /etc/stm/system_interfaces.csv | grep $real_port_1 | cut -d "," -f2 | sed 's/\://g' | sed 's/\.//g')
  else
    bitw_port_1_index=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 1 {print}')
    bitw_port_1_adminstatus=$(echo 'show interfaces select interface system_interface uid admin_status' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep $bitw_port_1_index | awk '{print $14}')
    bitw_port_1_adminstatus=$(echo "$bitw_port_1_adminstatus" | awk '{print tolower($0)}')
    check_model_type_enabled 1 1
  fi
fi

# get bump1's port2 adminstatus and enabled.
if [ ! -z $bitw_port_1_adminstatus ]; then
  if [ ! -z $bitw_port_2 ]; then
    if [ $version == "V7.1" ]; then
      bitw_port_2_index=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 $bitw_port_2 | cut -d " " -f1 | rev | cut -d "." -f1)
      bitw_port_2_adminstatus=$(snmpget -v 2c -c public localhost ifAdminStatus.$bitw_port_2_index | cut -d " " -f4 | awk -F"(" '{print $1}')
      check_model_type_enabled 1 2
      bitw_port_2_pci=$(cat /etc/stm/system_interfaces.csv | grep $real_port_2 | cut -d"," -f2 | sed 's/\://g' | sed 's/\.//g')
    else
      bitw_port_2_index=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 2 {print}')
      bitw_port_2_adminstatus=$(echo 'show interfaces select interface system_interface uid admin_status' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep $bitw_port_2_index | awk '{print $14}')
      bitw_port_2_adminstatus=$(echo "$bitw_port_2_adminstatus" | awk '{print tolower($0)}')
      check_model_type_enabled 1 2
    fi
  fi
fi
# get bump2's port1 adminstatus and enabled.
if [ ! -z $bitw2_port_1 ]; then
  if [ $version == "V7.1" ]; then
    bitw2_port_1_index=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 $bitw2_port_1 | cut -d " " -f1 | rev | cut -d "." -f1)
    bitw2_port_1_adminstatus=$(snmpget -v 2c -c public localhost ifAdminStatus.$bitw2_port_1_index | cut -d" " -f4 | awk -F"(" '{print $1}')
    check_model_type_enabled 2 1
    bitw2_port_1_pci=$(cat /etc/stm/system_interfaces.csv | grep $real_port_3 | cut -d "," -f2 | sed 's/\://g' | sed 's/\.//g')
  else
    bitw2_port_1_index=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 3 {print}')
    bitw2_port_1_adminstatus=$(echo 'show interfaces select interface system_interface uid admin_status' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep $bitw2_port_1_index | awk '{print $14}')
    bitw2_port_1_adminstatus=$(echo "$bitw2_port_1_adminstatus" | awk '{print tolower($0)}')
    check_model_type_enabled 2 1
  fi
fi
# get  bump2's port2 adminstatus and enabled
if [ ! -z $bitw2_port_1_adminstatus ]; then
  if [ ! -z $bitw2_port_2 ]; then
    if [ $version == "V7.1" ]; then
      bitw2_port_2_index=$(snmpwalk -v 2c -c public localhost ifName | grep -m 1 $bitw2_port_2 | cut -d " " -f1 | rev | cut -d "." -f1)
      bitw2_port_2_adminstatus=$(snmpget -v 2c -c public localhost ifAdminStatus.$bitw2_port_2_index | cut -d " " -f4 | awk -F"(" '{print $1}')
      check_model_type_enabled 2 2
      bitw2_port_2_pci=$(cat /etc/stm/system_interfaces.csv | grep $real_port_4 | cut -d"," -f2 | sed 's/\://g' | sed 's/\.//g')
    else
      bitw2_port_2_index=$(echo 'show interfaces select interface system_interface uid' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep ethernet | awk '{print $'$pos_virt_index'}' | awk 'FNR == 4 {print}')
      bitw2_port_2_adminstatus=$(echo 'show interfaces select interface system_interface uid admin_status' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | grep $bitw2_port_2_index | awk '{print $14}')
      bitw2_port_2_adminstatus=$(echo "$bitw2_port_2_adminstatus" | awk '{print tolower($0)}')
      check_model_type_enabled 2 2
    fi
  fi
fi

# TODO: add bump3 and bump4's cooper and fiber
# 1. check if each bump's ports is up and enabled or not,
# 2. if bump's ports are not up or enabled, let status DOWN,
# 3. if bump's ports are up and enabled, not thread hang of interface, let status UP
if [ ! -z $bitw_port_2_adminstatus ]; then
  if [ "$bitw_port_1_adminstatus" == "up" ]; then
    if [ "$bitw_port_2_adminstatus" == "up" ]; then
      if [ "$bump_type" == "cooper" ]; then
        if [ -d /sys/class/bypass/g3bp0 ]; then
          if [ $bitw_port_1_enable == "enabled" ] && [ $bitw_port_2_enable == "enabled" ]; then
            if [ $model_type == "tiny" ]; then
              # check interface thread hang
              if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                ps -elL | grep $virt_port_2 >/dev/null 2>&1
                if [ $? -eq 0 ]; then
                  bump1_operstatus="up"
                fi
              else
                stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                  #reboot
                # done
              fi
            else
              # check interface thread hang
              ps -elL | grep $virt_port_1 >/dev/null 2>&1
              if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                bump1_operstatus="up"
              else
                stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                  #reboot
                # done
              fi
            fi
          fi
          if [ -d /sys/class/bypass/g3bp1 ]; then
            if [ "$bitw2_port_1_enable" == "enabled" ] && [ "$bitw2_port_2_enable" == "enabled" ]; then
              if [ $model_type == "tiny" ]; then
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  if ps -elL | grep $virt_port_4 >/dev/null 2>&1; then
                    bump2_operstatus="up"
                  fi
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  done
                fi
              else
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  bump2_operstatus="up"
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              fi
            fi
          fi
        fi
      else
        for fiber_bump1_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment1_in_use")
        do
          if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6} ]; then
            if [ "$bitw_port_1_enable" == "enabled" ] && [ "$bitw_port_2_enable" == "enabled" ]; then
              if [ $model_type == "tiny" ]; then
                # check interface thread hang
                if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                  if ps -elL | grep $virt_port_2 >/dev/null 2>&1; then
                    bump1_operstatus="up"
                  fi
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              else
                # check interface thread hang
                if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                  bump1_operstatus="up"
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              fi
            fi
          fi
        done
      fi
    fi
  fi
fi

if [ ! -z $bitw2_port_2_adminstatus ]; then
  if [ "$bitw2_port_1_adminstatus" == "up" ]; then
    if [ "$bitw2_port_2_adminstatus" == "up" ]; then
      if [ "$bump2_type" == "cooper" ]; then
        if [ -d /sys/class/bypass/g3bp0 ]; then
          if [ "$bitw_port_1_enable" == "enabled" ] && [ "$bitw_port_2_enable" == "enabled" ]; then
            if [ $model_type == "tiny" ]; then
              # check interface thread hang
              if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                      if ps -elL | grep $virt_port_2 >/dev/null 2>&1; then
                              bump1_operstatus="up"
                      fi
              else
                      stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                      #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                              #reboot
                      #done
              fi
            else
              # check interface thread hang
              if ps -elL | grep $virt_port_1 >/dev/null 2>&1; then
                      bump1_operstatus="up"
              else
                      stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                      #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                              #reboot
                      #done
              fi
            fi
          fi
          if [ -d /sys/class/bypass/g3bp1 ]; then
            if [ "$bitw2_port_1_enable" == "enabled" ] && [ "$bitw2_port_2_enable" == "enabled" ]; then
              if [ $model_type == "tiny" ]; then
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  if ps -elL | grep $virt_port_4 >/dev/null 2>&1; then
                    bump2_operstatus="up"
                  fi
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              else
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  bump2_operstatus="up"
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              fi
            fi
          fi
        fi
      else
        for fiber_bump2_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep $fiber_segment2_in_use)
        do
          if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6} ]; then
            if [ "$bitw2_port_1_enable" == "enabled" ] && [ "$bitw2_port_2_enable" == "enabled" ]; then
              if [ $model_type == "tiny" ]; then
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  if ps -elL | grep $virt_port_4 >/dev/null 2>&1; then
                    bump2_operstatus="up"
                  fi
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              else
                # check interface thread hang
                if ps -elL | grep $virt_port_3 >/dev/null 2>&1; then
                  bump2_operstatus="up"
                else
                  stm_process_num=($(ps -ef | grep stm$ | awk '{print $2}'))
                  #for ((i = 0; i < ${#stm_process_num[@]}; i++)); do
                    #reboot
                  # done
                fi
              fi
            fi
          fi
        done
      fi
    fi
  fi
fi
# logging bump's status.
echo "Bump1 operstatus" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo $bump1_operstatus | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "Bump2 operstatus" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo $bump2_operstatus | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log

# case bump count is 2
if [ $seg_count -eq 2 ]; then
  if [ "$bump1_operstatus" == "up" ]; then
    echo "bump1_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    if [ "$bump2_operstatus" == "up" ]; then
      echo "bump2_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      stm_operstatus="up"
      echo "stm_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    else
      echo "bump2 operstatus not up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    fi
  else
    echo "bump1 operstatus not up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    if [ "$bump2_operstatus" == "up" ]; then
      echo "bump2_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo "stm_operstatus not up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    else
      echo "bump2 operstatus not up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
      echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    fi
  fi
# case bump count is 1
else
  if [ "$bump1_operstatus" == "up" ]; then
    echo "bump1_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    stm_operstatus="up"
    echo "stm_operstatus up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
  else
    echo "bump1 operstatus not up" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
  fi
fi
}

function rotate_log() {
  MAXLOG=5
  MAXSIZE=20480000
  log_name=/var/log/stm_bypass.log
  file_size=$(du -b $log_name | tr -s '\t' ' ' | cut -d' ' -f1)
  if [ $file_size -gt $MAXSIZE ]; then
    for i in $(seq $((MAXLOG - 1)) -1 1); do
      if [ -e $log_name"."$i ]; then
        mv $log_name"."{$i,$((i + 1))}
      fi
    done
    mv $log_name $log_name".1"
    touch $log_name
  fi
}

function enable_portwell_bypass()
{
# first bump
if [ $1 -eq 1 ]; then
  if [ "$bump_type" == "cooper" ]; then
    if [ -d /sys/class/bypass/g3bp0 ]; then
      cd /sys/class/bypass/g3bp0
      bump1_bypass_status=$(cat bypass)
      if [ "$bump1_bypass_status" != "b" ]; then
        echo "Enabling bypasses on bump1 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo b > bypass
        echo 1 > nextboot
      fi
    fi
  else
    for fiber_bump1_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment1_in_use"); do
      if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6} ]; then
        cd /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/
        bump1_bypass_fiber_status=$(cat ${fiber_bump1_slot_bypass:6})
        if [ "$bump1_bypass_fiber_status" != "2" ]; then
          echo "Enabling bypasses on bump1 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
          echo 2 > ${fiber_bump1_slot_bypass:6}
          echo 1 > nextboot0
        fi
      fi
    done
  fi
fi
# second bump
if [ $1 -eq 2 ]; then
  if [ "$bump2_type" == "cooper" ]; then
          if [ -d /sys/class/bypass/g3bp1 ]; then
          cd /sys/class/bypass/g3bp1
          bump2_bypass_status=$(cat bypass)
                  if [ "$bump2_bypass_status" != "b" ]; then
                          echo "Enabling bypasses on bump2 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
                          echo b > bypass
                          echo 1 > nextboot
                  fi
  fi
  else
    for fiber_bump2_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment2_in_use"); do
      if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6} ]; then
        cd /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/
        bump2_bypass_fiber_status=$(cat ${fiber_bump2_slot_bypass:6})
        if [ "$bump2_bypass_fiber_status" != "2" ]; then
          echo "Enabling bypasses on bump2 " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
          echo 2 > ${fiber_bump2_slot_bypass:6}
          echo 1 > nextboot0
        fi
      fi
    done
  fi
fi
}

function disable_portwell_bypass()
{
# first bump
if [ $1 -eq 1 ]; then
  if [ "$bump_type" == "cooper" ]; then
    if [ -d /sys/class/bypass/g3bp0 ]; then
      cd /sys/class/bypass/g3bp0
      bypass_status=$(cat bypass)
      if [ "$bypass_status" != "n" ]; then
        echo "Disabling bypass on bump1" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 1 >func
        echo n >bypass
      fi
    fi
  else
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
  fi
fi
# second bump
if [ $1 -eq 2 ]; then
  if [ "$bump2_type" == "cooper" ]; then
    if [ -d /sys/class/bypass/g3bp1 ]; then
      cd /sys/class/bypass/g3bp1
      bypass_status=$(cat bypass)
      if [ "$bypass_status" != "n" ]; then
        echo "Disabling bypass on bump2" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
        echo 1 >func
        echo n >bypass
      fi
    fi
  else
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
  fi
fi
}

function fiber_module_check()
{
if [ "$bump_type" == "1Gfiber" ] || [ "$bump_type" == "10Gfiber" ]; then
  if [ -d /opt/stm/bypass_drivers/portwell_fiber/driver ]; then
    cd /opt/stm/bypass_drivers/portwell_fiber/driver

    bypass_mod_installed=$(/sbin/lsmod | grep "network_bypass")
    network_bypass=$(lsmod | grep network_bypass | awk '{ print $1 }')
    i2c_i801=$(lsmod | grep i2c_i801 | awk '{ print $1 }')
    if [ ! -z "$bypass_mod_installed" ]; then
      rmmod network_bypass
    fi
    if [ -z $i2c_i801 ]; then
      modprobe i2c-i801
    fi
    if [ -z $network_bypass ]; then
      insmod network-bypass.ko board=$board
    fi
  fi
  sleep 5

  echo 'slotNum:bypassNum' > /etc/stm/system_fiber_slots
  fiber_slot0=($(ls /sys/class/misc/caswell_bpgen2/ |egrep slot0 -o))
  fiber_slot1=($(ls /sys/class/misc/caswell_bpgen2/ |egrep slot1 -o))
  fiber_slot2=($(ls /sys/class/misc/caswell_bpgen2/ |egrep slot2 -o))
  fiber_slot3=($(ls /sys/class/misc/caswell_bpgen2/ |egrep slot3 -o))
  if [ -z $fiber_slot0 ]; then
    fiber_slot0=None
  else
    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot0/ |egrep bypass0 -o >/dev/null 2>&1; then
      fiber_slot0_bypass0=($fiber_slot0:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot0/ |egrep bypass0 -o ))
      echo "$fiber_slot0_bypass0" >> /etc/stm/system_fiber_slots
    fi

    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot0/ |egrep bypass1 -o >/dev/null 2>&1; then
      fiber_slot0_bypass1=($fiber_slot0:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot0/ |egrep bypass1 -o ))
      echo "$fiber_slot0_bypass1" >> /etc/stm/system_fiber_slots
    fi
  fi

  if [ -z $fiber_slot1 ]; then
    fiber_slot1=None
  else
    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot1/ |egrep bypass0 -o >/dev/null 2>&1; then
      fiber_slot1_bypass0=($fiber_slot1:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot1/ |egrep bypass0 -o ))
      echo "$fiber_slot1_bypass0" >> /etc/stm/system_fiber_slots
    fi

    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot1/ |egrep bypass1 -o >/dev/null 2>&1; then
      fiber_slot1_bypass1=($fiber_slot1:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot1/ |egrep bypass1 -o ))
      echo "$fiber_slot1_bypass1" >> /etc/stm/system_fiber_slots
    fi
  fi

  if [ -z $fiber_slot2 ]; then
    fiber_slot2=None
  else
    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot2/ |egrep bypass0 -o >/dev/null 2>&1; then
      fiber_slot2_bypass0=($fiber_slot2:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot2/ |egrep bypass0 -o ))
      echo "$fiber_slot2_bypass0" >> /etc/stm/system_fiber_slots
    fi

    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot2/ |egrep bypass1 -o >/dev/null 2>&1; then
      fiber_slot2_bypass1=($fiber_slot2:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot2/ |egrep bypass1 -o ))
      echo "$fiber_slot2_bypass1" >> /etc/stm/system_fiber_slots
    fi
  fi

  if [ -z $fiber_slot3 ]; then
    fiber_slot3=None
  else
    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot3/ |egrep bypass0 -o >/dev/null 2>&1; then
      fiber_slot3_bypass0=($fiber_slot3:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot3/ |egrep bypass0 -o ))
      echo "$fiber_slot3_bypass0" >> /etc/stm/system_fiber_slots
    fi

    if ls /sys/class/misc/caswell_bpgen2/$fiber_slot3/ |egrep bypass1 -o >/dev/null 2>&1; then
      fiber_slot3_bypass1=($fiber_slot3:$(ls /sys/class/misc/caswell_bpgen2/$fiber_slot3/ |egrep bypass1 -o ))
      echo "$fiber_slot3_bypass1" >> /etc/stm/system_fiber_slots
    fi
  fi
fi
}

function bypass_status_check()
{
if [ "$bump_type" == "cooper" ]; then
  if [ -d /sys/class/bypass/g3bp0 ]; then
    bitw1_cooper_bypass=$(cat /sys/class/bypass/g3bp0/bypass)
  else
    bitw1_cooper_bypass='None'
  fi
  if [ -d /sys/class/bypass/g3bp1 ]; then
    bitw2_cooper_bypass=$(cat /sys/class/bypass/g3bp1/bypass)
  else
    bitw2_cooper_bypass='None'
  fi
else
  if [ "$fiber_segment1_in_use" = "None" ]; then
    bitw1_fiber_bypass='None'
  else
    for fiber_bump1_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment1_in_use"); do
      if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6} ]; then
        bitw1_fiber_bypass=$(cat /sys/class/misc/caswell_bpgen2/${fiber_bump1_slot_bypass:0:5}/${fiber_bump1_slot_bypass:6})
      fi
    done
  fi
  if [ "$fiber_segment2_in_use" = "None" ]; then
    bitw2_fiber_bypass='None'
  else
    for fiber_bump2_slot_bypass in $(cat /etc/stm/system_fiber_slots |egrep "$fiber_segment2_in_use"); do
      if [ -e /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6} ]; then
        bitw2_fiber_bypass=$(cat /sys/class/misc/caswell_bpgen2/${fiber_bump2_slot_bypass:0:5}/${fiber_bump2_slot_bypass:6})
      fi
    done
  fi
fi
}

function print_bypass_status()
{
if [ "$bump_type" == "cooper" ]; then
        echo "cooper bypass status(bump1, bump2 | b=bypass, n=normal) : "$bitw1_cooper_bypass","$bitw2_cooper_bypass | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
else
        echo "fiber bypass status(bump1, bump2 | 2=bypass, 0=normal) : "$bitw1_fiber_bypass","$bitw2_fiber_bypass | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
fi
if [ -z $bitw_port_1_enable ]; then
  bitw_port_1_enable="None"
fi
if [ -z $bitw_port_2_enable ]; then
  bitw_port_2_enable="None"
fi
if [ -z $bitw2_port_1_enable ]; then
  bitw2_port_1_enable="None"
fi
if [ -z $bitw2_port_2_enable ]; then
  bitw2_port_2_enable="None"
fi
echo "bump1's port1,2 : "$bitw_port_1_enable","$bitw_port_2_enable | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "bump2's port1,2 : "$bitw2_port_1_enable","$bitw2_port_2_enable | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "model type : "$model_type | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "bump1's type, bump2's type : "$bump_type","$bump2_type | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "segment count : "$seg_count | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "stm's operstatus : "$stm_operstatus", bump1's status : "$bump1_operstatus" , bump2's status : "$bump2_operstatus | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
echo "===========================" | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
}

# main logic
# 1. get real and virt port
# 2. check type of bump (fiber) and do checking module is installed and loaded..
# 3. if stm_operstatus is down, just go checking bump's status again
# 4. if stm_operstatus is up and each bump's status is up, change bypass to normal mode for each bump if the status of bypass is bypass mode
# 5. if stm_operstatus is up and each bump's status is down, change bypass to bypass mode for each bump if the status of bypass is normal mode
echo "=== Start bypass-monitor === " | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
get_real_ports
check_bump_type
fiber_module_check
check_bumps

while true; do
  rotate_log
  get_real_ports
  check_bump_type
  fiber_module_check
  if [ $stm_operstatus != "up" ]; then
    echo "stm_operstatus "$stm_operstatus | awk '{ print strftime(), $0; fflush() }' >>/var/log/stm_bypass.log
    enable_portwell_bypass 1
    enable_portwell_bypass 2
    check_bumps
  else
    if [ "$bump1_operstatus" == "up" ]; then
      disable_portwell_bypass 1
      check_bumps
    else
      enable_portwell_bypass 1
      check_bumps
    fi
    if [ "$bump2_operstatus" == "up" ]; then
      disable_portwell_bypass 2
      check_bumps
    else
      enable_portwell_bypass 2
      check_bumps
    fi
  fi
  # check bypass status
  bypass_status_check
  # print bypass status
  print_bypass_status
  sleep 10
done
