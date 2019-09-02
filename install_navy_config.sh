
# parameters
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters buffer_pool_size 20000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters flow_pool_size 10000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters host_expansion_pool_size 10000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters host_pool_internal_guarantee 5600 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters host_pool_size 50000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters host_reputation_table_size 1000000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_app_hosts 100000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_app_urls 40000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_app_users 150000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_applications 10000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_efc_instances 100000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_geo_users 6000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_peer_apps 50000 >/dev/null 2>&1
/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running/parameters max_users 2000 >/dev/null 2>&1

# 
/home/saisei/deploy/config/navy-bitwsetup-nolag-2seg-cooper.sh

/opt/stm/target/c.sh -r PUT /rest/stm/configurations/running save_partition both save_config true >/dev/null 2>&1
# bypass
cp /etc/stmfiles/files/scripts/bypass_portwell_monitor.sh /opt/stm/target/portwell-bypass-monitor.sh
cp /etc/stmfiles/files/scripts/bypass_portwell_monitor.sh /opt/stm/target.alt/portwell-bypass-monitor.sh

##sed -i "\$a@reboot (sleep 30 ; sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 34 ; sudo iptables -I INPUT -s 10.0.0.0/8 -j ACCEPT > /dev/null 2>&1)\n@reboot (sleep 36 ; sudo iptables -I INPUT -s 172.16.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 32 ; sudo iptables -I INPUT -s 218.38.155.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 44 ; sudo iptables -I INPUT -s 115.136.221.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 60 ; sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 64 ; sudo iptables -I INPUT -s 10.0.0.0/8 -j ACCEPT > /dev/null 2>&1)\n@reboot (sleep 66 ; sudo iptables -I INPUT -s 172.16.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 62 ; sudo iptables -I INPUT -s 218.38.155.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 64 ; sudo iptables -I INPUT -s 115.136.221.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 90 ; sudo iptables -I INPUT -s 192.168.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 94 ; sudo iptables -I INPUT -s 10.0.0.0/8 -j ACCEPT > /dev/null 2>&1)\n@reboot (sleep 96 ; sudo iptables -I INPUT -s 172.16.0.0/16 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 92 ; sudo iptables -I INPUT -s 218.38.155.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 94 ; sudo iptables -I INPUT -s 115.136.221.0/24 -j ACCEPT > /dev/null 2>&1) \n@reboot (sleep 78 ;  sudo iptables -I  INPUT -p tcp -m multiport --destination-ports 22,5000 -j ACCEPT  > /dev/null 2>&1) \n@reboot (sleep 120 ;  sudo iptables -I  INPUT -p tcp -m multiport --destination-ports 22,5000 -j ACCEPT  > /dev/null 2>&1)" /var/spool/cron/crontabs/root





