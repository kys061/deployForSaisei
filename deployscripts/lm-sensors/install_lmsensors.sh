#!/bin/bash

mkdir -p /opt/stm/lmsensors_drv
cp /etc/stmfiles/files/scripts/bison-3.4.1.tar.gz /opt/stm/lmsensors_drv/
cp /etc/stmfiles/files/scripts/flex-2.6.3.tar.gz /opt/stm/lmsensors_drv/
cp /etc/stmfiles/files/scripts/lm-sensors-3-5-0.zip /opt/stm/lmsensors_drv/

cd /opt/stm/lmsensors_drv
tar -zxvf bison-3.4.1.tar.gz 
tar -zxvf flex-2.6.3.tar.gz 
unzip lm-sensors-3-5-0.zip  

cd /opt/stm/lmsensors_drv/bison-3.4.1/
./configure
make
make install

cd /opt/stm/lmsensors_drv/flex-2.6.3/
./configure
make
make install

cd /opt/stm/lmsensors_drv/lm-sensors-3-5-0/
make
make install
sensors-detect
cp /opt/stm/lmsensors_drv/lm-sensors-3-5-0/prog/init/lm_sensors.service /lib/systemd/system/
systemctl enable lm_sensors.service
