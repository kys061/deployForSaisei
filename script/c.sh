#!/usr/bin/python
#
# Simple utility for generating STM Rest requests
#
# Normal usage is:
#
# ./c.sh verb url [optional arguments]
#
# where the URL is just the part that comes after the configuration name, e.g.
#
# ./c.sh GET applications/
# ./c.sh GET 'applications/?with=total_rate>10&order=<total_rate&limit=10'
# ./c.sh PUT interfaces/stm1 rate 10000
#
# Note that single quotes are needed around a URL that includes characters that the
# shell uses, such as '|' and '&'
#
# The following options can appear BEFORE the verb only:
#
# -a: host address (defaults to localhost)
# -c: URL starts with configuration name, e.g. running/applications/
# -m: URL is replaced by the name of a class, get its metadata
# -p: password
# -r: raw URL, starting with rest/, e.g. rest/mystm/configurations/running/applications/
# -u: username
# -v: verbose, output entire HTTP response
#
#
# Before you ask... this file is called c.sh even though it is written in Python
# because the original version was a bash script, and it is called c.sh in a zillion
# places
#
import argparse
import subprocess
import os
import re
import sys
import json

site_prefix = "SITE_"
subsite_prefix = "_SUB"
serverzone_prefix = "SVR"
sys_prefix = "_SYS"
svc_prefix = "_SVC"


acl_cmd = ""
source_ip_lists = [
"172.16.31.0/24",
"172.16.32.0/24",
"172.16.33.0/24",
"172.16.34.0/24",
"172.16.35.0/24",
"172.16.36.0/24",
"172.16.37.0/24",
"172.16.38.0/24",
"172.16.39.0/24",
"172.16.40.0/24",
"172.16.41.0/24",
"172.16.42.0/24",
"172.16.43.0/24",
"172.16.44.0/24",
"172.16.45.0/24",
"172.16.46.0/24",
"172.16.47.0/24",
"172.16.48.0/24",
"172.16.49.0/24",
"172.16.50.0/24",
"172.16.51.0/24",
"172.16.52.0/24",
"172.16.53.0/24",
"172.16.54.0/24",
"172.16.55.0/24",
"172.16.56.0/24",
"172.16.57.0/24",
"172.16.58.0/24",
"172.16.59.0/24",
"172.16.60.0/24",
"172.16.61.0/24",
"172.16.62.0/24",
"172.16.63.0/24",
"172.16.64.0/24",
"172.16.65.0/24",
"172.16.66.0/24",
"172.16.67.0/24",
"172.16.68.0/24",
"172.16.69.0/24",
"172.16.70.0/24",
"172.16.71.0/24",
"172.16.72.0/24",
"172.16.73.0/24",
"172.16.74.0/24",
"172.16.75.0/24",
"172.16.76.0/24",
"172.16.77.0/24",
"172.16.78.0/24",
"172.16.79.0/24",
"172.16.80.0/24",

]

#make 50 site acl
site_name = []
subsite_name = []
def make_acl():
	for i in range(1, 51):
		_site_name = site_prefix+str(i)
		site_name.append(_site_name)
		print("/opt/stm/target/c.sh POST acls/ name {}".format(_site_name)) 
		for j, ip in enumerate(source_ip_lists, 1):
			if j == i:
				_ip = ip.replace('/', '_')
				print("/opt/stm/target/c.sh POST acls/{}/entries/ name {} source_subnet {}".format(_site_name, _ip, ip))
		_site_name_temp = _site_name
		for k in range(1, 3):
			_site_name += subsite_prefix+str(k)
			subsite_name.append(_site_name)
			print("/opt/stm/target/c.sh POST acls/ name {}".format(_site_name))
			for l, ip in enumerate(source_ip_lists, 1):
				if l == i:
					if k == 1:
						__ip = ip.replace('/24', '_25')
						___ip = ip.replace('/24', '/25')
						print("/opt/stm/target/c.sh POST acls/{}/entries/ name {} source_subnet {}".format(_site_name, __ip, ___ip))
					if k == 2:
						__ip = ip.replace('0/24', '128_25')
						___ip = ip.replace('0/24', '128/25')
						print("/opt/stm/target/c.sh POST acls/{}/entries/ name {} source_subnet {}".format(_site_name, __ip, ___ip))
#						print("/opt/stm/target/c.sh DELETE acls/{}/entries/{}".format(_site_name, __ip))
			_site_name = _site_name_temp



apps = []
def make_app():
	for i in range(1, 51):
		for j in range(1, 3):
			print("/opt/stm/target/c.sh POST applications/ name {} priority 30000".format(serverzone_prefix + str(j) + "-APP"+str(i)))
			apps.append()




