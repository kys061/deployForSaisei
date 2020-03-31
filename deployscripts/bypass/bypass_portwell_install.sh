#!/bin/bash
#
# Install portwell bypass driver and enable bypass
#
# Must be run as root.
#
kernel_ver=4.15.0-20-generic
uname_r=$(uname -r)

if [ "$uname_r" = "$kernel_ver" ]; then
  bpcooper_ver=V1.21.3
  bpfiber_ver=V3.20.2
else
  bpcooper_ver=V1.13.0
  bpfiber_ver=V3.20.0
fi

hw_model=$(cat /etc/stmfiles/files/scripts/deployconfig.txt |egrep "^#" -v |grep model |awk -F: '{print  $2}')
if [ $hw_model == "fc4000" ]; then
  board="COSD304"
elif [ $hw_model == "fc2000" ]; then
  board="CAR3070"
else
  borad="None"
fi

mkdir -p /opt/stm/bypass_drivers
mkdir -p /opt/stm/bypass_drivers/portwell_kr
mkdir -p /opt/stm/bypass_drivers/portwell_fiber
cp /etc/stmfiles/files/scripts/caswell_drv_network-bypass-$bpfiber_ver.zip /opt/stm/bypass_drivers/portwell_fiber/.
cp /etc/stmfiles/files/scripts/caswell_drv_bypass-gen3-$bpcooper_ver.zip /opt/stm/bypass_drivers/portwell_kr/.
#
# cooper install
#
if [ ! -e /opt/stm/bypass_drivers/portwell_kr/src/driver/caswell-bpgen3.ko ]; then
  cd  /opt/stm/bypass_drivers/portwell_kr
  unzip caswell_drv_bypass-gen3-$bpcooper_ver.zip
  cd src/driver
  make [KSRC=..]

  bypass_mod_installed=$(/sbin/lsmod | grep "caswell_bpgen3")
  if [ ! -z "$bypass_mod_installed" ]; then
    cd  /opt/stm/bypass_drivers/portwell_kr/src/driver
    rmmod caswell-bpgen3.ko
    insmod caswell-bpgen3.ko
  fi

  cd /sys/class/bypass/g3bp0
  echo 1 > func
  echo b > bypass
  echo 1 > nextboot
  echo 1 > bpe
  if [ -d /sys/class/bypass/g3bp1 ];
  then
    cd /sys/class/bypass/g3bp1
    echo 1 > func
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe
  fi
fi

#
# fiber install
#
is_fiber=$(lspci |grep Ether |grep Fiber -o)
is_fiber_sfp=$(lspci |grep Ether |grep SFP -o)
if [ ! -e /opt/stm/bypass_drivers/portwell_fiber/driver/network-bypass.ko ]; then
  if [ ! -z "$is_fiber" -o ! -z "$is_fiber_sfp" ]; then
      cd  /opt/stm/bypass_drivers/portwell_fiber
      unzip caswell_drv_network-bypass-$bpfiber_ver.zip
      cd driver
      make [KSRC=..]

    bypass_mod_installed=$(/sbin/lsmod | grep "network_bypass")
    if [ ! -z "$bypass_mod_installed" ]; then
      rmmod network_bypass
    fi
    i2c_mod_installed=$(/sbin/lsmod |grep "i2c_i801")
    if [ -z "$i2c_mod_installed" ]; then
      modprobe i2c-i801
    fi
    cd /opt/stm/bypass_drivers/portwell_fiber/driver/
    insmod network-bypass.ko board=$board

    if [ -e /sys/class/misc/caswell_bpgen2/slot0/bypass0 ];
    then
      cd /sys/class/misc/caswell_bpgen2/slot0/
      echo 2 > bypass0
      echo 1 > bpe0
      echo 1 > nextboot0
    fi
    if [ -e /sys/class/misc/caswell_bpgen2/slot1/bypass0 ];
    then
      cd /sys/class/misc/caswell_bpgen2/slot1/
      echo 2 > bypass0
      echo 1 > bpe0
      echo 1 > nextboot0
    fi
  fi
fi

# make initial file for reboot and shutdown
if [ "$uname_r" = "$kernel_ver" ]; then # 18.04
  cp /etc/stmfiles/files/scripts/portwell_multi.service /lib/systemd/system/.
  cp /etc/stmfiles/files/scripts/bypass_portwell_enable.sh /etc/init.d/bypass_portwell_enable.sh
  cd /lib/systemd/system/
  systemctl daemon-reload
  systemctl enable portwell_multi.service
else  # 14.04
  if [ ! -e /etc/init.d/bypass_portwell_enable.sh ]; then
    cp /etc/stmfiles/files/scripts/bypass_portwell_enable.sh /etc/init.d/.
      if [ ! -e /etc/rc6.d/K20bypass_portwell_enable.sh ]; then
        # for reboot
        cd /etc/rc6.d
        ln -s ../init.d/bypass_portwell_enable.sh K20bypass_portwell_enable.sh
        echo "make link file in rc6.d"
      fi
      if [ ! -e /etc/rc0.d/K20bypass_portwell_enable.sh ]; then
        # for shutdown
        cd /etc/rc0.d
        ln -s ../init.d/bypass_portwell_enable.sh K20bypass_portwell_enable.sh
        echo "make link file in rc0.d"
      fi
  fi
fi