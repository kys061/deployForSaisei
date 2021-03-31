#4.5	DEFAULT 정책 삭제 및 INTELLIGENTQOS 정책 생성
#STM 초기실행시  default-polices.csv 파일에 있는 내용들을 불러들여 기본정책을 생성합니다. 기본정책에 포함된 항목들이 한국고객들의 네트워크 환경과 맞지 않은 경우가 많아 해당파일의 내용을 초기화 후 IntelligentQoS이름의 정책을 추가해줍니다.
# root@stm:/home/saisei# 아래 명령어들을 우분투 shell 상에서 그대로 copy & paste해넣으면 됨
sed -i -e 2,10d /opt/stm/target.alt/scripts/default_policies.csv
echo "IntelligentQoS,,,990,90,,,,,host_equalisation=true" >> /opt/stm/target.alt/scripts/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stm.alt/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stm/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stmfiles/files/scripts/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /opt/stm/target/scripts/default_policies.csv
touch /etc/stm.alt/parameters
cp /dev/null /etc/stm.alt/parameters
echo load_default_policies=false >> /etc/stm.alt/parameters
cp /etc/stm.alt/parameters /etc/stm/parameters

# 4.6	불필요한 스크립트 초기화
# 아래의 나열된 스크립트의 내용들을 초기화해주면 CPU코어자원을 30%가량 절감가능
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3600) \n    " > /opt/stm/target.alt/python/mapui.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4000) \n    " > /opt/stm/target.alt/call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4400) \n    " > /opt/stm/target.alt/restful_call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4800) \n    " > /opt/stm/target.alt/set_firewall.py

cp /opt/stm/target.alt/python/mapui.py /opt/stm/target/python/mapui.py
cp /opt/stm/target.alt/call_home.py /opt/stm/target/call_home.py
cp /opt/stm/target.alt/restful_call_home.py /opt/stm/target/restful_call_home.py
cp /opt/stm/target.alt/set_firewall.py /opt/stm/target/set_firewall.py

# 4.7	[옵션]리눅스 인터페이스명 고정
# stm이 초기 실행되어 올라오면 리눅스 인터페이스명을 우분투가 생성한 이름이 아니라 BIOS에서 주어지는 이름으로 변경하게 되는데 이를 막아야 될 경우 아래의 파일을 /etc/stm/ , /etc/stm.alt/ 디렉토리에 생성해주면 된다.
touch /etc/stm/nobiosdevname
touch /etc/stm.alt/nobiosdevname

# 우분투 OS상에서 아래의 명령어로 BIOS가 부여하는 인터페이스명을 먼저 확인할수도 있다.
# http://manpages.ubuntu.com/manpages/bionic/man1/biosdevname.1.html
# root@stm:/home/saisei# biosdevname -i eno1
# em1
# 4.8	리붓
# 설치완료후 리붓
# root@stm:/home/saisei/stm-install-18.04-7.3.1-11217# reboot
# 5	SAISEI FLOWCOMMAND 셋업
# Config Wizard를 이용하여 기본 지정된 단순 파라미터 설정값으로 셋팅할 수도 있고, 고객별로 상세 설정이 필요할경우 수동으로 셋업할수 있습니다. 한국고객의 경우 세부설정이 필요한 경우가 많아 무조건 개별수동설정방법을 이용하여 셋업하도록 합니다.
# 수동 셋업방법은 REST API 명령어 / CLI / GUI 3가지의 방법으로 가능하며 아래 수동 설정을 통한 셋업은 시간단축을 위해 REST API명령어를 사용합니다. (고객에게 사용방법을 설명할 경우 무조건 GUI를 이용한 사용법을 교육해야 합니다.) 
# 5.1	[옵션] CONFIG WIZARD를 이용한 설정
# STM 설치후 리붓, 최초GUI접속시 자동실행되는 config wizard를 따라 설정을 완료. LAG없는 single or dual bump구성만 가능. 
# Config Wizard를 이용한 구성방법은 1세그먼트 단순 구성에 최적화되어 있는 관계로 2세그먼트 이상 또는 복잡한 파라미터 설정시 반드시 수동으로 설정해야 합니다.

