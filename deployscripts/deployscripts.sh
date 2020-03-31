#!/bin/bash
#####################################
# Last Date : 2020.03.26            #
# Writer : yskang(kys061@gmail.com) #
#####################################

LOG_FILE=/var/log/deployscripts.log

equal_count=0
bypass_cooper_driver=caswell_drv_bypass-gen3-V1.13.0.zip
bypass_fiber_driver=caswell_drv_network-bypass-V3.20.0.zip
bypass7_3_cooper_driver=caswell_drv_bypass-gen3-V1.21.3.zip
bypass7_3_fiber_driver=caswell_drv_network-bypass-V3.20.2.zip
# bypassdrv_target=/etc/stmfiles/files/scripts/
deploy_config_path=/home/saisei/deployscripts/
deploy_bypass_path=/home/saisei/deployscripts/bypass/
scripts_target=/etc/stmfiles/files/scripts/
deploy_threadmonitor_path=/home/saisei/deployscripts/thread_monitor/
threadmonitor_target=/etc/stmfiles/files/scripts/
deploy_report_path=/home/saisei/deployscripts/report/
report_target=/opt/stm/target/files/
report_config_path=/opt/stm/target/files/report/config/report-config-7.3.1-11149.json
report_config_target=/opt/stm/target/files/report/config/report-config.json
jq_path=/home/saisei/deployscripts/

id="cli_admin"
pass="cli_admin"

platform=$(dmidecode -q |grep "Chassis Information" -A10 |grep Type | cut -d ":" -f2 |  tr -d ' ')
version=$(echo 'show version' | sudo /opt/stm/target/pcli/stm_cli.py $id:$pass@localhost | awk '{print $1}' | egrep 'V[0-9]+\.[0-9]+' -o)

red="\033[0;31m"
green="\e[0;32m"
NC="\033[0m"
light_green="\e[92m"
light_red="\e[91m"

ori="\e[0m"
sleep_time=2

function log_info()
{
  local msg=$*
  echo -e $(date -Iseconds) " INFO  $msg" >> $LOG_FILE
  # log_info_and_echo $(date -Iseconds) " INFO  $msg"
}

function log_info_and_echo()
{
  local msg=$*
  log_info $msg
  echo -e $msg
}

function cpFilesToTarget()
{
  # copy bypass to target
  cp ${deploy_bypass_path}${bypass_cooper_driver} ${scripts_target}
  cp ${deploy_bypass_path}${bypass7_3_cooper_driver} ${scripts_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}${bypass_cooper_driver} is coping in ${scripts_target}"
    log_info_and_echo "# ${deploy_bypass_path}${bypass7_3_cooper_driver} is coping in ${scripts_target}"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi
  
  cp ${deploy_bypass_path}${bypass_fiber_driver} ${scripts_target}
  cp ${deploy_bypass_path}${bypass7_3_fiber_driver} ${scripts_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}${bypass_fiber_driver} is coping in ${scripts_target}"
    log_info_and_echo "# ${deploy_bypass_path}${bypass7_3_fiber_driver} is coping in ${scripts_target}"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi
  
  cp ${deploy_bypass_path}bypass_portwell_install.sh ${scripts_target}bypass_portwell_install.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_install.sh is coping in ${scripts_target}bypass_portwell_install.sh"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi

  
  cp ${deploy_bypass_path}bypass_portwell_monitor.sh ${scripts_target}bypass_portwell_monitor.sh
  cp ${deploy_config_path}deployconfig.txt ${scripts_target}deployconfig.txt
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_monitor.sh is coping in ${scripts_target}bypass_portwell_monitor.sh"
    log_info_and_echo "# ${deploy_config_path}deployconfig.txt is coping in ${scripts_target}deployconfig.txt"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi

  cp ${deploy_bypass_path}portwell_multi.service ${scripts_target}portwell_multi.service
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}portwell_multi.service is coping in ${scripts_target}portwell_multi.service"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi
  cp ${deploy_bypass_path}bypass_portwell_enable.sh ${scripts_target}bypass_portwell_enable.sh
  cp ${deploy_bypass_path}enable_bypass.sh ${scripts_target}enable_bypass.sh
  cp ${deploy_bypass_path}disable_bypass.sh ${scripts_target}disable_bypass.sh
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_bypass_path}bypass_portwell_enable.sh is coping in ${scripts_target}bypass_portwell_enable.sh"
    log_info_and_echo "# ${deploy_bypass_path}enable_bypass.sh is coping in ${scripts_target}enable_bypass.sh"
    log_info_and_echo "# ${deploy_bypass_path}disable_bypass.sh is coping in ${scripts_target}disable_bypass.sh"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi
  cp ${deploy_threadmonitor_path}thread_monitor.py ${threadmonitor_target}thread_monitor.py
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_threadmonitor_path}thread_monitor.sh is coping in ${threadmonitor_target}thread_monitor.sh"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi

  cp -R ${deploy_report_path} ${report_target}
  if [ $? -eq 0 ]; then
    log_info_and_echo "# ${deploy_report_path} is coping in ${report_target}"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  fi
}

function installBypassDrv()
{
  # install bypass driver
  ${scripts_target}bypass_portwell_install.sh
}

function changeReportConfig()
{
  id=$(cat deployconfig.txt |egrep "^#" -v |grep id |awk -F: '{print  $2}' |sed  -e 's/\^M//' |sed -e 's/\([a-zA-Z]*\)/"\1"/')
  passwd=$(cat deployconfig.txt |egrep "^#" -v |grep password |awk -F: '{print  $2}' |sed  -e 's/\^M//' |sed -e 's/\([A-Za-Z0-9!#@$%^&]*\)/"\1"/')
  ip=\"http://$(cat deployconfig.txt |egrep "^#" -v |grep ip |awk -F: '{print  $2}' |sed  -e 's/\^M//'):\"
  cat <<< $( ./jq '(.config.common.id = '$id' | .config.common.passwd = '$passwd' | .config.common.ip = '$ip' )' $report_config_path ) > $report_config_target
  if [ $? -eq 0 ]; then
    log_info_and_echo "# change report config done!"
    sleep $sleep_time
    log_info_and_echo "$light_green ...OK! $ori"
  else
    log_info_and_echo "$light_red ...ERROR! check plz. $ori"
  fi
}

function restartApache()
{
  cp -r /opt/stm/target/files/report/stm.conf /etc/apache2/sites-available/
  sudo service apache2 restart
  if [ $? -eq 0 ]; then
    log_info_and_echo "# Apache restarting "
    sleep $sleep_time
    log_info_and_echo "$light_green ...OK! $ori"
  else
    log_info_and_echo "$light_red ...Apache error! check plz. $ori"
  fi
}

#
find ./ -name "*.sh" |xargs chmod 755
find ./ -name "*.py" |xargs chmod 755
chmod 755 /home/saisei/deployscripts/deployscripts.sh
chmod 755 /home/saisei/deployscripts/jq
#
echo "=== Start enable_bypass.sh ===" | awk '{ print strftime(), $0; fflush() }' >> $LOG_FILE
cpFilesToTarget
installBypassDrv
changeReportConfig
restartApache
echo "============= END ============" | awk '{ print strftime(), $0; fflush() }' >> $LOG_FILE