app_groups_1 = []
app_groups_2 = []
app_groups_3 = []
def make_app_groups():
	for i in range(1, 3):
		print("/opt/stm/target/c.sh POST app_groups/ name {}".format(serverzone_prefix + str(i)))
		app_groups_1.append(serverzone_prefix + str(i))
		for j in range(1, 5):
			print("/opt/stm/target/c.sh POST app_groups/ name {}".format(serverzone_prefix + str(i)+"_SYS"+str(j)))
			app_groups_2.append(serverzone_prefix + str(i)+"_SYS"+str(j))
			for k in range(1, 4):
				print("/opt/stm/target/c.sh POST app_groups/ name {}".format(serverzone_prefix + str(i)+"_SYS"+str(j)+"_SVC"+str(k)))
				app_groups_3.append(serverzone_prefix + str(i)+"_SYS"+str(j)+"_SVC"+str(k))



def put_app_ingroups():
	for i in range(1, 51):
		for j in range(1, 3):
			for k in range(1, 5):
				for l in range(1, 4):
					if i % 5 == 1:
						print("/opt/stm/target/c.sh PUT applications/{} groups {} dynamic false".format(serverzone_prefix + str(j) + "_APP"+str(i), serverzone_prefix + str(j) +","+ serverzone_prefix + str(j)+"_SYS"+str(k) +","+ serverzone_prefix + str(j)+"_SYS"+str(k)+"_SVC"+str(l)))


subsvrsite_name = []
sub_svr_sys_site_name = []
def make_policy(lv=1):
	if lv == 1:
		for i in range(1, 51):
			_site_name = site_prefix+str(i)
			print("/opt/stm/target/c.sh POST policies/ name {} acl {} sequence 1000 upstream_mir 100000 downstream_mir 100000".format(_site_name, _site_name))
	elif lv == 2:
		for _subsite in subsite_name:
#			print(_subsite)
			__site = re.sub('_SUB[0-9]+', '', _subsite)
			print("/opt/stm/target/c.sh POST policies/ name {} acl {} parent {} sequence 910 upstream_mir 60000 downstream_mir 60000 burst_threshold 0 shaped true shaper_margin 0".format(_subsite, _subsite, __site))
#			_site_name = site_prefix+str(i)
#			_site_name += subsite_prefix+str(j)
	elif lv == 3:
		for _subsite in subsite_name:
			for i in range(1,3):
		#		print("/opt/stm/target/c.sh POST policies/ name {} acl {} groups {} parent {} sequence 900 upstream_mir 30000 downstream_mir 30000 burst_threshold 0 shaped true shaper_margin 0".format(_subsite+"_SVR"+str(i), _subsite, "SVR"+str(i), _subsite))
				subsvrsite_name.append(_subsite+"_SVR"+str(i))


#	elif lv == 4:
		for _subsvrsite in subsvrsite_name:
			__subsite = re.sub('_SVR[0-9]+', '', _subsvrsite)
#			print(_subsvrsite)
			for i in range(1, 5):
				_sub_svr_sys_site = _subsvrsite+"_SYS"+str(i)
				sub_svr_sys_site_name.append(_sub_svr_sys_site)
				_groups = re.sub('SITE_[0-9]+_SUB[1-2]_', '', _sub_svr_sys_site)
#				print("/opt/stm/target/c.sh POST policies/ name {} acl {} groups {} parent {} sequence 800 upstream_mir 10000 downstream_mir 10000 burst_threshold 0 shaped true shaper_margin 0".format(_subsvrsite+"_SYS"+str(i), __subsite , _groups, __subsite))

		for _sub_svr_sys_site in sub_svr_sys_site_name:
			__subsite = re.sub('_SVR[0-9]+_SYS[0-9]', '', _sub_svr_sys_site)
			for i in range(1, 4):
				__sub_svr_sys_svc_site = _sub_svr_sys_site+"_SVC"+str(i)
				_groups = re.sub('SITE_[0-9]+_SUB[1-2]_', '', __sub_svr_sys_svc_site)
				print("/opt/stm/target/c.sh POST policies/ name {} acl {} groups {} parent {} sequence 700 upstream_cir 2000 downstream_cir 2000 burst_threshold 0 shaped true shaper_margin 0".format(_sub_svr_sys_site+"_SVC"+str(i), __subsite , _groups, _sub_svr_sys_site))
				

make_acl()
#make_app()
#make_app_groups()
#put_app_ingroups()
make_policy(lv=3)
