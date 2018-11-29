#!/bin/bash
#####################################
# Copyright (c) 2017 Saise          #
# Last Date : 2017.07.21            #
# Writer : yskang(kys061@gmail.com) #
#####################################

equal_count=0
bypass_cooper_driver=caswell_drv_bypass-gen3-V1.13.0.zip
bypass_fiber_driver=caswell_drv_network-bypass-V3.20.0.zip
#bypass_deploy_path=/home/saisei/deploy/script/bypass/
bypass_deploy_path=/home/saisei/deploy/script/bypass7_2/bypass_monitor/
recorder_deploy_path=/home/saisei/deploy/script/flow_recorder7.2/
bypass_target=/opt/stm/target/
bypass7_2_target=/etc/stmfiles/files/scripts/
recorder_target=/etc/stmfiles/files/scripts/
config_path=/home/saisei/deploy/config/
user_listener_config_filename=/etc/stmfiles/files/scripts/user_listener_config.py
recorder_filename=/etc/stmfiles/files/scripts/flow_recorder.py

red='\033[0;31m'
green="\e[0;32m"
NC='\033[0m'
sleep_time=2
# copy bypass to target
cp ${bypass_deploy_path}${bypass_cooper_driver} ${bypass_target}
cp ${bypass_deploy_path}${bypass_cooper_driver} ${bypass7_2_target}
if [ $? -eq 0 ]; then
    echo -e "${bypass_deploy_path}${bypass_cooper_driver} is coping in ${bypass_target}"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${bypass_deploy_path}${bypass_fiber_driver} ${bypass_target}
cp ${bypass_deploy_path}${bypass_fiber_driver} ${bypass7_2_target}
if [ $? -eq 0 ]; then
    echo "# ${bypass_deploy_path}${bypass_fiber_driver} is coping in ${bypass_target}"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${bypass_deploy_path}bypass_portwell_install.sh ${bypass_target}bypass_portwell_install.sh
cp ${bypass_deploy_path}bypass_portwell_install.sh ${bypass7_2_target}bypass_portwell_install.sh
if [ $? -eq 0 ]; then
    echo "# ${bypass_deploy_path}bypass_portwell_install.sh is coping in ${bypass_target}bypass_portwell_install.sh"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${bypass_deploy_path}bypass_portwell_monitor.sh ${bypass7_2_target}bypass_portwell_monitor.sh
if [ $? -eq 0 ]; then
    echo "# ${bypass_deploy_path}bypass_portwell_monitor.sh is coping in ${bypass7_2_target}bypass_portwell_monitor.sh"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${bypass_deploy_path}enable_bypass.sh ${bypass_target}enable_bypass.sh
if [ $? -eq 0 ]; then
    echo "# ${bypass_deploy_path}enable_bypass.sh is coping in ${bypass_target}enable_bypass.sh"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi

# copy flow_recorder_mod to /etc/stmfiles/files/script 
cp ${recorder_deploy_path}flow_recorder_mod.py ${recorder_target}
if [ $? -eq 0 ]; then
    echo "# ${recorder_deploy_path}flow_recorder_mod.py is coping in ${recorder_target}"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${recorder_deploy_path}flow_recorder.py ${recorder_target}
if [ $? -eq 0 ]; then
    echo "# ${recorder_deploy_path}flow_recorder.py is coping in ${recorder_target}"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi
cp ${recorder_deploy_path}flow_recorder_monitor.py ${recorder_target}
if [ $? -eq 0 ]; then
    echo "# ${recorder_deploy_path}flow_recorder_monitor.py is coping in ${recorder_target}"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi

# make user listener config
if [ -e /etc/stmfiles/files/scripts/user_listener_config_template.py ]; then
    cp /etc/stmfiles/files/scripts/user_listener_config_template.py $user_listener_config_filename
    sleep $sleep_time
    echo -e "making user listener config...${red}...OK!${NC}"
else
    echo '#### there is no /etc/stmfiles/files/scripts/user_listener_config_template.py'
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
				echo "# Adding ${internals[$i]} in $user_listener_config_filename"
    				sleep $sleep_time
    				echo -e "${red}...OK!${NC}"
			fi
        done
    else
        echo "#### there is no $user_listener_config_filename"
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
		echo "# Adding ${internals[$i]} in $recorder_filename"
		    sleep $sleep_time
		    echo -e "${red}...OK!${NC}"
	    fi
        done
    else
        echo "#### there is no $recorder_filename"
    fi
else
    echo "#### there is no INTERNAL"
fi

# install bypass driver 
${bypass_target}bypass_portwell_install.sh

# run on start
#if [ -e ${recorder_target}flow_recorder.py ]; then
#/opt/stm/target/c.sh PUT scripts/flow_recorder.py run_on_boot true >/dev/null 2>&1
#fi
#if [ $? -eq 0 ]; then
#    echo "# flow_recorder.py is set as run on boot"
#fi
#if [ -e ${recorder_target}flow_recorder_monitor.py ]; then
#/opt/stm/target/c.sh PUT scripts/flow_recorder_monitor.py run_on_boot true >/dev/null 2>&1
#fi
#if [ $? -eq 0 ]; then
#    echo "# flow_recorder_monitor.py is set as run on boot"
#fi

# cooper config
if [ "$1" == "cooper" ]; then
${config_path}bitwsetup-portwell-lag-2seg-copper.sh
fi

# fiber config
if [ "$1" == "fiber" ]; then
${config_path}bitwsetup-portwell-lag-2seg-fiber.sh
fi

# add monitor users
/opt/stm/target/c.sh -r POST /rest/stm/configurations/running/administrators name monitor_only password monitor_only enabled true >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "# monitor account was created!"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi

# save config
/opt/stm/target/c.sh -r PUT /rest/top/configurations/running save_partition both save_config true >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "# save configs well!"
    sleep $sleep_time
    echo -e "${red}...OK!${NC}"
fi

