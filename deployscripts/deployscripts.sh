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
bypass7_3_fiber_driver=caswell_drv_network-bypass-V3.21.0.zip
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
pmbus_path=/home/saisei/deployscripts/pmbus/
lmsensor_path=/home/saisei/deployscripts/lm-sensors/
lcd_path=/home/saisei/deployscripts/lcd/


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

function checkaftercmd()
{
  is_done=$1
  src_path=$2
  filename=$3
  dst_path=$4
  if [ $1 -eq 0 ]; then
    log_info_and_echo "# ${src_path}${filename} is coping in ${dst_path}"
    sleep $sleep_time
    log_info_and_echo "...$light_green OK! $ori"
  else
    log_info_and_echo "$light_red ...ERROR! check plz. $ori"
  fi
}

function cpFilesToTarget()
{
  # copy bypass to target
  cp ${deploy_bypass_path}${bypass_cooper_driver} ${scripts_target}
  checkaftercmd $? ${deploy_bypass_path} ${bypass_cooper_driver} ${scripts_target}

  cp ${deploy_bypass_path}${bypass7_3_cooper_driver} ${scripts_target}
  checkaftercmd $? ${deploy_bypass_path} ${bypass7_3_cooper_driver} ${scripts_target}

  
  cp ${deploy_bypass_path}${bypass_fiber_driver} ${scripts_target}
  checkaftercmd $? ${deploy_bypass_path} ${bypass_fiber_driver} ${scripts_target}


  cp ${deploy_bypass_path}${bypass7_3_fiber_driver} ${scripts_target}
  checkaftercmd $? ${deploy_bypass_path} ${bypass7_3_fiber_driver} ${scripts_target}
  
  cp ${deploy_bypass_path}bypass_portwell_install.sh ${scripts_target}bypass_portwell_install.sh
  checkaftercmd $? ${deploy_bypass_path} bypass_portwell_install.sh ${scripts_target}bypass_portwell_install.sh


  
  cp ${deploy_bypass_path}bypass_portwell_monitor.sh ${scripts_target}bypass_portwell_monitor.sh
  checkaftercmd $? ${deploy_bypass_path} bypass_portwell_monitor.sh ${scripts_target}bypass_portwell_monitor.sh

  cp ${deploy_bypass_path}bypass_portwell_monitor_v2.sh ${scripts_target}bypass_portwell_monitor_v2.sh
  checkaftercmd $? ${deploy_bypass_path} bypass_portwell_monitor_v2.sh ${scripts_target}bypass_portwell_monitor_v2.sh


  cp ${deploy_bypass_path}bypass_portwell_monitor.py ${scripts_target}bypass_portwell_monitor.py
  checkaftercmd $? ${deploy_bypass_path} bypass_portwell_monitor.py ${scripts_target}bypass_portwell_monitor.py

  cp ${deploy_config_path}deployconfig.txt ${scripts_target}deployconfig.txt
  checkaftercmd $? ${deploy_config_path} deployconfig.txt ${scripts_target}deployconfig.txt

  cp ${deploy_bypass_path}portwell_multi.service ${scripts_target}portwell_multi.service
  checkaftercmd $? ${deploy_bypass_path} portwell_multi.service ${scripts_target}portwell_multi.service


  cp ${deploy_bypass_path}bypass_portwell_enable.sh ${scripts_target}bypass_portwell_enable.sh
  checkaftercmd $? ${deploy_bypass_path} bypass_portwell_enable.sh ${scripts_target}bypass_portwell_enable.sh
  
  cp ${deploy_bypass_path}enable_bypass.sh ${scripts_target}enable_bypass.sh
  checkaftercmd $? ${deploy_bypass_path} enable_bypass.sh ${scripts_target}enable_bypass.sh

  cp ${deploy_bypass_path}disable_bypass.sh ${scripts_target}disable_bypass.sh
  checkaftercmd $? ${deploy_bypass_path} disable_bypass.sh ${scripts_target}disable_bypass.sh

  cp ${deploy_threadmonitor_path}thread_monitor.py ${threadmonitor_target}thread_monitor.py
  checkaftercmd $? ${deploy_threadmonitor_path} thread_monitor.sh ${threadmonitor_target}thread_monitor.sh

  cp ${deploy_threadmonitor_path}thread_monitor_v2.py ${threadmonitor_target}thread_monitor_v2.py
  checkaftercmd $? ${deploy_threadmonitor_path} thread_monitor_v2.sh ${threadmonitor_target}thread_monitor_v2.sh

  cp -R ${deploy_report_path} ${report_target}
  checkaftercmd $? ${deploy_report_path} " " ${report_target}


  cp ${pmbus_path}caswell_drv_pmbus-V1.19.0.zip ${scripts_target}
  checkaftercmd $? ${pmbus_path} caswell_drv_pmbus-V1.19.0.zip ${scripts_target}


  cp ${lmsensor_path}bison-3.4.1.tar.gz ${scripts_target}
  checkaftercmd $? ${lmsensor_path} bison-3.4.1.tar.gz ${scripts_target}


  cp ${lmsensor_path}flex-2.6.3.tar.gz ${scripts_target}
  checkaftercmd $? ${lmsensor_path} flex-2.6.3.tar.gz ${scripts_target}

  cp ${lmsensor_path}lm-sensors-3-5-0.zip ${scripts_target}
  checkaftercmd $? ${lmsensor_path} lm-sensors-3-5-0.zip ${scripts_target}


  cp ${lcd_path}build/lcdd.service ${scripts_target}
  checkaftercmd $? ${lcd_path}build/ lcdd.service ${scripts_target}

}