# [Config Wizard 접속]
# GUI기본접속방법과 동일하게 5000번 포트를 이용하여 접속하면 되며, 추가적로 ‘?wizard=brief’ 옵션을 뒤에 붙일경우 기본패스워드 강제변경 스텝을 건너띄기할수 있습니다. 
# http://192.168.100.136:5000/?wizard=brief

# 5.2	REST API명령어를 이용한 수동 설정
# 다양한 옵션을 파라미터 수정으로 자유롭게 설정할 수 있고, LAG인터페이스 및 2개이상의 Bump 설정해야 할 경우 반드시 수동으로 설정해야 합니다.

# 5.2.1	모델 설정
# 설치 후 리붓하면 FlowCommand 는 최소 처리 요구성능에 맞춰져(model=small, 세그먼트별 최대 1G) 파라미터 값들이 자동 설정되고 시작됩니다. 만일 10G급 성능에 맞춰 설정할 경우 제일 먼저 소프트웨어 처리 모델을 medium 또는 large모델로 변경해줘야 합니다. 
# •	Medium 모델의 플로우 처리용량은 100만 플로우이며 인터페이스포트별 2개의 CPU코어를 할당하고 DPI를 위한 CPU코어는 share하고, 트래픽 처리용량은 세그먼트별 Full-duplex기준 5Gbps입니다.
# •	Large모델은 200만 또는 그 이상(메모리 용량에 따라 추가 설정)의 플로우를 처리하며, 인터페이스당 3개의 코어를 할당하고, , 트래픽 처리용량은 Full-duplex기준 10Gbps입니다.

# [Ubuntu Shell상에서 아래 명령어 copy&paste]
# Medium모델로 설정시
/opt/stm/target/c.sh PUT parameters model medium

# Large 모델로 설정시
/opt/stm/target/c.sh PUT parameters model large
 
# 5.2.2	시스템 일반 파라미터 설정 
/opt/stm/target/c.sh -c PUT running system_name stm
/opt/stm/target/c.sh -c PUT running system_banner "Saisei Flow Command"
/opt/stm/target/c.sh -c PUT running time_zone Asia/Seoul
/opt/stm/target/c.sh -c PUT running fc_panes 4
/opt/stm/target/c.sh -c PUT running display_mapui false 
/opt/stm/target/c.sh PUT parameters pcap_max_file_size 1000
/opt/stm/target/c.sh PUT parameters dpi_core_per_bump false
/opt/stm/target/c.sh PUT parameters scanners_dedicate_core_for_high_pri false
/opt/stm/target/c.sh PUT parameters use_remote_application_identification false
/opt/stm/target/c.sh PUT parameters disable_huge_coredump true
/opt/stm/target/c.sh PUT parameters generate_core_on_hang false
/opt/stm/target/c.sh PUT parameters netflow_long_flow_interval 600
/opt/stm/target/c.sh PUT parameters inspect_radius false
/opt/stm/target/c.sh PUT parameters use_reputations false
/opt/stm/target/c.sh PUT parameters enable_threats false
/opt/stm/target/c.sh PUT parameters internal_host_quiet_limit 720:00:00.000
/opt/stm/target/c.sh PUT parameters log_periodic_scheduler false 

# threats 기본 폴리시들 disable
/opt/stm/target/c.sh PUT threats/DNS-Amplification enabled false
/opt/stm/target/c.sh PUT threats/IPv4-Fragment-Violations enabled false 
/opt/stm/target/c.sh PUT threats/Nonexistent-Destination-Host enabled false
/opt/stm/target/c.sh PUT threats/TCP-Flag-Violations enabled false
/opt/stm/target/c.sh PUT threats/TCP-Sequence-Mismatch enabled false 
/opt/stm/target/c.sh PUT threats/TCP-SYN-DOS enabled false

# [선택옵션-smtp서버설정]
/opt/stm/target/c.sh -c PUT running smtp_address "reports@saisei.com"
/opt/stm/target/c.sh -c PUT running smtp_password Report1ng
/opt/stm/target/c.sh -c PUT running smtp_server smtp.gmail.com:587

# 5.2.3	10G IMIX Rate지원 (64GB DRAM)

