#!/bin/bash
bypass_masters=/etc/stm/bypass_masters
kernel_ver=4.15.0-20-generic
uname_r=$(uname -r)
killall -9 bypass_portwell_monitor.sh >>/var/log/stm_bypass.log 2>&1
#  ps ax | grep "bypass_portwell_monitor.sh" | head -n 1 | cut -d" " -f2

function enable_bypass(){
  # cooper
  if [ -d /sys/class/bypass/g3bp0 ]; then
    cd /sys/class/bypass/g3bp0
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe
    echo "forcing bypass cooper bump1" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp1 ]; then
    cd /sys/class/bypass/g3bp1
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp2 ]; then
    cd /sys/class/bypass/g3bp2
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump3" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp3 ]; then
    cd /sys/class/bypass/g3bp3
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump4" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp4 ]; then
    cd /sys/class/bypass/g3bp4
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump5" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp5 ]; then
    cd /sys/class/bypass/g3bp5
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump6" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp6 ]; then
    cd /sys/class/bypass/g3bp6
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump7" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -d /sys/class/bypass/g3bp7 ]; then
    cd /sys/class/bypass/g3bp7
    echo b > bypass
    echo 1 > nextboot
    echo 1 > bpe    
    echo "forcing bypass cooper bump8" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi

  # fiber
  if [ -e /sys/class/misc/caswell_bpgen2/slot0/bypass0 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot0/
    echo 2 > bypass0
    echo 1 > nextboot0
    echo "forcing bypass fiber bump1" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot0/bypass1 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot0/
    echo 2 > bypass1
    echo 1 > nextboot1
    echo "forcing bypass fiber bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot1/bypass0 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot1/
    echo 2 > bypass0
    echo 1 > nextboot0
    echo "forcing bypass fiber bump3" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot1/bypass1 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot1/
    echo 2 > bypass1
    echo 1 > nextboot1
    echo "forcing bypass fiber bump4" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot2/bypass0 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot2/
    echo 2 > bypass0
    echo 1 > nextboot0
    echo "forcing bypass fiber bump5" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot2/bypass1 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot2/
    echo 2 > bypass1
    echo 1 > nextboot1
    echo "forcing bypass fiber bump6" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot3/bypass0 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot3/
    echo 2 > bypass0
    echo 1 > nextboot0
    echo "forcing bypass fiber bump7" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
  if [ -e /sys/class/misc/caswell_bpgen2/slot3/bypass1 ]; then
    cd /sys/class/misc/caswell_bpgen2/slot3/
    echo 2 > bypass1
    echo 1 > nextboot1
    echo "forcing bypass fiber bump8 | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  fi
}

echo "=== Starting ${0##*/} ===" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
rl="$(runlevel | awk '{ print $2  }')"
echo "runlevel is $rl" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
if [ "$rl" -eq 0  ]; then
  echo "runlevel is 0, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
  enable_bypass
else
  if [ "$rl" -eq 3  ]; then
    echo "runlevel is 3, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    enable_bypass
  fi
  if [ "$rl" -eq 5  ]; then
    echo "runlevel is 5, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    enable_bypass
  fi
  if [ "$rl" -eq 6  ]; then
    echo "runlevel is 6, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    enable_bypass
  fi
fi
echo "=== Completed ${0##*/} ===" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
