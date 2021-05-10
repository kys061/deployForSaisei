#!/bin/bash
#
LOG=/var/log/check_bypass_script.log

script_name=bypass_portwell_monitor.py
script_path=/etc/stmfiles/files/scripts/
prefix=check_bypass


function check_bypass_script(){ 
        ps_count=$(ps -ef |grep "${script_name}" |grep -v grep |wc -l) 
        if [ $ps_count -eq 0 ]; then
            echo -e "${script_name} is starting.." | awk '{ print strftime("%Y-%m-%d %T") " - '$prefix' - ", $0; fflush() }' >> $LOG
            sudo $script_path${script_name} &
        else
            echo -e "No need to start ${script_name}" | awk '{ print strftime("%Y-%m-%d %T") " - '$prefix' - ", $0; fflush() }' >> $LOG
        fi
}

function rotate_log() {
  MAXLOG=5
  MAXSIZE=20480000
  log_name=/var/log/check_bypass_script.log
  file_size=$(du -b $log_name | tr -s '\t' ' ' | cut -d' ' -f1)
  if [ $file_size -gt $MAXSIZE ]; then
    for i in $(seq $((MAXLOG - 1)) -1 1); do
      if [ -e $log_name"."$i ]; then
        mv $log_name"."{$i,$((i + 1))}
      fi
    done
    if [ ! -e $log_name ]; then
      touch $log_name
    fi
    mv $log_name $log_name".1"
    touch $log_name
  fi
}

# main
echo "=== ${0##*/} started ===" | awk '{ print strftime("%Y-%m-%d %T"), $0; fflush() }' >> $LOG
check_bypass_script
rotate_log