# Large 모델이지만 공공기관고객 같이 트래픽이 많지않은 노드의 메모리 절감을 위해 medium모델 정도의 셋팅으로 변경해줄 경우 아래의 파라미터 추가
/opt/stm/target/c.sh PUT parameters flow_pool_size 1000000
/opt/stm/target/c.sh PUT parameters flow_table_size 1048576
/opt/stm/target/c.sh PUT parameters host_pool_size 1000000
/opt/stm/target/c.sh PUT parameters host_pool_internal_guarantee 500000
/opt/stm/target/c.sh PUT parameters max_applications 200000
/opt/stm/target/c.sh PUT parameters max_peer_apps 200000
/opt/stm/target/c.sh PUT parameters max_geo_users 600000
/opt/stm/target/c.sh PUT parameters max_app_hosts 600000
/opt/stm/target/c.sh PUT parameters max_app_urls 600000
/opt/stm/target/c.sh PUT parameters max_app_users 600000

# Large모델 기본파라미터 셋팅시 메모리 사용량 – 첫부팅후 50.8GB 에서 시작 , 트래픽이 흐르면 어플리케이션 수집 및 각종 통계정보를 수집하기에 64기가메모리일경우 full이 될 가능성이 있음
# 상기 파라미터 적용후 리부팅, 메모리 사용량 체크결과 – 39.6GB 부터 시작.

# 5.2.4	10G Line Rate지원 (128GB DRAM)

#10G Line rate 지원되어야하고, 1000만플로우 이상의 트래픽처리가 요구될경우 대용량 트래픽 처리를 지원할수 있도록 인터페이스별 코어할당수, flow table, flow pool 수 변경 (메모리 128GB요구됨) 
/opt/stm/target/c.sh PUT parameters cores_per_interface 4
/opt/stm/target/c.sh PUT parameters flow_pool_size 10000000
/opt/stm/target/c.sh PUT parameters flow_table_size 10000000
/opt/stm/target/c.sh -c PUT running save_partition both save_config true

# 5.2.5	Display pane 수 설정 
# /opt/stm/target/c.sh -c PUT running fc_panes 4
# 기본 5로 설정되어 있으나 4로 셋팅할경우 2번째 display pane이 closed된 상태로 로딩됨
# 1로 셋팅할경우 모든 pane이 closed된 상태로 로딩됨
# GUI에서 수정할경우 Expert모드에서 running, 오른쪽 마우스 클릭, modify선택하여 수정가능

# 5.2.6	세그먼트 생성 인터페이스 확인
# /etc/stm/devices.csv 파일을 확인하면 모든 가용한 인터페이스들을 확인할수 있다. 여기서 어떠한 인터페이스를 사용할것인지 확인후 ‘stmOO” 로 시작하는 인터페이스명을 확인해야한다.
# root@stm:/home/saisei# more /etc/stm/devices.csv 
# "linux_name","stm_name","pci_address","driver_path","ip_address","mac_address","flags","vendor","device","driver_id"
# "enp13s0","stm0","0000:0d:00.0","/sys/bus/pci/drivers/igb","123.141.93.150/28","08:35:71:09:8d:a7",4163,32902,5427,0
# "enp14s0","stm1","0000:0e:00.0","/sys/bus/pci/drivers/igb","192.168.1.100/24","08:35:71:10:47:7d",4099,32902,5433,6
# "enp1s0f0","stm2","0000:01:00.0","/sys/bus/pci/drivers/igb","::/0","08:35:71:11:32:77",4099,32902,5391,6
# "enp1s0f1","stm3","0000:01:00.1","/sys/bus/pci/drivers/igb","::/0","08:35:71:11:32:78",4099,32902,5391,6
# "enp1s0f2","stm4","0000:01:00.2","/sys/bus/pci/drivers/igb","::/0","08:35:71:11:32:79",4099,32902,5391,6
# "enp1s0f3","stm5","0000:01:00.3","/sys/bus/pci/drivers/igb","::/0","08:35:71:11:32:7a",4099,32902,5391,6
# "ens1f0","stm6","0000:03:00.0","/sys/bus/pci/drivers/igb","::/0","08:35:71:f8:ca:f6",4099,32902,5390,6
# "ens1f1","stm7","0000:03:00.1","/sys/bus/pci/drivers/igb","::/0","08:35:71:f8:ca:f7",4099,32902,5390,6
# "ens1f2","stm8","0000:03:00.2","/sys/bus/pci/drivers/igb","::/0","08:35:71:f8:ca:f8",4099,32902,5390,6
# "ens1f3","stm9","0000:03:00.3","/sys/bus/pci/drivers/igb","::/0","08:35:71:f8:ca:f9",4099,32902,5390,6
# "ens3f0","stm10","0000:05:00.0","/sys/bus/pci/drivers/ixgbe","::/0","08:35:71:11:48:13",4099,32902,4347,3
# "ens3f1","stm11","0000:05:00.1","/sys/bus/pci/drivers/ixgbe","::/0","08:35:71:11:48:14",4099,32902,4347,3
# root@stm:/home/saisei#

