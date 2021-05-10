#!/bin/bash
#####################################
# Last Date : 2021.4.12              #
# Writer : yskang(kys061@gmail.com) #
#####################################

if [ ! -z $1 ]; then
    depolyip=$1
else
    echo "Please check usage"
    echo "usage: ./build {dst_ip_address}"
    exit 0
fi 

version=7.3.1
date=$(date "+%Y%m%d")
password=FlowCommand#1

cd /home/saisei/dev/deploy/

tar -zcvf /home/saisei/dev/deploy/build/${date}-${version}-deployscripts.tgz deployscripts/
scp /home/saisei/dev/deploy/build/${date}-${version}-deployscripts.tgz saisei@${depolyip}:/home/saisei
~/.google-drive-upload/bin/gupload /home/saisei/dev/deploy/build/${date}-${version}-deployscripts.tgz