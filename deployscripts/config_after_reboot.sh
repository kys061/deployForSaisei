# Model: Small-8G(군프로젝트 납품전용 - 50M트래픽 처리 - 8GB메모리 탑재시)
# 설치메모리 사이즈 – 8GB, 256GBSSD  
# 50M IMIX 트래픽 처리용
# 아래 config적용후 메모리 사용량 약 5.9GB (7.1버전에서는 4.5기가정도 소모되었으나 7.3.1버전에서는 2기가정도 추가 소요되는 관계로 가능하면 16GB메모리 사용 권장)
 opt/stm/target/c.sh PUT parameters model small
/opt/stm/target/c.sh PUT parameters app_host_queue_size 2000
/opt/stm/target/c.sh PUT parameters app_user_queue_size 2000
/opt/stm/target/c.sh PUT parameters buffer_pool_size 30000
/opt/stm/target/c.sh PUT parameters default_assigned_port_table_size 4000
/opt/stm/target/c.sh PUT parameters flow_pool_size 25000
/opt/stm/target/c.sh PUT parameters flow_table_size 65536
/opt/stm/target/c.sh PUT parameters geo_user_queue_size 2000
/opt/stm/target/c.sh PUT parameters host_pool_internal_guarantee 5600
/opt/stm/target/c.sh PUT parameters host_pool_size 20000
/opt/stm/target/c.sh PUT parameters host_queue_size 3000
/opt/stm/target/c.sh PUT parameters host_efci_queue_size 3000

/opt/stm/target/c.sh PUT parameters max_app_hosts 100000
/opt/stm/target/c.sh PUT parameters max_app_urls 40000
/opt/stm/target/c.sh PUT parameters max_app_users 100000
/opt/stm/target/c.sh PUT parameters max_applications 10000
/opt/stm/target/c.sh PUT parameters max_geo_users 1000
/opt/stm/target/c.sh PUT parameters max_peer_apps 10000
/opt/stm/target/c.sh PUT parameters peer_app_queue_size 2000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true

# Model: Small
# 최소설치시 default 모델 설정값.
# 메모리 요구사항 16GB (메모리 사용량 약 7.5GB)
# 1G IMIX 트래픽 처리
# 인터페이스2개(한 개의 세그먼트)에 CPU코어 1개를 자동 할당
/opt/stm/target/c.sh PUT parameters model small


# Model: Small-Medium
# 메모리 요구사항 32GB (메모리 사용량 약 15.5GB)
# 1G Line-rate 성능 처리
# 인터페이스별 코어1개씩 수동할당 
/opt/stm/target/c.sh PUT parameters model medium
/opt/stm/target/c.sh PUT parameters cores_per_interface 1
/opt/stm/target/c.sh PUT parameters dpi_dedicated_core false
/opt/stm/target/c.sh PUT parameters max_interface_rate 2000000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true
echo "model=medium" > /etc/stm/parameters
echo "cores_per_interface=1" >> /etc/stm/parameters
echo "dpi_dedicated_core=false" >> /etc/stm/parameters
cp /etc/stm/parameters /etc/stm.alt/parameters


# Model: Medium  
# 메모리 요구사항 32GB
# 5G IMIX성능 처리
# 인터페이스별 코어2개씩 자동할당 
/opt/stm/target/c.sh PUT parameters model medium
/opt/stm/target/c.sh PUT parameters dpi_dedicated_core false
/opt/stm/target/c.sh PUT parameters max_interface_rate 10000000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true
echo "model=medium" > /etc/stm/parameters
echo "dpi_dedicated_core=false" >> /etc/stm/parameters
cp /etc/stm/parameters /etc/stm.alt/parameters

# Model: Medium-Large
# 메모리 요구사항 64GB
# 10G IMIX성능 처리
# 인터페이스별 코어3개씩 자동할당 
# DPI코어 별도 할당
/opt/stm/target/c.sh PUT parameters model large
/opt/stm/target/c.sh PUT parameters flow_pool_size 1000000
/opt/stm/target/c.sh PUT parameters flow_table_size 1048576
/opt/stm/target/c.sh PUT parameters host_pool_internal_guarantee 500000
/opt/stm/target/c.sh PUT parameters host_pool_size 1000000
/opt/stm/target/c.sh PUT parameters max_app_hosts 500000
/opt/stm/target/c.sh PUT parameters max_app_urls 500000
/opt/stm/target/c.sh PUT parameters max_app_users 1000000
/opt/stm/target/c.sh PUT parameters max_applications 500000
/opt/stm/target/c.sh PUT parameters max_peer_apps 200000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true


