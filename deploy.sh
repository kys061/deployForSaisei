#!/bin/bash
#####################################
# Last Date : 2019.09.02            #
# Writer : yskang(kys061@gmail.com) #
#####################################

LOG_FILE=/var/log/deploy_setup_$(date -Iseconds).log

equal_count=0
bypass_cooper_driver=caswell_drv_bypass-gen3-V1.13.0.zip
bypass_fiber_driver=caswell_drv_network-bypass-V3.20.0.zip
bypass7_3_cooper_driver=caswell_drv_bypass-gen3-V1.21.3.zip
bypass7_3_fiber_driver=caswell_drv_network-bypass-V3.20.2.zip
#deploy_bypass_path=/home/saisei/deploy/script/bypass/
deploy_bypass7_1_path=/home/saisei/deploy/script/bypass7_2/bypass_monitor/7_1/
deploy_bypass_path=/home/saisei/deploy/script/bypass7_2/bypass_monitor/
deploy_recorder_path=/home/saisei/deploy/script/flow_recorder7.2/
bypass_target=/opt/stm/target/
bypass_alt_target=/opt/stm/target.alt/
bypass7_2_target=/etc/stmfiles/files/scripts/
recorder_target=/etc/stmfiles/files/scripts/
config_path=/home/saisei/deploy/config/
user_listener_config_filename=/etc/stmfiles/files/scripts/user_listener_config.py
recorder_filename=/etc/stmfiles/files/scripts/flow_recorder.py
grub_path=/etc/default/grub

id="cli_admin"
pass="cli_admin"

platform=$(dmidecode -q |grep "Chassis Information" -A10 |grep Type | cut -d ":" -f2 |  tr -d ' ')
version=$(echo 'show version' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | awk '{print $1}' | egrep 'V[0-9]+\.[0-9]+' -o)

red="\033[0;31m"
green="\e[0;32m"
NC="\033[0m"
sleep_time=2

function log_info()
{
    local msg=$*
    echo $(date -Iseconds) " INFO  $msg" >> $LOG_FILE
    # log_info_and_echo $(date -Iseconds) " INFO  $msg"
}

function log_info_and_echo()
{
    local msg=$*
    log_info $msg
    echo $msg
}

