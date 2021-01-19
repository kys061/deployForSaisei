#set_small_2seg_config.sh
/opt/stm/target/c.sh PUT parameters model medium
/opt/stm/target/c.sh PUT parameters dpi_dedicated_core false

/opt/stm/target/c.sh -c PUT running system_banner "Saisei Flow Command"
/opt/stm/target/c.sh -c PUT running time_zone Asia/Seoul
/opt/stm/target/c.sh -c PUT running fc_panes 4
/opt/stm/target/c.sh -c PUT running display_mapui false 
/opt/stm/target/c.sh -c PUT running display_global_search false 
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
/opt/stm/target/c.sh PUT parameters shaper_adjust_min 0.97
/opt/stm/target/c.sh PUT parameters default_file_check_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters geolocation_update_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters as_names_update_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters application_update_interval 00:00:00.000
/opt/stm/target/c.sh PUT parameters mapui_update_interval 0
/opt/stm/target/c.sh PUT threats/DNS-Amplification enabled false
/opt/stm/target/c.sh PUT threats/IPv4-Fragment-Violations enabled false 
/opt/stm/target/c.sh PUT threats/Nonexistent-Destination-Host enabled false
/opt/stm/target/c.sh PUT threats/TCP-Flag-Violations enabled false
/opt/stm/target/c.sh PUT threats/TCP-Sequence-Mismatch enabled false 
/opt/stm/target/c.sh PUT threats/TCP-SYN-DOS enabled false
/opt/stm/target/c.sh PUT parameters hcc__application__limit 1000
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_geolocation__limit 5
/opt/stm/target/c.sh PUT parameters  hcc__sd_user_application__limit 5
/opt/stm/target/c.sh PUT parameters hcc__user__limit 1000

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

/opt/stm/target/c.sh POST applications/ name windowsupdate.com server windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name download.microsoft.com server download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name software-download.microsoft.com server software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name delivery.mp.microsoft.com server delivery.mp.microsoft.com  groups updates dynamic false
 /opt/stm/target/c.sh PUT applications/windowsupdate.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/software-download.microsoft.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/delivery.mp.microsoft.com groups updates dynamic false

/opt/stm/target/c.sh PUT applications/windows-store groups updates dynamic false
/opt/stm/target/c.sh PUT applications/windows-azure groups updates dynamic false
/opt/stm/target/c.sh PUT applications/microsoft-services groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msonline groups updates dynamic false
/opt/stm/target/c.sh PUT applications/msoffice365 groups updates dynamic false

/opt/stm/target/c.sh POST applications/ name update.ahnlab.com server update.ahnlab.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name alyac.com server alyac.com  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name update.alyac.co.kr server update.alyac.co.kr  groups updates dynamic false
/opt/stm/target/c.sh POST applications/ name chrome-update server gvt1.com  groups updates dynamic false

/opt/stm/target/c.sh PUT applications/update.ahnlab.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/alyac.com groups updates dynamic false
/opt/stm/target/c.sh PUT applications/update.alyac.co.kr groups updates dynamic false
/opt/stm/target/c.sh PUT applications/chrome-update groups updates dynamic false

# 2seg setting
/opt/stm/target/c.sh POST interfaces/ name external1 type eth rate 1000000 system_interface stm1
/opt/stm/target/c.sh POST interfaces/ name internal1 type eth rate 1000000 internal true system_interface stm2
/opt/stm/target/c.sh POST interfaces/ name external1.any type vlan outer_interface external1 rate 1000000 
/opt/stm/target/c.sh POST interfaces/ name internal1.any type vlan outer_interface internal1 rate 1000000 internal true


/opt/stm/target/c.sh PUT interfaces/external1 peer internal1
/opt/stm/target/c.sh PUT interfaces/external1.any peer internal1.any


/opt/stm/target/c.sh PUT interfaces/external1 shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1 shaper_margin 0

/opt/stm/target/c.sh PUT interfaces/external1.any shaper_margin 0
/opt/stm/target/c.sh PUT interfaces/internal1.any shaper_margin 0


/opt/stm/target/c.sh PUT interfaces/external1 state enabled
/opt/stm/target/c.sh PUT interfaces/internal1 state enabled


/opt/stm/target/c.sh PUT interfaces/external1.any state enabled
/opt/stm/target/c.sh PUT interfaces/internal1.any state enabled



/opt/stm/target/c.sh POST policies/ name IntelligentQoS percent_mir 90 host_equalisation true shaped true sequence 990