# Large모델 기본 파라미터 셋팅시 메모리 사용량 – 초기 부팅시 50.8GB 에서 시작하여 트래픽 로딩시 70기가정도까지 증가, 만일 시스템이 64기가메모리를 탑재하였을경우 메모리 full이 될 가능성이 있음
# 상기 파라미터 적용후 리부팅, 메모리 사용량 체크결과 – 39.6GB 부터 시작. 64기가용량으로도 충분

# Model: Large 
# 메모리 요구사항 96GB
# 10G IMIX성능 처리
# 인터페이스별 코어3개씩 자동 할당 
# DPI코어 별도 할당
/opt/stm/target/c.sh PUT parameters model large
/opt/stm/target/c.sh -c PUT running save_partition both save_config true

# Model: Large-Huge 
# 메모리 요구사항 128GB
# 10G Line-Rate성능 처리
# 인터페이스별 코어4개씩 할당 
/opt/stm/target/c.sh PUT parameters model large
/opt/stm/target/c.sh PUT parameters cores_per_interface 4
/opt/stm/target/c.sh PUT parameters flow_pool_size 10000000
/opt/stm/target/c.sh PUT parameters flow_table_size 10000000
/opt/stm/target/c.sh PUT parameters max_applications 1000000
/opt/stm/target/c.sh PUT parameters max_app_hosts 5000000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true


# 시스템 일반 파라미터 설정 
/opt/stm/target/c.sh -c PUT running system_name stm
/opt/stm/target/c.sh -c PUT running system_banner "Saisei Traffic Manager"
/opt/stm/target/c.sh -c PUT running time_zone Asia/Seoul
/opt/stm/target/c.sh -c PUT running fc_panes 4
/opt/stm/target/c.sh -c PUT running display_mapui false 
/opt/stm/target/c.sh -c PUT running display_global_search false 
/opt/stm/target/c.sh -c PUT running display_dashboard true 
/opt/stm/target/c.sh PUT parameters pcap_max_file_size 1000
/opt/stm/target/c.sh PUT parameters dpi_core_per_bump false
/opt/stm/target/c.sh PUT parameters scanners_dedicate_core_for_high_pri false
/opt/stm/target/c.sh PUT parameters use_remote_application_identification false
/opt/stm/target/c.sh PUT parameters disable_huge_coredump true
/opt/stm/target/c.sh PUT parameters generate_core_on_hang false
/opt/stm/target/c.sh PUT parameters accountee_time_constant 1
/opt/stm/target/c.sh PUT parameters netflow_long_flow_interval 600
/opt/stm/target/c.sh PUT parameters inspect_radius false
/opt/stm/target/c.sh PUT parameters use_reputations false
/opt/stm/target/c.sh PUT parameters enable_threats false
/opt/stm/target/c.sh PUT parameters internal_host_quiet_limit 12:00:00.000
/opt/stm/target/c.sh PUT parameters log_periodic_scheduler false 
/opt/stm/target/c.sh PUT parameters default_shape_interfaces true
/opt/stm/target/c.sh PUT parameters default_shape_policies true
/opt/stm/target/c.sh PUT parameters default_shaper_margin 0
/opt/stm/target/c.sh PUT parameters control_peak_by_shaper true
/opt/stm/target/c.sh PUT parameters shape_using_cir true
/opt/stm/target/c.sh PUT parameters default_file_check_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters geolocation_update_interval 19080:00:00.000
/opt/stm/target/c.sh PUT parameters as_names_update_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters application_update_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters mapui_update_interval 0
/opt/stm/target/c.sh PUT parameters hcp__schema__slow 5m:31d,1h:92d,12h:183d
/opt/stm/target/c.sh PUT parameters hcp__storage_schema 1m:7d,10m:31d,1h:92d,12h:183d
/opt/stm/target/c.sh -c PUT running save_partition both save_config true
cp /etc/stmfiles/files/config/stm/default_config.cfg /home/saisei/default_config_1_parameters_configured.cfg

# Threats 기본 폴리시들 끄기
/opt/stm/target/c.sh PUT threats/DNS-Amplification enabled false
/opt/stm/target/c.sh PUT threats/IPv4-Fragment-Violations enabled false 
/opt/stm/target/c.sh PUT threats/Nonexistent-Destination-Host enabled false
/opt/stm/target/c.sh PUT threats/TCP-Flag-Violations enabled false
/opt/stm/target/c.sh PUT threats/TCP-Sequence-Mismatch enabled false 
/opt/stm/target/c.sh PUT threats/TCP-SYN-DOS enabled false


