#!/bin/bash
bypass_masters=/etc/stm/bypass_masters
kernel_ver=4.15.0-20-generic
uname_r=$(uname -r)

rl="$(runlevel | awk '{ print $2  }')"
echo "runlevel is $rl" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
if [ "$rl" -eq 0  ]; then
    echo "runlevel is 0, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    # cooper
    if [ -d /sys/class/bypass/g3bp0 ]; then
      cd /sys/class/bypass/g3bp0
      echo b > bypass
      echo 1 > nextboot
      echo "forcing bypass cooper bump1" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    fi
    if [ -d /sys/class/bypass/g3bp1 ]; then
      cd /sys/class/bypass/g3bp1
      echo b > bypass
      echo 1 > nextboot
      echo "forcing bypass cooper bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    fi
    # fiber
    if [ -e /sys/class/misc/caswell_bpgen2/slot0/bypass0 ]; then
      cd /sys/class/misc/caswell_bpgen2/slot0/
      echo 2 > bypass0
      echo 1 > nextboot0
      echo "forcing bypass fiber bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    fi
    if [ -e /sys/class/misc/caswell_bpgen2/slot0/bypass1 ]; then
      cd /sys/class/misc/caswell_bpgen2/slot0/
      echo 2 > bypass1
      echo 1 > nextboot1
      echo "forcing bypass fiber bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
    fi
else
        if [ "$rl" -eq 6  ]; then
          echo "runlevel is 6, forcing bypass" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
		  # cooper
			if [ -d /sys/class/bypass/g3bp0 ]; then
			  cd /sys/class/bypass/g3bp0
			  echo b > bypass
			  echo 1 > nextboot
      		  echo "forcing bypass cooper bump1" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
			fi
			if [ -d /sys/class/bypass/g3bp1 ]; then
			  cd /sys/class/bypass/g3bp1
			  echo b > bypass
			  echo 1 > nextboot
      		  echo "forcing bypass cooper bump2" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
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
        fi
fi
echo "=== Completed ${0##*/} ===" | awk '{ print strftime(), $0; fflush()  }' >> /var/log/stm_bypass.log
