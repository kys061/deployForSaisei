#!/bin/bash
#####################################
# Last Date : 2021.4.8            #
# Writer : yskang(kys061@gmail.com) #
#####################################
depolyip=10.11.0.190
date=$(date "+%Y%m%d")
password=FlowCommand#1

tar -zcvf deployscripts${date}.tgz deployscripts/
scp deployscripts${date}.tgz saisei@${depolyip}:/home/saisei
# expect <<EOF 
# expect "saisei@10.11.0.190's password: " 
# send "FlowCommand#1\n";
# expect eof
# EOF