# 인터페이스 속도 및 방향, 시스템 인터페이스 설정
##라이센스 구매한 인터페이스 속도에 따라 rate값 변경해줄 것, 기본단위는 Kbps, 아래 예제는 1G로 구성, ## /etc/stm/devices.csv파일을 확인하여 위에서logical interface들을(external1, internal1등) 시스템 인터페이스와 연결해줍니다.
/opt/stm/target/c.sh POST interfaces/ name external1 type eth rate 1000000 system_interface stm1
/opt/stm/target/c.sh POST interfaces/ name internal1 type eth rate 1000000 internal true system_interface stm2
/opt/stm/target/c.sh POST interfaces/ name external2 type eth rate 1000000 system_interface stm3
/opt/stm/target/c.sh POST interfaces/ name internal2 type eth rate 1000000 internal true system_interface stm4
/opt/stm/target/c.sh POST interfaces/ name external1.any type vlan outer_interface external1 rate 1000000 
/opt/stm/target/c.sh POST interfaces/ name internal1.any type vlan outer_interface internal1 rate 1000000 internal true
/opt/stm/target/c.sh POST interfaces/ name external2.any type vlan outer_interface external2 rate 1000000  
/opt/stm/target/c.sh POST interfaces/ name internal2.any type vlan outer_interface internal2 rate 1000000 internal true

#10G Link Speed 설정 
#기본 인터페이스 링크스피드는 1G 이다. 10G 인터페이스로 세그먼트를 구성할 경우 10g로 변경이 필요하다
/opt/stm/target/c.sh PUT interfaces/external1 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/external2 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/internal1 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/internal2 link_speed 10g

# 피어 인터페이스 설정 
## 세그먼트를 이루는 인터페이스간 피어링을 설정합니다.
/opt/stm/target/c.sh PUT interfaces/external1 peer internal1
/opt/stm/target/c.sh PUT interfaces/external2 peer internal2
/opt/stm/target/c.sh PUT interfaces/external1.any peer internal1.any
/opt/stm/target/c.sh PUT interfaces/external2.any peer internal2.any

# Physical 인터페이스 Shaper마진 설정 
## 설정된 인터페이스 속도를 초과하지 않기 위해 shaper margin을 기본값 10%에서 0%로 변경해줍니다. 변경하지 않을 경우 인터페이스 속도 500메가로 설정시 실제로는 속도의 10%인 50메가를 더하여 최대 550메가까지 트래픽을 흘려 보내게 됩니다. 
/opt/stm/target/c.sh PUT interfaces/external1 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external2 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal2 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external1.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external2.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal2.any shaper_margin 0


# 인터페이스 활성화
/opt/stm/target/c.sh PUT interfaces/external1 state enabled
/opt/stm/target/c.sh PUT interfaces/internal1 state enabled
/opt/stm/target/c.sh PUT interfaces/external2 state enabled
/opt/stm/target/c.sh PUT interfaces/internal2 state enabled

# VLAN 인터페이스 활성화
/opt/stm/target/c.sh PUT interfaces/external1.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal1.any state enabled
/opt/stm/target/c.sh PUT interfaces/external2.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal2.any state enabled

# Config 저장 및 백업
/opt/stm/target/c.sh -c PUT running save_partition both save_config true
cp /etc/stmfiles/files/config/stm/default_config.cfg /home/saisei/default_config_2_interfaces_created.cfg

# 기본정책 설정
# 일반적인 엔터프라이즈 고객 시스템 설치시 기본 정책 구성.
/opt/stm/target/c.sh POST policies/ name Other-IP-Data percent_mir 90 host_equalisation false shaped true sequence 990

# Application Group 추가 설정
/opt/stm/target/c.sh PUT applications/https groups web
/opt/stm/target/c.sh PUT applications/ipv6-icmp groups networking
/opt/stm/target/c.sh PUT applications/icmpd groups networking
/opt/stm/target/c.sh PUT applications/isis groups networking
/opt/stm/target/c.sh PUT applications/isis-am groups networking
/opt/stm/target/c.sh PUT applications/isis-ambc groups networking
/opt/stm/target/c.sh PUT applications/isis-bcast groups networking
/opt/stm/target/c.sh PUT applications/isis_over_ipv4 groups networking
/opt/stm/target/c.sh PUT applications/multicast-ping groups networking
/opt/stm/target/c.sh PUT applications/pcp-multicast groups networking
/opt/stm/target/c.sh PUT applications/bgp groups networking
/opt/stm/target/c.sh PUT applications/ripng groups networking
/opt/stm/target/c.sh PUT applications/ospf-lite groups networking
/opt/stm/target/c.sh PUT applications/ospf-igp groups networking
/opt/stm/target/c.sh PUT applications/vxlan groups networking

