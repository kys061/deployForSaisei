#!/bin/bash
#####################################
# Last Date : 2019.09.25            #
# Writer : yskang(kys061@gmail.com) #
#####################################


geofilename="/etc/stm/GeoIPCountryWhois.csv"
logindefs="/etc/login.defs"


function deleteGeolocation()
{
  mv ${geofilename} ${geofilename}.bak
}

function changeLogindefs()
{
  # change 99999 to 31 if line that starts with PASS_MAX_DAYS character exist in logindefs
  sed -i '/^PASS_MAX_DAYS/s/99999/31/g' $logindefs
}

# to do: need to change line number
function deleteFileManger()
{
  sed -i '656s/<li class="tab6"><a href="#tab6-6">File Manager<\/a><\/li>/ <!-- <li class="tab6"><a href="#tab6-6">File Manager<\/a><\/li> -->/g' /opt/stm/target.alt/files/index.html
}

function changeSudoers()
{
  sudo chmod 755 /etc/sudoers
  sed -i '/^%sudo/s/%sudo\sALL=NOPASSWD:\sALL/#%sudo ALL=NOPASSWD: ALL\n%sudo ALL= ALL/g' /etc/sudoers
  sudo chmod 444 /etc/sudoers
}

function setTimeout()
{
  echo "TMOUT=180" >> /root/.profile
  echo "export TMOUT" >> /root/.proflie
  echo "TMOUT=180" >> /home/saisei/.profile
  echo "export TMOUT" >> /home/saisei/.proflie
}

function setAccountLock()
{
  sed -i '1iauth required pam_tally2.so onerr=fail even_deny_root deny=3 unlock_time=120' /etc/pam.d/common-auth
}

deleteGeolocation
deleteFileManger
changeLogindefs
changeSudoers
setTimeout
setAccountLock