function installBypassDrv()
{
  # install bypass driver
  ${scripts_target}bypass_portwell_install.sh
}

function changeReportConfig()
{
  id=$(cat ${deploy_config_path}deployconfig.txt |egrep "^#" -v |grep id |awk -F: '{print  $2}' |sed  -e 's/\^M//' |sed -e 's/\([a-zA-Z]*\)/"\1"/')
  passwd=$(cat ${deploy_config_path}deployconfig.txt |egrep "^#" -v |grep password |awk -F: '{print  $2}' |sed  -e 's/\^M//' |sed -e 's/\([A-Za-Z0-9!#@$%^&]*\)/"\1"/')
  ip=\"http://$(cat ${deploy_config_path}deployconfig.txt |egrep "^#" -v |grep ip |awk -F: '{print  $2}' |sed  -e 's/\^M//'):\"
  cat <<< $( ${jq_path}jq '(.config.common.id = '$id' | .config.common.passwd = '$passwd' | .config.common.ip = '$ip' )' $report_config_path ) > $report_config_target
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

function add_serial_console()
{
  is_console=$(cat /etc/default/grub |grep GRUB_CMDLINE_LINUX= |grep console=tty1 -o)
  is_terminal=$(cat /etc/default/grub |grep "console serial")
  is_serial_cmd=$(cat /etc/default/grub |grep "serial --speed=115200")

  if [ -z "$is_serial_cmd" ]; then
  sed -i 's/\(^GRUB_CMDLINE_LINUX=\"[a-zA-Z0-9_=. ]*\"$\)/\1\nGRUB_SERIAL_COMMAND=\"serial --speed=115200 --unit=0 --word=8 --parity=no --stop=1\"/' /etc/default/grub
  fi

  if [ -z "$is_terminal" ]; then
  sed -i 's/\(^GRUB_CMDLINE_LINUX=\"[a-zA-Z0-9_=. ]*\"$\)/\1\nGRUB_TERMINAL=\"console serial\"/' /etc/default/grub
  fi

  if [ -z "$is_console" ]; then
    sed -i 's/\(^GRUB_CMDLINE_LINUX=\"[a-zA-Z0-9_=. ]*\)/\1 console=tty1 console=ttyS0,115200/' /etc/default/grub
  fi
  
  update-grub
  if [ $? -eq 0 ]; then
    log_info_and_echo "# add serial "
    sleep $sleep_time
    log_info_and_echo "$light_green ...OK! $ori"
  else
    log_info_and_echo "$light_red ...Error! check plz. $ori"
  fi
}

function set_crontab()
{
  crontab -l > file
  echo "15 0 1 * * find /etc/stmfiles/files/cores/* -type f,d -ctime +7 -exec rm -rf {} \;" > file
  echo "@reboot (sleep 10 ; printf \"#\"'!""'""\"""/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3600) \n    \" > /opt/stm/target/python/mapui.py)" >> file
  echo "@reboot (sleep 10 ; printf \"#\"'!""'""\"""/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4000) \n    \" > /opt/stm/target/call_home.py)" >> file
  echo "@reboot (sleep 10 ; printf \"#\"'!""'""\"""/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4400) \n    \" > /opt/stm/target/restful_call_home.py)" >> file
  echo "@reboot (sleep 10 ; printf \"#\"'!""'""\"""/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4800) \n    \" > /opt/stm/target/python/auto_upgrade.py)" >> file
  echo "@reboot (sleep 120 ; sudo /etc/stmfiles/files/scripts/bypass_portwell_monitor.sh & > /dev/null 2>&1)" >> file
  echo "#@reboot (sleep 120 ; sudo /etc/stmfiles/files/scripts/bypass_portwell_monitor.py & > /dev/null 2>&1)" >> file
  echo "@reboot (sleep 60 ;  sudo iptables -I  INPUT -p tcp -m multiport --destination-ports 22,5000 -j ACCEPT > /dev/null 2>&1)" >> file
  echo "@reboot (sleep 180 ; cp -r /home/saisei/deployscripts/report/stm.conf /etc/apache2/sites-available/ > /dev/null 2>&1)" >> file
  echo "@reboot (sleep 200 ; sudo service apache2 restart > /dev/null 2>&1)" >> file
  echo "@reboot (sleep 240 ; sudo /etc/stmfiles/files/scripts/thread_monitor_v2.py & > /dev/null 2>&1)" >> file  
  crontab file
  rm file

  if [ $? -eq 0 ]; then
    log_info_and_echo "# setting crontab "
    sleep $sleep_time
    log_info_and_echo "$light_green ...OK! $ori"
  else
    log_info_and_echo "$light_red ...Error! check plz. $ori"
  fi
}


function namingMgmt()
{
  pci_count=($(lspci -m |grep Ether |grep I2 | cut -d " " -f1))
  # test=$(/etc/stmfiles/files/scripts/dpdk_nic_bind.py -s |grep -A 3 "Network devices using kernel driver" | cut -d " " -f1 |egrep [0-9A-Za-z]+:[0-9A-Za-z]+:[0-9A-Za-z]+.[0-9A-Za-z]+)
  if [ ${#pci_count[@]} -gt 2 ]; then
    echo "cooper interfaces are too many,, check plz.."
    break
  else
    if [ ! -e /etc/udev/rules.d/70-persistent-net.rules ]; then
      touch /etc/udev/rules.d/70-persistent-net.rules
      # pci_address=($(lspci -m |grep Ether |grep I2 | cut -d " " -f1))
      # mac=$(cat /etc/stm/devices.csv |grep ${pci_address[$i]} |cut -d "," -f6 |tr -d '"')
      MGMTCOUNTER=0
      for pci in $(lspci -m |grep Ether |grep I2 | cut -d " " -f1); do
        mac=$(cat /etc/stm/devices.csv |grep ${pci} |cut -d "," -f6 |tr -d '"')
        echo "SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"${mac}\", NAME=\"mgmt${MGMTCOUNTER}\" is added to rules.."
        echo "SUBSYSTEM==\"net\", ACTION==\"add\", DRIVERS==\"?*\", ATTR{address}==\"${mac}\", NAME=\"mgmt${MGMTCOUNTER}\"" >> /etc/udev/rules.d/70-persistent-net.rules
        MGMTCOUNTER=$[$MGMTCOUNTER +1]
        if [ MGMTCOUNTER -ge 2 ]; then
          break
        fi
      done
    fi
  fi
}

function install_pmbus_drv()
{
  ${pmbus_path}install_pmbus.sh
  cas_pmb_ctrl --help
}

function install_lmsensors()
{
  ${lmsensor_path}install_lmsensors.sh
  sensors
}

function install_lcd()
{
  cp ${lcd_path}build/lcdd /usr/sbin/
  checkaftercmd $? ${lcd_path}build/ lcdd /usr/sbin/

  cp ${scripts_target}lcdd.service /lib/systemd/system/
  checkaftercmd $? ${lcd_path}build/ lcdd.service /lib/systemd/system/

  sudo systemctl enable lcdd.service
  if [ $? -eq 0 ]; then
    log_info_and_echo "# Enabling lcdd.service ..."
    sleep $sleep_time
    log_info_and_echo "$light_green ...OK! $ori"
  else
    log_info_and_echo "$light_red ...ERROR! check plz. $ori"
  fi
}


#
find ./ -name "*.sh" |xargs chmod 755
find ./ -name "*.py" |xargs chmod 755
chmod 755 /home/saisei/deployscripts/deployscripts.sh
chmod 755 /home/saisei/deployscripts/jq
#
echo "=== Start deployscripts.sh ===" | awk '{ print strftime(), $0; fflush() }' >> $LOG_FILE
if [ $platform == "Desktop" ]; then
  cpFilesToTarget
  installBypassDrv
  changeReportConfig
  restartApache
  add_serial_console
  set_crontab
  install_pmbus_drv
  install_lmsensors
  install_lcd
  # namingMgmt
else
  cpFilesToTarget
  changeReportConfig
  restartApache
fi
echo "============= END ============" | awk '{ print strftime(), $0; fflush() }' >> $LOG_FILE