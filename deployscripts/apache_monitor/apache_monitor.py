#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
# Write by yskang(kys061@gmail.com)

import subprocess
import time
from logging.handlers import RotatingFileHandler
import logging
import sys
import re
from time import sleep
import pdb


stm_ver = r'7.3'
stm_id = r'cli_admin'
stm_pass = r'cli_admin'
stm_host = r'localhost'
stm_port = r'5000'
stm_script_path = r'/opt/stm/target/pcli/stm_cli.py'
stm_flow_path = 'configurations/running/flows/'
stm_interface_path = r'configurations/running/interfaces/'
LOG_FILENAME = r'/var/log/apache_monitoring.log'

is_stm_started = False
stm_chk_interval = 30
logger = None
err_lists = ['Cannot connect to server', 'does not exist', 'no matching objects', 'waiting for server']

##########

def make_logger():
    global logger
    try:
        logger = logging.getLogger('saisei.thread_monitor')
        fh = RotatingFileHandler(LOG_FILENAME, 'a', 50 * 1024 * 1024, 4)
        logger.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        fh.setFormatter(formatter)
        logger.addHandler(fh)
    except Exception as e:
        print('cannot make logger, please check system, {}'.format(e))
    else:
        logger.info("***** logger starting %s *****" % (sys.argv[0]))


def logging_line():
    logger.info("=================================")


def get_command(cmd, param=''):
    # logger.info('echo \'{0} \' |sudo {1} {2}:{3}@{4} {5}'.format(cmd, stm_script_path, stm_id, stm_pass, stm_host, param))
    return 'echo \'{0} \' |sudo {1} {2}:{3}@{4} {5}'\
        .format(cmd, stm_script_path, stm_id, stm_pass, stm_host, param)


def get_pid(name):
    try:
        subprocess.check_output("sudo ps -elL |grep %s" % name, shell=True)
    except subprocess.CalledProcessError:
        return False
    else:
        return True


def subprocess_open(command, timeout):
    try:
        p_open = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    except Exception as e:
        logger.error("subprocess_open() cannot be executed, {}".format(e))
        pass
    else:
        for t in xrange(timeout):
            sleep(1)
            if p_open.poll() is not None:
                (stdout_data, stderr_data) = p_open.communicate()
                return stdout_data, stderr_data
            if t == timeout-1:
                p_open.kill()
                return [False]


def reboot_system():
    try:
        logger.info("Try system restarting")
        subprocess_open('sudo reboot', 10)
    except Exception as e:
        logger.error("reboot system() cannot be executed, {}".format(e))
        pass


def restart_apache():
    try:
        logger.info("Try APACHE restarting")
        subprocess_open('sudo service apache2 restart', 10)
    except Exception as e:
        logger.error("restart_apache() : {}".format(e))
        pass


def check_subprocess_data(sub):
    result = sub
    if result is '':
        logger.info("stm return blank data")
        return False
    elif result is False:
        logger.info("stm return Timeout Error")
        return False
    elif result in err_lists:
        logger.info("stm return {}".format(err_lists))
        return False
    else:
        return True


def check_stm_status():
    global is_stm_err, is_stm_started
    if check_subprocess_data(subprocess_open(get_command(r"show parameter model"), 10)[0]):
        is_stm_started = True
        logger.info("STM is running...")
    else:
        is_stm_started = False
        logger.info("STM is NOT running...")


def main():
    global is_stm_started
    stm_alive_count = 0

    while True:
        check_stm_status()
        if is_stm_started:
            logger.info("apache is running...")
            logger.info("sleep 300s...")
            stm_alive_count = 0
            sleep(300)
        else:
            stm_alive_count += 1
            logger.info("apache is NOT running... alive count : {}".format(stm_alive_count))
            logger.info("sleep 60s...")
            sleep(60)
        
        if stm_alive_count >= 4:
            logger.info("count is {}".format(stm_alive_count))
            logger.info("Restart apache now")
            restart_apache()
            stm_alive_count = 0

        logging_line()
        


make_logger()
# version = get_stm_version()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        logger.info("The script is terminated by interrupt!")
        print("\r\nThe script is terminated by user interrupt!")
        print("Bye!!")
        sys.exit()

