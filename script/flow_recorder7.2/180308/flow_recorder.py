#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
# Copyright (C) 2016 Saisei Networks Inc. All rights reserved.

import sys
from datetime import datetime
import time
from collections import defaultdict
#sys.path.append("/home/saisei/dev/flow_recorder_module")
from flow_recorder_mod import *
from SubnetTree import SubnetTree
################################################################################
# Env for (eg. echo 'show int stm3 flows top 100 by average_rate' | ./stm_cli.py
# admin:admin@localhost)
################################################################################
DD_INTERFACE_LIST = [
    ('external' , 'external1'),
    ('internal' , 'internal1'),
    ('external' , 'external2'),
    ('internal' , 'internal2')
    #    ('external' , 'es1e'),
    #    ('internal' , 'es1i'),
    #    ('external' , 'vs1e.any'),
    #    ('internal' , 'vs1i.any'),
]

# for cmd type is 1 :all users and do log for INCLUDE subnets
INCLUDE = [
'10.10.0.0/16',
'10.24.33.0/24',
'10.27.0.0/16',
'10.60.0.0/16',
'10.160.0.0/16',
'10.161.0.0/16',
'10.162.0.0/16',
'10.163.0.0/16',
'10.164.0.0/16',
'10.165.0.0/16',
'43.227.116.0/22',
'45.125.232.0/22',
'103.194.108.0/22',
'103.243.200.0/24',
'103.243.201.0/24',
'103.243.202.0/24',
'103.243.203.0/24',
'106.249.18.0/23',
'106.249.20.0/22',
'106.249.24.0/21',
'106.249.32.0/23',
'133.186.128.0/17',
'133.186.128.0/24',
'133.186.136.0/24',
'133.186.137.0/24',
'106.249.32.0/24',
'133.186.133.0/24',
'133.186.148.0/24',
'106.249.31.0/25',
'133.186.131.0/24',
'133.186.138.0/24',
'133.186.139.0/24',
'133.186.132.0/24',
'133.186.134.0/24',
'133.186.135.0/24',
'45.249.162.128/25',
'45.249.161.0/25',
'45.249.163.0/25',
'45.249.161.128/25',
'45.249.163.128/25',
'45.249.160.128/25',
'45.249.162.0/25',
'45.249.160.0/25',
'103.218.156.0/25',
'103.218.156.128/25',
'103.218.157.0/25',
'103.218.157.128/25',
'103.218.158.0/25',
'103.218.158.128/25',
'103.218.159.0/25',
'103.218.159.128/25',
]
# for cmd type is 2 : one user by src or dst so host must be in internal iprange
HOST = [
]
TOP_NUM = '5'
ARRIVAL_RATE = '0'
DOMAIN = 'localhost'
USERNAME = 'admin'
PASSWORD = 'FlowCommand#1'
STM_SCRIPT_PATH = r'/opt/stm/target/pcli/stm_cli.py'
# For several interfaces(eg. iterate logging for interfaces(stm01, stm02) sequently)
INTERVAL = 1    # type interval
SCRIPT_INTERVAL = 1    # script interval
# Recording file type selecting(0: only csv, 1: only txt, 2:csv and txt)
RECORD_FILE_TYPE = 0
# Recording cmd type selecting(0: total, 1: all users, 2:one user by src or dst, 3: all of them)
'''
0(flows of total) : extracting all flows in cmd.
1(flows of all users) : extracting all flows and user's ip in INCLUDE.
2(flows of user by src or dst) : extracting user's ip in HOST.
3(all of them) : extracting all explained above
'''
RECORD_CMD_TYPE = 0
# Get current time
CURRENTTIME_INIT = datetime.today().strftime("%Y:%m:%d")
################################################################################

################################################################################
# For parse_fieldname in order to reduce cpu loads.
################################################################################
INTERFACE_LIST = []
#INTERFACE_LIST = list(D_INTERFACE_LIST.values())
#
D_INTERFACE_LIST = defaultdict(list)
for k,v in DD_INTERFACE_LIST:
    D_INTERFACE_LIST[k].append(v)

for i,v in enumerate(D_INTERFACE_LIST.values()):
    for j,val in enumerate(v):
        INTERFACE_LIST.append(val)
#INTERFACE_LIST = [ val for j, val in enumerate(v) for i, v in enumerate(D_INTERFACE_LIST.values()) ]

# RECORD_CMD_TYPE:0 and 1
CMD = []
for i in range(len(INTERFACE_LIST)):
    CMD.append("echo \'show int {} flows top {} by \
average_rate select distress geolocation autonomous_system \
retransmissions round_trip_time timeouts udp_jitter red_threshold_discards in_control rtt_server rtt_client\' |sudo \
{} {}:{}@{}".format(INTERFACE_LIST[i],
                    TOP_NUM, STM_SCRIPT_PATH, USERNAME, PASSWORD, DOMAIN))