function cpFilesToTarget()
{
  # copy bypass to target
  cp ${deploy_bypass_path}${bypass_cooper_driver} ${bypass_target}
  cp ${deploy_bypass_path}${bypass_cooper_driver} ${bypass7_2_target}
  cp ${deploy_bypass_path}${bypass7_3_cooper_driver} ${bypass7_2_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "${deploy_bypass_path}${bypass_cooper_driver} is coping in ${bypass_target}"
  	log_info_and_echo "${deploy_bypass_path}${bypass_cooper_driver} is coping in ${bypass7_2_target}"
  	log_info_and_echo "${deploy_bypass_path}${bypass7_3_cooper_driver} is coping in ${bypass7_2_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_bypass_path}${bypass_fiber_driver} ${bypass_target}
  cp ${deploy_bypass_path}${bypass_fiber_driver} ${bypass7_2_target}
  cp ${deploy_bypass_path}${bypass7_3_fiber_driver} ${bypass7_2_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}${bypass_fiber_driver} is coping in ${bypass_target}"
  	log_info_and_echo "${deploy_bypass_path}${bypass_fiber_driver} is coping in ${bypass7_2_target}"
  	log_info_and_echo "${deploy_bypass_path}${bypass7_3_fiber_driver} is coping in ${bypass7_2_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_bypass_path}bypass_portwell_install.sh ${bypass_target}bypass_portwell_install.sh
  cp ${deploy_bypass_path}bypass_portwell_install.sh ${bypass7_2_target}bypass_portwell_install.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_install.sh is coping in ${bypass7_2_target}bypass_portwell_install.sh"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  if [ $version == "V7.1" ]; then
    cp ${deploy_bypass7_1_path}bypass_portwell_monitor.sh ${bypass7_2_target}bypass_portwell_monitor.sh
    if [ $? -eq 0 ]; then
      log_info_and_echo "# ${deploy_bypass7_1_path}bypass_portwell_monitor.sh is coping in ${bypass7_2_target}bypass_portwell_monitor.sh"
      sleep $sleep_time
      log_info_and_echo "...OK!"
    fi
  else
    cp ${deploy_bypass_path}bypass_portwell_monitor.sh ${bypass7_2_target}bypass_portwell_monitor.sh
    if [ $? -eq 0 ]; then
      log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_monitor.sh is coping in ${bypass7_2_target}bypass_portwell_monitor.sh"
      sleep $sleep_time
      log_info_and_echo "...OK!"
    fi
  fi
  cp ${deploy_bypass_path}portwell_multi.service ${bypass7_2_target}portwell_multi.service
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}portwell_multi.service is coping in ${bypass7_2_target}portwell_multi.service"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_bypass_path}bypass_portwell_enable.sh ${bypass7_2_target}bypass_portwell_enable.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_enable.sh is coping in ${bypass7_2_target}bypass_portwell_enable.sh"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_bypass_path}enable_bypass.sh ${bypass7_2_target}enable_bypass.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}enable_bypass.sh is coping in ${bypass7_2_target}enable_bypass.sh"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi

  # copy flow_recorder_mod to /etc/stmfiles/files/script
  cp ${deploy_recorder_path}flow_recorder_mod.py ${recorder_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_recorder_path}flow_recorder_mod.py is coping in ${recorder_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_recorder_path}flow_recorder.py ${recorder_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_recorder_path}flow_recorder.py is coping in ${recorder_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi
  cp ${deploy_recorder_path}flow_recorder_monitor.py ${recorder_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_recorder_path}flow_recorder_monitor.py is coping in ${recorder_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
  fi

  rm -f /opt/stm/target/portwell-bypass-monitor.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# portwell-bypass-monitor.sh is deleting in ${bypass_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
    cp ${bypass7_2_target}bypass_portwell_monitor.sh ${bypass_target}portwell-bypass-monitor.sh
  fi

  rm -f /opt/stm/target.alt/portwell-bypass-monitor.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# portwell-bypass-monitor.sh is deleting in ${bypass_alt_target}"
    sleep $sleep_time
    log_info_and_echo "...OK!"
    cp ${bypass7_2_target}bypass_portwell_monitor.sh ${bypass_alt_target}portwell-bypass-monitor.sh
  fi
}

function setUserAndRecConfig()
{
  # make user listener config
  if [ -e /etc/stmfiles/files/scripts/user_listener_config_template.py ]; then
    cp /etc/stmfiles/files/scripts/user_listener_config_template.py $user_listener_config_filename
    sleep $sleep_time
    log_info_and_echo "making user listener config......OK!"
  else
    log_info_and_echo '#### there is no /etc/stmfiles/files/scripts/user_listener_config_template.py'
  fi

  # set config files.
  internals=($(cat INTERNAL))
  if [ -e INTERNAL ] && [ ! -z internals ]; then
      # setting user listener config
      if [ -e $user_listener_config_filename ]; then
        old_user_internals=($(cat $user_listener_config_filename |egrep "'[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\/[0-9]+'," -o |egrep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\/[0-9]+" -o))
        sed -i "/^INCLUDE/,/]/s/'0.0.0.0\/0'\,/#'0.0.0.0\/0'\,/g" $user_listener_config_filename
        sed -i "/^INCLUDE/,/]/s/'::\/0',/#'::\/0'\,/g" $user_listener_config_filename
        sed -i "/^REVERSE_DNS_INCLUDE/,/]/s/'0.0.0.0\/0'\,/#'0.0.0.0\/0'\,/g" $user_listener_config_filename
        sed -i "/^REVERSE_DNS_INCLUDE/,/]/s/'::\/0',/#'::\/0'\,/g" $user_listener_config_filename
        for ((i=${#internals[@]}-1; i>=0; i--));
        do
          for ((j=${#old_user_internals[@]}-1; j>=0; j--))
            do
              if [ ${internals[$i]} = ${old_user_internals[$j]} ]; then
                equal_count=$[$equal_count +1]
              fi
            done
  			  if [ $equal_count = 0 ]; then
    				sed -i "/^INCLUDE/a'${internals[$i]}'," $user_listener_config_filename
    				log_info_and_echo "# Adding ${internals[$i]} in $user_listener_config_filename"
    				sleep $sleep_time
    				log_info_and_echo "...OK!"
			    fi
        done
      else
          log_info_and_echo "#### there is no $user_listener_config_filename"
      fi
      # setting flow recorder config
      equal_count=0
      if [ -e $recorder_filename ]; then
  	     old_flow_internals=($(cat $recorder_filename |egrep "'[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\/[0-9]+'," -o |egrep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\/[0-9]+" -o))
         for ((i=${#internals[@]}-1; i>=0; i--));
         do
  	        for ((j=${#old_flow_internals[@]}-1; j>=0; j--))
  	         do
               if [ ${internals[$i]} = ${old_flow_internals[$j]} ]; then
  	           equal_count=$[$equal_count +1]
  	           fi
  	         done
            if [ $equal_count = 0 ]; then
              sed -i "/^INCLUDE/a'${internals[$i]}'," $recorder_filename
  		        log_info_and_echo "# Adding ${internals[$i]} in $recorder_filename"
  		        sleep $sleep_time
  		        log_info_and_echo "...OK!"
  	       fi
         done
       else
         log_info_and_echo "#### there is no $recorder_filename"
       fi
  else
    log_info_and_echo "#### there is no INTERNAL"
  fi
}
function installBypassDrv()
{
  # install bypass driver
  ${bypass_target}bypass_portwell_install.sh
}
# maybe not used unless specific circumstance
function setBypassConfig()
{
  # cooper config
  if [ "$1" == "cooper" ]; then
  ${config_path}bitwsetup-portwell-lag-2seg-copper.sh
  fi

  # fiber config
  if [ "$1" == "fiber" ]; then
  ${config_path}bitwsetup-portwell-lag-2seg-fiber.sh
  fi
}
function setGrubConfig()
{
  # sudo apt-get purge biosdevname -y
  # sudo update-initramfs -u
  # change grub setting
  sed -i "/^GRUB_CMDLINE_LINUX_DEFAULT=/s/hugepages=1024\"/hugepages=1024 isolcpus=1-2\"/g" $grub_path
  if [ $? -eq 0 ]; then
  	update-grub
    log_info_and_echo "# changed grub setting"
    sleep $sleep_time
    log_info_and_echo "...OK!"
    # chattr +i /etc/default/grub
  fi
}
function setOtherConfig()
{
  # add monitor users
  /opt/stm/target/c.sh -r POST /rest/stm/configurations/running/administrators name monitor_only password monitor_only enabled true >/dev/null 2>&1
  if [ $? -eq 0 ]; then
      log_info_and_echo "# monitor account was created!"
      sleep $sleep_time
      log_info_and_echo "...OK!"
  fi

  # change run_on_boot true for user_listener.py
  /opt/stm/target/c.sh -r PUT /rest/top/configurations/running/scripts/user_listener.py run_on_boot true >/dev/null 2>&1
  if [ $? -eq 0 ]; then
      log_info_and_echo "# changed run_on_boot as true!"
      sleep $sleep_time
      log_info_and_echo "...OK!"
  fi

  # addtional app
  apt-get install htop
  apt-get install dos2unix

  # save config
  /opt/stm/target/c.sh -r PUT /rest/stm/configurations/running save_partition both save_config true >/dev/null 2>&1
  if [ $? -eq 0 ]; then
      log_info_and_echo "# save configs well!"
      sleep $sleep_time
      log_info_and_echo "...OK!"
  fi
}

if [ $platform == "Desktop" ]; then
  cpFilesToTarget
  setUserAndRecConfig
  installBypassDrv
  setGrubConfig
  setOtherConfig
else
  cpFilesToTarget
  setUserAndRecConfig
  setOtherConfig
fi
# run on start
#if [ -e ${recorder_target}flow_recorder.py ]; then
#/opt/stm/target/c.sh PUT scripts/flow_recorder.py run_on_boot true >/dev/null 2>&1
#fi
#if [ $? -eq 0 ]; then
#    log_info_and_echo "# flow_recorder.py is set as run on boot"
#fi
#if [ -e ${recorder_target}flow_recorder_monitor.py ]; then
#/opt/stm/target/c.sh PUT scripts/flow_recorder_monitor.py run_on_boot true >/dev/null 2>&1
#fi
#if [ $? -eq 0 ]; then
#    log_info_and_echo "# flow_recorder_monitor.py is set as run on boot"
#fi