# 5.2.7	인터페이스 생성
# 상기 스텝에서 외부인터페이스와/내부인터페이스로 사용할 포트가 STM8, STM9, STM10, STM11로 확인되었으면 아래 스크립트에서 시스템 인터페이스 부분을 수정해주고, 쉘에서 Copy&Paste해넣어주면 되겠습니다.

# 5.2.7.1	인터페이스 속도 및 방향, 시스템 인터페이스 설정
##인터페이스 속도에 따라 rate값 변경해줄 것, 기본단위는 Kbps, 아래 예제는 1G로 구성, ## /etc/stm/devices.csv파일을 확인하여 위에서logical interface들을(external1, internal1등) 시스템 인터페이스와 연결해줍니다.
/opt/stm/target/c.sh POST interfaces/ name external1 type eth rate 1000000 system_interface stm8
/opt/stm/target/c.sh POST interfaces/ name internal1 type eth rate 1000000 internal true system_interface stm9
/opt/stm/target/c.sh POST interfaces/ name external2 type eth rate 1000000 system_interface stm10
/opt/stm/target/c.sh POST interfaces/ name internal2 type eth rate 1000000 internal true system_interface stm11
/opt/stm/target/c.sh POST interfaces/ name external1.any type vlan outer_interface external1 rate 1000000 
/opt/stm/target/c.sh POST interfaces/ name internal1.any type vlan outer_interface internal1 rate 1000000 internal true
/opt/stm/target/c.sh POST interfaces/ name external2.any type vlan outer_interface external2 rate 1000000  
/opt/stm/target/c.sh POST interfaces/ name internal2.any type vlan outer_interface internal2 rate 1000000 internal true
/opt/stm/target/c.sh PUT interfaces/external1 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/external2 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/internal1 link_speed 10g
/opt/stm/target/c.sh PUT interfaces/internal2 link_speed 10g

# 5.2.7.2	피어 인터페이스 설정 
## 세그먼트를 이루는 인터페이스간 피어링을 설정합니다.
/opt/stm/target/c.sh PUT interfaces/external1 peer internal1
/opt/stm/target/c.sh PUT interfaces/external2 peer internal2
/opt/stm/target/c.sh PUT interfaces/external1.any peer internal1.any
/opt/stm/target/c.sh PUT interfaces/external2.any peer internal2.any

# 5.2.7.3	Physical 인터페이스 Shaper마진 설정 
## 설정된 인터페이스 속도를 초과하지 않기 위해 shaper margin을 기본값 10%에서 0%로 변경해줍니다. 변경하지 않을 경우 인터페이스 속도 500메가로 설정시 실제로는 속도의 10%인 50메가를 더하여 최대 550메가까지 트래픽을 흘려보내게 됩니다. 
/opt/stm/target/c.sh PUT interfaces/external1 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external2 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal2 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external1.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/external2.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal2.any shaper_margin 0

# 상기와 같이 Shaper Margin을 조정하면 아래 캡쳐화면과 같이 Interface Transmit Rate이 아주 smooth한 곡선을 그리게 되며, 설정된 인터페이스 속도에서 정확히 Shaping이 이루어집니다.
 


