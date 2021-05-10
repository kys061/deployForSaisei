#!/bin/bash

# Delete default policy
sed -i -e 2,10d /opt/stm/target.alt/scripts/default_policies.csv
echo "IntelligentQoS,,,990,90,,,,,host_equalisation=false;shaped=true" >> /opt/stm/target.alt/scripts/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stm.alt/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stm/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /etc/stmfiles/files/scripts/default_policies.csv
cp /opt/stm/target.alt/scripts/default_policies.csv /opt/stm/target/scripts/default_policies.csv
touch /etc/stm.alt/parameters
cp /dev/null /etc/stm.alt/parameters
echo load_default_policies=false >> /etc/stm.alt/parameters
cp /etc/stm.alt/parameters /etc/stm/parameters

# Initialisation script not used
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3610) \n    " > /opt/stm/target.alt/python/mapui.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4020) \n    " > /opt/stm/target.alt/call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4430) \n    " > /opt/stm/target.alt/restful_call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4840) \n    " > /opt/stm/target.alt/python/auto_upgrade.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(5250) \n    " > /opt/stm/target.alt/raid.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(3610) \n    " > /opt/stm/target/python/mapui.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4020) \n    " > /opt/stm/target/call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4430) \n    " > /opt/stm/target/restful_call_home.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(4840) \n    " > /opt/stm/target/python/auto_upgrade.py
printf "#"'!'"/usr/bin/python2.7 \n\nimport time \nwhile True: \n  time.sleep(5250) \n    " > /opt/stm/target/raid.py

# add history timestamp
echo 'export HISTTIMEFORMAT="%F %T "' >> /home/saisei/.bashrc
echo 'export HISTTIMEFORMAT="%F %T "' >> /root/.bashrc