# Windows Updates Control
/opt/stm/target/c.sh POST applications/ name windowsupdate.com server windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name download.microsoft.com server download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name software-download.microsoft.com server software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name delivery.mp.microsoft.com server delivery.mp.microsoft.com  groups updates dynamic false

/opt/stm/target/c.sh PUT applications/windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/delivery.mp.microsoft.com groups updates dynamic false

# Microsoft Applications
/opt/stm/target/c.sh PUT applications/windows-store groups updates dynamic false
/opt/stm/target/c.sh PUT applications/windows-azure groups updates dynamic false
/opt/stm/target/c.sh PUT applications/microsoft-services groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msonline groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msoffice365 groups updates dynamic false

# Other Updates
/opt/stm/target/c.sh POST applications/ name update.ahnlab.com server update.ahnlab.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name alyac.com server alyac.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name update.alyac.co.kr server update.alyac.co.kr  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name chrome-update server gvt1.com  groups updates dynamic false

/opt/stm/target/c.sh PUT applications/update.ahnlab.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/alyac.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/update.alyac.co.kr groups updates dynamic false
/opt/stm/target/c.sh PUT applications/chrome-update groups updates dynamic false


# User listener실행 
# 사용자 대역 등록은 사이트 설치 후 고객에게 내부 IP 대역이 무엇인지 파악하여 정확한 서브넷을 등록해주세요. 0.0.0.0/0을 등록하면 IP spoofing 발생시 시스템 장애의 원인이 됩니다.

# [GUI직접 설정]
# 1)	User Listener구동
/opt/stm/target/c.sh POST userlisteners/ name Default ul_enable_userlistener true ul_persistant true ul_dynamic dynamic ul_use_access_points false ul_use_rate_plans false ul_use_flow_limits false ul_use_rate_limits false

# 2)	고객사 서브넷 추가 (예: 100.10.10.0/24 을 UserGroup Branch1에 할당한다고 가정)
/opt/stm/target/c.sh POST userlisteners/Default/entries/ name 100.10.10.0/24 config_groups Branch1 


# Crontab 설정(기본설정)
# Ubuntu Shell 에서 crontab -e 실행후 아래 내용 추가