# 5.2.7.4	[옵션] LAG 인터페이스 생성
## 2개이상의 Segment를 Link Aggregation Interface 로 묶을경우 필요하며 여러개 세그먼트를 하나의 logical interface에서 모니터링 및 제어가 가능하게 됩니다.
/opt/stm/target/c.sh POST interfaces/ name external type lag 
/opt/stm/target/c.sh POST interfaces/ name internal type lag internal true
/opt/stm/target/c.sh POST interfaces/ name external.any type lag rate 2000000
/opt/stm/target/c.sh POST interfaces/ name internal.any type lag rate 2000000 internal true

# 5.2.7.5	[옵션] LAG 인터페이스 피어 설정
/opt/stm/target/c.sh PUT interfaces/external peer internal
/opt/stm/target/c.sh PUT interfaces/external.any peer internal.any

# 5.2.7.6	[옵션] Physical 인터페이스를 LAG 인터페이스와 연동 
/opt/stm/target/c.sh PUT interfaces/internal1 lag_interface internal
/opt/stm/target/c.sh PUT interfaces/external2 lag_interface external 
/opt/stm/target/c.sh PUT interfaces/internal2 lag_interface internal
/opt/stm/target/c.sh PUT interfaces/external1.any lag_interface external.any
/opt/stm/target/c.sh PUT interfaces/internal1.any lag_interface internal.any 
/opt/stm/target/c.sh PUT interfaces/external2.any lag_interface external.any
/opt/stm/target/c.sh PUT interfaces/internal2.any lag_interface internal.any

# 5.2.7.7	인터페이스 활성화
/opt/stm/target/c.sh PUT interfaces/external1 state enabled
/opt/stm/target/c.sh PUT interfaces/internal1 state enabled
/opt/stm/target/c.sh PUT interfaces/external2 state enabled
/opt/stm/target/c.sh PUT interfaces/internal2 state enabled

# 5.2.7.8	VLAN 인터페이스 활성화
/opt/stm/target/c.sh PUT interfaces/external1.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal1.any state enabled
/opt/stm/target/c.sh PUT interfaces/external2.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal2.any state enabled

# 5.2.7.9	[옵션] LAG 인터페이스 활성화
/opt/stm/target/c.sh PUT interfaces/external state enabled
/opt/stm/target/c.sh PUT interfaces/internal state enabled
/opt/stm/target/c.sh PUT interfaces/external.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal.any state enabled

# 5.2.8	기본정책 설정
/opt/stm/target/c.sh POST policies/ name IntelligentQoS percent_mir 90 host_equalisation true sequence 990

# 5.2.9	Application Group 추가 설정
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

# 5.2.9.1	Windows Updates Control
/opt/stm/target/c.sh POST applications/ name windowsupdate.com server windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name download.microsoft.com server download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name software-download.microsoft.com server software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name delivery.mp.microsoft.com server delivery.mp.microsoft.com  groups updates dynamic false

/opt/stm/target/c.sh PUT applications/windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/delivery.mp.microsoft.com groups updates dynamic false

# 5.2.9.2	Microsoft Applications
/opt/stm/target/c.sh PUT applications/windows-store groups updates dynamic false
/opt/stm/target/c.sh PUT applications/windows-azure groups updates dynamic false
/opt/stm/target/c.sh PUT applications/microsoft-services groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msonline groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msoffice365 groups updates dynamic false

# 5.2.9.3	Other Updates
/opt/stm/target/c.sh POST applications/ name update.ahnlab.com server update.ahnlab.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name alyac.com server alyac.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name update.alyac.co.kr server update.alyac.co.kr  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name chrome-update server gvt1.com  groups updates dynamic false

/opt/stm/target/c.sh PUT applications/update.ahnlab.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/alyac.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/update.alyac.co.kr groups updates dynamic false
/opt/stm/target/c.sh PUT applications/chrome-update groups updates dynamic false

# 5.2.10	History파라미터 설정
# 1000사용자 수집이 필요할경우에만 설정
# [초기설치시 기본셋팅]
# 최대 200명의 사용자에 대해 app, geo 를 수집하고 사용자별 200개항목씩 수집
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_geolocation__limit 10
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_application__limit 10
/opt/stm/target/c.sh PUT parameters hcc__user__limit 200

# [기본 200유저 히스토리 저장 되는 파라미터를 1000유저 history collection 으로 변경]
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_geolocation__limit 5
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_application__limit 5
/opt/stm/target/c.sh PUT parameters hcc__user__limit 1000


