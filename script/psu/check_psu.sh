#####################################
# Last Date : 2020.3. 28            #
# Writer : yskang(kys061@gmail.com) #
#####################################
#
# Must be run as root.
#


while true; do
    psu0=$(/home/saisei/cas_pmb_ctrl -v |grep -A 2 "Module 0" |grep Output |awk -F: '{print  $2}' |awk -F" " '{print  $1}')
    psu1=$(/home/saisei/cas_pmb_ctrl -v |grep -A 2 "Module 1" |grep Output |awk -F: '{print  $2}' |awk -F" " '{print  $1}')

    if [ $psu0 == "0.000" ]; then
        killall -9 bypass_portwell_monitor.sh >>/var/log/stm_bypass.log 2>&1
        cd /sys/class/misc/caswell_bpgen2/slot0/
        echo 2 > bypass0
        echo 1 > bpe0
        echo 1 > nextboot0
    elif [ $psu1 == "0.000" ]; then
        killall -9 bypass_portwell_monitor.sh >>/var/log/stm_bypass.log 2>&1
        cd /sys/class/misc/caswell_bpgen2/slot0/
        echo 2 > bypass0
        echo 1 > bpe0
        echo 1 > nextboot0
    else
        echo "psu is safe" >>/var/log/stm_bypass.log 2>&1
    fi
done