15 0 1 * * find /etc/stmfiles/files/cores/* -type f,d -ctime +30 -exec rm -rf {} \; 
@reboot (sleep 5 ; cp -r /home/saisei/deployscripts/report/stm.conf /etc/apache2/sites-available/ > /dev/null 2>&1)
@reboot (sleep 6 ; sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /opt/stm/target/applications-pace2.csv > /dev/null 2>&1)
@reboot (sleep 7 ; sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /etc/stmfiles/files/resources/applications-pace2.csv > /dev/null 2>&1)
@reboot (sleep 8 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3610) \n    " > /opt/stm/target/python/mapui.py)
@reboot (sleep 9 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4020) \n    " > /opt/stm/target/call_home.py)
@reboot (sleep 10 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4430) \n    " > /opt/stm/target/restful_call_home.py)
@reboot (sleep 11 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4840) \n    " > /opt/stm/target/python/auto_upgrade.py)
@reboot (sleep 12 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(5250) \n    " > /opt/stm/target/raid.py)
@reboot (sleep 13 ; sudo rm -f /etc/stm/cli/* > /dev/null 2>&1)
@reboot (sleep 240 ; sudo service apache2 restart > /dev/null 2>&1)
@reboot (sleep 300 ; sudo /etc/stmfiles/files/scripts/thread_monitor_v2.py & > /dev/null 2>&1)
@reboot (sleep 30 ; sudo /etc/stmfiles/files/scripts/bypass_portwell_monitor.py & > /dev/null 2>&1)
@reboot (sleep 60 ;  sudo iptables -I  INPUT -p tcp -m multiport --destination-ports 22,5000 -j ACCEPT  > /dev/null 2>&1) 

# 6.3	삼성 해외망 시스템 구성 전용
# 6.3.1	삼성 해외망 전용 시스템 파라미터
/opt/stm/target/c.sh PUT parameters load_geolocations false
/opt/stm/target/c.sh PUT parameters load_nlri_files false
sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /opt/stm/target/applications-pace2.csv
sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /opt/stm/target.alt/applications-pace2.csv
sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /etc/stm.alt/applications-pace2.csv
sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /etc/stm/applications-pace2.csv
sed -i 's/23.246.0.0\/18|45.57.0.0\/17|64.120.128.0\/17|66.197.128.0\/17|192.173.64.0\/18\, 2906|40027|55095|136292|394406//g' /etc/stmfiles/files/resources/applications-pace2.csv

# 6.3.2	Out-of-band MGMT 인터페이스 이름 변경
# - 아래와 같이 파일이 생성되었는지 내용이 변경되었는지 맥주소를 확인해서 정상적으로 mgmt가 셋팅될 수 있도록 수동 확인 필요
# plz check 70-persistent-net.rules in /etc/udev/rules.d/ like below? 
# SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="08:35:71:09:8d:a7", NAME="mgmt0" 
# SUBSYSTEM=="net", ACTION=="add", DRIVERS=="?*", ATTR{address}=="08:35:71:10:47:7d", NAME="mgmt1"
 
# - 기본적으로 두번째 매니지먼트 포트 MGMT1 에는 192.168.100.25/24 아이피를 셋팅하여 장애시 로컬에서 SSH접속할 수 있도록 준비해둘 것
# 6.3.3	추가확인사항
# Crontab에 bypass script와 thread monitor스크립트가 정상적으로 동록되었는지 확인
# 하드웨어 상태 체크 명령어 설치 확인
# LCD 화면 2줄 확인

# 6.4	GUI 화면 구성
# 6.4.1	GUI 구성 설정 파일 Copy & Paste
# 리붓 완료후 고객이 처음접속시 보게 될 기본 차트화면을 변경해줍니다.
# GUI 변경사항은 /etc/stm.preferences/admin/ 디렉토리에 아래의 파일들로 저장되며, 백업해두었다가 동일한서버 셋업시 덮어쓰기를 하면 GUI셋업에 필요한 시간들을 줄일수 있습니다.

# Optional (방화벽 설정 예제)

sed -i 's/IPV6=yes/IPV6=no/g' /etc/default/ufw
sudo ufw reload

echo "sudo ufw delete deny from any to any" > /etc/stmfiles/files/scripts/ufw-allow-all-for-maintenance.sh
echo "sudo ufw insert 1 allow from any to any" >> /etc/stmfiles/files/scripts/ufw-allow-all-for-maintenance.sh

echo "sudo ufw delete deny from any to any" > /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh
echo "sudo ufw delete allow from any to any" >> /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh
echo "sudo ufw insert 1 allow from 10.0.0.0/8 to any port 5000" >> /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh
echo "sudo ufw insert 2 allow from 172.16.0.0/12 to any port 5000" >> /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh
echo "sudo ufw insert 3 allow from 192.168.0.0/16 to any port 5000" >> /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh
echo "sudo ufw insert 4 deny from any to any" >> /etc/stmfiles/files/scripts/ufw-allow-gui-access-only.sh

chmod 755 /etc/stmfiles/files/scripts/ufw*.sh

/opt/stm/target/c.sh POST scripts/ name ufw-allow-all-for-maintenance.sh 
/opt/stm/target/c.sh POST scripts/ name ufw-allow-gui-access-only.sh

/opt/stm/target/c.sh -c PUT running save_partition both save_config true
cp /etc/stmfiles/files/config/stm/default_config.cfg /home/saisei/default_config_firewall_scripts_added.cfg

### 아래의 명령어 적용시 신규 SSH 세션은 차단되고 GUI 접속만 허용됨
/opt/stm/target/c.sh PUT scripts/ufw-allow-gui-access-only.sh start true


# 6.9.5	기존히스토리 삭제 방법
# 새로운 POC를 시작할 경우 이젠 테스트에서 저장되어 있는 히스토리를 삭제한다
# Mysql8로 DB가 업그레이드 되면서 history collection 에 히스토리 추출속도 개선을 위한 많은 변화가 추가됨. 
# 1)	Mysql login
# mysql -u root -psaisei


# mysql> show databases;
# +--------------------+
# | Database           |
# +--------------------+
# | history            |
# | information_schema |
# | mysql              |
# | performance_schema |
# | sys                |
# +--------------------+
# 5 rows in set (0.00 sec)

# mysql>

# 2)	History database 삭제
# drop database history;
# 3)	History database 새로 생성
# create database history;
# exit
# 4)	History_collector.py 스크립트 재시작
# root@stm:/opt/stm/target# ps -ef | grep his
# root       4005      1  0 15:59 ?        00:00:10 /usr/bin/python2.7 /opt/stm/target/history_collector.py
# root      10414   2349  0 16:56 pts/0    00:00:00 grep --color=auto his
# root@stm:/opt/stm/target# kill -9 4005
# 또는 아래의 한줄명령어 사용

# root@stm:/home/saisei# ps -ef | grep history_collector.py | grep -v grep | awk '{print $2}' | xargs kill -9
# 5)	reboot