# RECORD_CMD_TYPE:2
CMD_BY_SOURCEHOST = []
for host in HOST:
    for intf in INTERFACE_LIST:
        for d_intf in D_INTERFACE_LIST['internal']:
            if intf == d_intf:
                CMD_BY_SOURCEHOST.append("echo \'show interface {} flows with source_host={} \
arrival_rate > {} top {} by average_rate select geolocation \
autonomous_system  retransmissions round_trip_time timeouts' |sudo {} \
{}:{}@{}".format(intf, host, ARRIVAL_RATE, TOP_NUM, STM_SCRIPT_PATH,
                    USERNAME, PASSWORD, DOMAIN))
# RECORD_CMD_TYPE:2
CMD_BY_DESTHOST = []
for host in HOST:
    for intf in INTERFACE_LIST:
        for d_intf in D_INTERFACE_LIST['external']:
            if intf == d_intf:
                CMD_BY_DESTHOST.append("echo \'show interface {} flows with dest_host={} \
arrival_rate > {} top {} by average_rate select geolocation \
autonomous_system  retransmissions round_trip_time timeouts' |sudo {} \
{}:{}@{}".format(intf, host, ARRIVAL_RATE, TOP_NUM, STM_SCRIPT_PATH,
                    USERNAME, PASSWORD, DOMAIN))
################################################################################

################################################################################
def main():
    while True:
        try:
            monlog_size = get_logsize()
#            if monlog_size > LOGSIZE or monlog_size < 1000:
#                logrotate(SCRIPT_MON_LOG_FILE, monlog_size)
#                init_logger()
            current_time = datetime.today().strftime("%Y:%m:%d")
            foldername = parsedate(current_time)
            #logfolderpath = r'/var/log/flows/users/' + foldername[0] + foldername[1] + '/' + foldername[0] + foldername[1] + foldername[2] + r'-'
            logfolderpath = r'/var/log/flows/users/' + foldername[0] + foldername[1] + '/'  # redmine #2
            # total or all users
            if RECORD_CMD_TYPE == 0 or RECORD_CMD_TYPE == 1:
                for i in range(len(INTERFACE_LIST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, i)   # Get list of filepaths[txtpath, csvpath]
                    fr = Flowrecorder(CMD[i], D_INTERFACE_LIST, foldername,
                                            file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
                    time.sleep(INTERVAL)
            # one user
            elif RECORD_CMD_TYPE == 2:
                for j in range(len(CMD_BY_SOURCEHOST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, j%len(INTERFACE_LIST))
                    fr = Flowrecorder(CMD_BY_SOURCEHOST[j], D_INTERFACE_LIST, foldername,
                                        file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
                for l in range(len(CMD_BY_DESTHOST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, l%len(INTERFACE_LIST))
                    fr = Flowrecorder(CMD_BY_DESTHOST[l], D_INTERFACE_LIST, foldername,
                                        file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
            # all of them
            elif RECORD_CMD_TYPE == 3:
                # total or all users
                for i in range(len(INTERFACE_LIST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, i)   # Get list of filepaths[txtpath, csvpath]
                    fr = Flowrecorder(CMD[i], D_INTERFACE_LIST, foldername,
                                            file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
                    time.sleep(INTERVAL)
                # one user
                for j in range(len(CMD_BY_SOURCEHOST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, j%len(INTERFACE_LIST))
                    fr = Flowrecorder(CMD_BY_SOURCEHOST[j], D_INTERFACE_LIST, foldername,
                                        file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
                for l in range(len(CMD_BY_DESTHOST)):
                    file_paths = get_filepaths(foldername, INTERFACE_LIST, TOP_NUM, l%len(INTERFACE_LIST))
                    fr = Flowrecorder(CMD_BY_DESTHOST[l], D_INTERFACE_LIST, foldername,
                                        file_paths, logfolderpath, include_subnet_tree)
                    fr.start(RECORD_FILE_TYPE, RECORD_CMD_TYPE)
            else:
                pass
            time.sleep(SCRIPT_INTERVAL)
#            init_logger()
        except KeyboardInterrupt:
            print ("\r\nThe script is terminated by user interrupt!")
            print ("Bye!!")
            sys.exit()
################################################################################
foldername_init = parsedate(CURRENTTIME_INIT)
create_folder(foldername_init)
# make subnet tree for INCLUDE
try:
    include_subnet_tree = SubnetTree()
    for subnet in INCLUDE:
        include_subnet_tree[subnet] = str(subnet)
except Exception as e:
    pass
################################################################################
if __name__ == "__main__":
    main()