# 5.2.11	User listener실행 
# 내부 네트워크 구성에따라 user_listener_config.py 수정, user-group분류하여 그룹별 모니터링 가능하게 해줘야함
cp /etc/stmfiles/files/scripts/user_listener_config_template.py /etc/stmfiles/files/scripts/user_listener_config.py
vi /etc/stmfiles/files/scripts/user_listener_config.py
# 1)	사용자 등록할 IP대역 수정. default값은 internal interface에서 올라오는 모든 IP를 Users에 등록함
 

# 2)	DNS 이름을 이용한 사용자명 등록 끄기. default값은 해당 IP가 dns에서 인식될경우 return받은 이름을 그대로 사용하여 User로 등록. 두 라인을 #처리하여 기능 끄기
 

# 3)	사용자 그룹 등록. 예제와 같이 그룹으로 분류할 서버넷과 이름을 추가 
 
# 
# 4)	스크립트 실행
/opt/stm/target/c.sh POST scripts/ name user_listener.py run_on_boot true
/opt/stm/target/c.sh PUT scripts/user_listener.py start true
/opt/stm/target/c.sh -c PUT running save_partition both save_config true

# 5.2.12	추가 계정생성
# monitor_only 계정 필요시 생성

# 5.2.13	바이패스/ThreadMonitor/한글리포트 스크립트 설치
#     1. deployscripts.tgz 파일을 장비에 복사 (설치파일은 brian@saisei.com으로 문의)
#     2. deployscripts.tgz 파일을 압축 풀기
#     3. deployconfig.txt 파일 수정      
#         - 장비 모델 설정  - fc2000 or fc4000 or fc8000
#             #bypass_config 
#             #model:['fc2000', 'fc4000']
#             model:fc4000 // 하드웨어 모델 설정
#         - 세그먼트 별 하드웨어 슬롯 위치 설정 
#              segment1:slot0 // 1번슬롯일 경우 slot0, 4번째 슬롯일 경우 slot3
#              segment2:slot0
#              segment3:slot1
#              segment4:slot1
#         - 한글 리포트 적용을 위한 컨피그 설정
#             #report_config
#             id:admin // 웹접속 아이디
#             password:FlowCommand#1 // 웹접속 패스워드
#             management_ip:1.1.1.1 // 고객사 관리 아이피
#     4.  스크립트 실행 
# 상기 deployconfig.txt수정완료후 설치스크립트 실행
# cd /home/saisei/deployscripts/ && ./deployscripts.sh

# 5.2.14	설정저장 및 config 백업
/opt/stm/target/c.sh -c PUT running save_partition both save_config true
cp /etc/stmfiles/files/config/stm/default_config.cfg /home/saisei/default_config.cfg_allset.bak

# 5.2.15	사후지원용 2nd Management 인터페이스 생성
# em2에 192.168.1.100/32 심어둘것 

# 5.2.16	크론설정

# 15 0 1 * * find /etc/stmfiles/files/cores/* -type f,d -ctime +7 -exec rm -rf {} \; 

# @reboot (sleep 10 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3600) \n    " > /opt/stm/target/python/mapui.py)
# @reboot (sleep 12 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4000) \n    " > /opt/stm/target/call_home.py)
# @reboot (sleep 14 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4400) \n    " > /opt/stm/target/restful_call_home.py)
# @reboot (sleep 16 ; printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4800) \n    " > /opt/stm/target/set_firewall.py)

# @reboot (sleep 30 ; sudo /etc/stmfiles/files/scripts/bypass_portwell_monitor.sh & > /dev/null 2>&1)
# @reboot (sleep 60 ;  sudo iptables -I  INPUT -p tcp -m multiport --destination-ports 22,5000 -j ACCEPT  > /dev/null 2>&1)
# @reboot (sleep 180 ; cp -r /opt/stm/target/files/report/stm.conf /etc/apache2/sites-available/ > /dev/null 2>&1)
# @reboot (sleep 200 ; sudo service apache2 restart > /dev/null 2>&1)
# @reboot (sleep 240 ; sudo /etc/stmfiles/files/scripts/thread_monitor.py & > /dev/null 2>&1)
