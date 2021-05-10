#!/usr/bin/python
# -*- coding: utf-8 -*-

'''
    Support Module
    1. 10G Gen2 - 82599ES
    2. 10G Gen3 - X710
    3. 1G COPPER Gen3 - 82580 Gigabit
    4. 1G fiber Gen2 - 82580 Gigabit Fiber
    5. 1G COPPER Gen3 - I210 in J201

    Restrictions
    1. Only tested in COSD304
    2. Recommend bypass_portwell_monitor.sh in j201
    3. Also use this script in j201, just only support 2 segments
'''

from saisei.saisei_api import saisei_api
import subprocess
import time
from logging.handlers import RotatingFileHandler
import logging
import sys
import re
import csv, json
import os
from pprint import pprint 
from time import sleep, time
import pdb

stm_ver=r'7.3'
LOG_FILENAME=r'/var/log/stm_bypass.log'
logger = None

class Segment(object):
    """ Represents a Segment of stm

    Segment class is making attributes of Segment instance
    
    Attributes:
    - name: segment name
    - bypass_state: status of bypass (bypass(2) or inline(0))
    - segment_state: status of interfaces pair
    - link_type: type of interface link
    - pci_width: pci slot width(4x or 8x)
    - driver_type: type of interface driver ( 82599ES, X710, 82580 Gigabit, 82580 Gigabit Fiber )
    """
    def __init__(self):
        self.__name, self.__bypass_state, self.__segment_state = "None", None, None
        self.__ext_name, self.__ext_state, self.__ext_admin_status = None, None, None
        self.__peer_name, self.__peer_state, self.__peer_admin_status = None, None, None
        self.__slot , self.__link_type, self.__pci_address = "None", None, None
        self.__pci_width, self.__driver_type = "None", "None"
        
    def __str__(self):
        return "(segment: {})".format(self.name)

    def log_segment_state(self):
        logger.info("{0:7}{1:13}{2:13}{3:15}{4:15}{5:13}{6:13}{7:15}{8:13}{9:15}{10:13}{11:9}".format(
            self.slot, self.name, self.segment_state, 
            self.ext_name, self.ext_state, self.ext_admin_status,
            self.peer_name, self.peer_state, self.peer_admin_status,
            self.bypass_state, self.link_type, self.pci_width
            ))

    @property
    def name(self):
        return self.__name
    
    @name.setter
    def name(self, val):
        self.__name = "segment" + val

    @property
    def link_type(self):
        return self.__link_type
        
    @link_type.setter
    def link_type(self, val):
        self.__link_type = val
    
    @property
    def pci_address(self):
        return self.__pci_address
        
    @pci_address.setter
    def pci_address(self, val):
        self.__pci_address = val    

    @property
    def bypass_state(self):
        return self.__bypass_state
        
    @bypass_state.setter
    def bypass_state(self, val):
        self.__bypass_state = val
    
    @property
    def segment_state(self):
        return self.__segment_state
    
    @segment_state.setter
    def segment_state(self, val):
        if val:
            self.__segment_state="Activated"
        else:
            self.__segment_state="Inactivated"      

    @property
    def ext_name(self):
        return self.__ext_name
    
    @ext_name.setter
    def ext_name(self, val):
        self.__ext_name = val

    @property
    def ext_state(self):
        return self.__ext_state
    
    @ext_state.setter
    def ext_state(self, val):
        self.__ext_state = val

    @property
    def ext_admin_status(self):
        return self.__ext_admin_status
    
    @ext_admin_status.setter
    def ext_admin_status(self, val):
        self.__ext_admin_status = val                

    @property
    def peer_name(self):
        return self.__peer_name
    
    @peer_name.setter
    def peer_name(self, val):
        self.__peer_name = val
    
    @property
    def peer_state(self):
        return self.__peer_state
    
    @peer_state.setter
    def peer_state(self, val):
        self.__peer_state = val

    @property
    def peer_admin_status(self):
        return self.__peer_admin_status
    
    @peer_admin_status.setter
    def peer_admin_status(self, val):
        self.__peer_admin_status = val

    @property
    def slot(self):
        return self.__slot
        
    @slot.setter
    def slot(self, val):
        self.__slot = val        

    @property
    def pci_width(self):
        return self.__pci_width
        
    @pci_width.setter
    def pci_width(self, val):
        self.__pci_width = val

    @property
    def driver_type(self):
        return self.__driver_type
        
    @driver_type.setter
    def driver_type(self, val):
        self.__driver_type = val



class G(object):
    """ Class for Global Variables
    """
    __instance = None
    _segment = list()
        
    stm_status=False
    stm_thread_status=False

    userid=r'cli_admin'
    passwd=r'cli_admin'
    host=r'localhost'
    port=r'5000'

    rest_basic_path=r'configurations/running/'
    rest_token=r'1'
    rest_order=r'>interface_id'
    rest_start=r'0'
    rest_limit=r'10'
    
    link_type=r'copper'
    model_type=r'small'
    cores_per_interface=0 # small,
    interface_size=0
    segment_size=0
    # copper interfaces count
    copper_count=0
    # gen3 10g count
    gen3_count=0
    # check api is run or not
    is_api=False

    board = 'COSD304'

    fiber_seg_slot_number=[]
    is_same_slot_number=[]
    segment_state=[]

    # for global list of segments
    segments = []

    @classmethod
    def __getInstance(cls):
        return cls.__instance

    @classmethod
    def instance(cls, *args, **kargs):
        cls.__instance = cls(*args, **kargs)
        cls.instance = cls.__getInstance
        return cls.__instance

    @classmethod
    def append_segment_data(cls, data):
        cls._segment.append(data)


class Resturl():
    def __init__(self, saisei_class, select_attrs, with_attrs=[], with_vals=[]):
        if len(with_attrs) >= 2:
            self.select_attrs = ",".join(select_attrs)
            with_patterns = []
            for i, attr in enumerate(with_attrs):
                for j, val in enumerate(with_vals):
                    if i==j:
                        with_pattern = "%s=%s" % (attr, val)
                        with_patterns.append(with_pattern)
            with_patterns = ",".join(with_patterns)
            self.rest_url = '%s%s?token=%s&order=%s&start=%s&limit=%s&select=%s&with=%s' \
            % ( G.rest_basic_path,
                saisei_class,
                G.rest_token,
                G.rest_order,
                G.rest_start,
                G.rest_limit,
                self.select_attrs, with_patterns)
        elif len(with_attrs) == 1:
            self.select_attrs = ",".join(select_attrs)
            with_pattern = None
            for i, attr in enumerate(with_attrs):
                for j, val in enumerate(with_vals):
                    if i==j:
                        with_pattern = "%s=%s" % (attr, val)
            self.rest_url = '%s%s?token=%s&order=%s&start=%s&limit=%s&select=%s&with=%s' \
            % ( G.rest_basic_path, 
                saisei_class, 
                G.rest_token, 
                G.rest_order, 
                G.rest_start, 
                G.rest_limit, 
                self.select_attrs, with_pattern)
        else:
            self.select_attrs = select_attrs
            self.rest_url = '%s%s%s' \
            % ( G.rest_basic_path, 
                saisei_class,  
                self.select_attrs)

    def __str__(self):
        return "RestUrl: %s" % (self.rest_url)

    def get_rest_url(self):
        return self.rest_url


def timer(func):
    def wrapper():
        before = time()
        func()
        logger.info("main() took {} seconds".format(time()- before))
    return wrapper


# make_url = lambda suffix : 'http://%s:%d/%s%s' % (host, port, root, suffix)

def make_logger():
    global logger
    try:
        logger = logging.getLogger('bypass')
        fh = RotatingFileHandler(LOG_FILENAME, 'a', 50 * 1024 * 1024, 4)
        logger.setLevel(logging.INFO)
        formatter = logging.Formatter('%(asctime)s - %(name)s(:%(lineno)s) - %(levelname)s - %(message)s')
        fh.setFormatter(formatter)
        logger.addHandler(fh)
    except Exception as e:
        # print('cannot make logger, please check system, {}'.format(e))
        # sys.exit()
        pass
    else:
        logger.info("***** logger starting %s *****" % (sys.argv[0]))


class Timeout(Exception):
    ''' exception for timeout of subprocess '''


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
            try:
                if t == timeout-1:
                    p_open.kill()
                    raise Timeout
            except Timeout:
                logger.error("timout while running subprocess()")


def logging_line(lines=164):
    logger.info("-"*lines)


def set_segment_slot(segment):
    # segment.pci_address is external's pci address  -> 0000:0{N}:00.0
    try:
        pci_number = segment.pci_address.split(":")[1]
    except Exception as e:
        logger.error("set_segment_slot: {}".format(e))

    if int(pci_number) == 1:
        segment.slot="slot0"
    elif int(pci_number) == 2:
        segment.slot="slot1"
    elif int(pci_number) == 3:
        for _segment in G.segments:
            try:
                if int(_segment.name.replace("segment", "")) == int(segment.name.replace("segment", "")) - 1:
                    if int(_segment.pci_address.split(":")[1]) == int(segment.pci_address.split(":")[1]):
                        segment.slot=_segment.slot
                    else:
                        if int(_segment.pci_width) == 4:
                            segment.slot="slot1"
                        if int(_segment.pci_width) == 8:
                            segment.slot="slot2"                                       
                        # segment.slot="slot{}".format(int(_segment.slot.replace("slot", ""))+1)
                if int(segment.name.replace("segment", "")) - 1 == 0:
                    if _segment.slot == "None":
                        segment.slot="slot1"
            except ValueError as e:
                pass
    elif int(pci_number) == 4:
        segment.slot="slot2"
    elif int(pci_number) == 5:
        segment.slot="slot2"
    elif int(pci_number) == 6:
        segment.slot="slot3"
    elif int(pci_number) == 7:
        segment.slot="slot3"
    else:
        logger.error("pci address is not match.. plz check module of interface..")


def set_pci_width(pci_address, segment):
    pci_address = pci_address.replace("0000:", "")
    cmd = "lspci -vv -s {} |grep LnkCap |egrep 'Width x[0-9]' -o |egrep '[0-9]' -o".format(pci_address)
    _pci_width, _ = subprocess_open(cmd, 10)
    try:
        _pci_width = _pci_width.strip()
    except Exception as e:
        logger.error(e)
    segment.pci_width = _pci_width


def get_driver_type(pci_address):
    # driver = "82580"
    # cmd = 'lspci -m |grep Ether |grep {} |awk -F "\" " \'{print $3}\''.format(pci_address)
    cmd = '/etc/stmfiles/files/scripts/dpdk_nic_bind.py -s |grep {} |cut -d "\'" -f2'.format(pci_address)
    try:
        driver_type, _ = subprocess_open(cmd, 10)
        driver_type = driver_type.strip()
    except Exception as e:
        logger.error(e)

    if "82580 Gigabit Network Connection" in driver_type:
        _driver_type="UTP-GEN3"
    if "82580 Gigabit Fiber Network Connection" in driver_type:
        _driver_type="1G-GEN2"
    if "82599ES" in driver_type:
        _driver_type="10G-GEN2"
    if "X710" in driver_type:
        _driver_type="10G-GEN3"
    if "I210" in driver_type:
        _driver_type="UTP-I210-GEN3"
    
    return _driver_type


def get_link_type(pci_address):
    cmd = r'/etc/stmfiles/files/scripts/dpdk_nic_bind.py -s |grep -B 15 "Network devices using kernel driver" |grep ' + pci_address
    nic_bind, _ = subprocess_open(cmd, 10)
    try:
        nic_bind = nic_bind.strip().split('\n')
    except Exception as e:
        logger.error(e)
        pass
    
    try:
        nic_data=[]
        for nic in nic_bind:
            nic_data.append({
                "pci_address": re.findall(r"0000:[0-9A-Za-z][0-9A-Za-z]:00.[0-9]", nic)[0].replace("0000:", ""),
                "link_type": re.findall(r"\'[0-9A-Za-z +-/]+\'", nic)[0].replace(" ", "")
            })
    except IndexError as e:
        # print("Indexing error for nic_data!!")
        logger.error("Indexing error for nic_data!!")
        return 'None'
        pass
        # sys.exit()
    else:
        for data in nic_data:
            if ('Fiber') in data['link_type']:
                return '1G_fiber'
            elif ('SFP') in data['link_type']:
                return '10G_fiber'
            else:
                return 'copper'


def get_interface_info(saisei_class, select_attrs, with_attrs, with_vals):
    rest_url = Resturl(saisei_class, select_attrs, with_attrs, with_vals)
    try:
        response = api.rest.get(rest_url.get_rest_url())
    except Exception as e:
        logger.error("get_interface_info() : {}".format(e))
        G.is_api = False
        pass
        return False
    else:
        G.is_api = True

    
    return response


def get_peer_interface_info(peer_name):
    rest_url = Resturl("interfaces/", "{}?level=detail&format=human&link=expand&time=utc".format(peer_name))
    try:
        peer_int = api.rest.get(rest_url.get_rest_url())['collection'][0]
    except Exception as e:
        logger.error("get_peer_interface_info() : {}".format(e))
        G.is_api = False
        pass
        return False
    else:
        G.is_api = True
        
    return peer_int



def set_segment_state(segment_number, peer_status, enabled_size, int_thread_state, interface, peer_int, segment):
    _link_type = get_link_type(interface["pci_address"])
    segment.link_type = _link_type
    _driver_type = get_driver_type(interface["pci_address"])
    segment.driver_type=_driver_type
    set_segment_slot(segment)
    set_bypass_state(segment_number, _link_type, segment)
    set_pci_width(interface["pci_address"], segment)
    if peer_status == "up":
        int_thread_state, _ = subprocess_open("sudo /bin/ps -elL |grep {} -o |wc -l".format(interface["name"]), 10)
        if (int(int_thread_state.strip()) > 0):
            segment.segment_state = True
        else:
            segment.segment_state = False
            G.is_api = False
    else:
        segment.segment_state = False
        G.is_api = False


def set_parameter_info(attr="cores_per_interface"):
    parameter_url = Resturl(
    "parameters?",
    "level=full&format=human&link=expand&time=utc")
    try:
        G.cores_per_interface = api.rest.get(parameter_url.get_rest_url())['collection'][0][attr] 
    except Exception as e:
        logger.error("set_parameter_info() : {}".format(e))
        G.is_api = False
        pass
    else:
        G.is_api = True

def get_parameter_info(attr="model"):
    parameter_url = Resturl(
    "parameters?",
    "level=full&format=human&link=expand&time=utc")
    try:
        response = api.rest.get(parameter_url.get_rest_url())['collection'][0][attr]
    except Exception as e:
        logger.error("get_parameter_info() : {}".format(e))
        G.is_api = False
        pass
        return False
    else:
        G.is_api = True        
    return response



def set_segment_obj(seg_num, interface, peer_int, segment):
    segment.name = str(seg_num+1)
    segment.ext_name = interface["name"]
    segment.ext_admin_status = interface["admin_status"]
    segment.ext_state = interface["state"]
    segment.peer_name = peer_int["name"]
    segment.peer_admin_status = peer_int["admin_status"]
    segment.peer_state = peer_int["state"]
    segment.pci_address = interface["pci_address"]


def classify_segment_from_interfaces():
    '''
        classfy interfaces from duplicated in segments
    '''
    saisei_class = "interfaces/"
    select_attrs = ["name", "actual_direction", "state", "admin_status", "pci_address", "interface_id", "type", "peer"]
    with_attrs = ["type"]
    with_vals = ["ethernet"]

    interfaces = get_interface_info(saisei_class, select_attrs, with_attrs, with_vals)

    try:
        seg_lists=[]
        for i, interface in enumerate(interfaces["collection"]):
            int_list=[]
            int_list.append(interface["name"])
            int_list.append(interface["peer"]["link"]["name"])
            seg_lists.append(int_list)

        seg_lists = list(set([tuple(set(seg_list)) for seg_list in seg_lists]))
        
        _int_list = []
        for i, seg in enumerate(seg_lists):
            _int_list.append(sorted(seg)[0])
    except Exception as e:
        logger.info("classify_segment_from_interfaces() : {}".format(e))
        pass
        return False
    else:
        return sorted(_int_list)


def check_segment_state():
    '''
        check all segment
    '''
    saisei_class = "interfaces/"
    select_attrs = ["name", "actual_direction", "state", "admin_status", "pci_address", "interface_id", "type", "peer"]
    with_attrs = ["type"]
    with_vals = ["ethernet"]
    # with_attrs = ["type", "actual_direction"]
    # with_vals = ["ethernet", "external"]

    interfaces = get_interface_info(saisei_class, select_attrs, with_attrs, with_vals)
    classified_interfaces = classify_segment_from_interfaces()
    set_parameter_info(attr="cores_per_interface")
    
    enabled_size = 0
    try:
        for i, interface in enumerate(interfaces["collection"]):
            if interface["name"] in classified_interfaces:
                if (interface["state"] == "enabled" and interface["admin_status"] == "up"):
                    # get thread count of external interface
                    int_thread_state, _ = subprocess_open(r'/bin/ps -elL |grep {} -o |wc -l'.format(interface["name"]), 10)
                    peer_int = get_peer_interface_info(interface["peer"]["link"]["name"])
                    peer_status = peer_int['admin_status']
                    set_segment_obj(i, interface, peer_int, G.segments[i])
                    set_segment_state(i, peer_status, enabled_size, int_thread_state, interface, peer_int, G.segments[i])
                else:
                    logger.error("There is no Segment.")
                    G.is_api = False
    except Exception as e:
        logger.error("check_segment_state() : {}".format(e))
        pass


def compare_segment_pci_adress(segment):
    '''
        for 1G_fiber 4port, it will check which port is bypass0 or not
    '''
    try:
        for _segment in G.segments:
            if int(_segment.pci_address.split(":")[1]) == int(segment.pci_address.split(":")[1]):
                if float(_segment.pci_address.split(":")[2]) != float(segment.pci_address.split(":")[2]):
                    if float(_segment.pci_address.split(":")[2]) > float(segment.pci_address.split(":")[2]):
                        return True
                    else:
                        return False
    except Exception as e:
        logger.error("compare_segment_pci_adress() : {}".format(e))
        pass


def set_gen3_bypass_state(count, segment):
    bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp{}/bypass'.format(count), 10)
    if bypass_state.strip('\n') == "n":
        segment.bypass_state = 'disabled'
    elif bypass_state.strip('\n') == "b":
        segment.bypass_state = 'enabled'
    else:
        segment.bypass_state = 'None'


def get_gen3_count(_pci_address):
    '''
        COSD304 장비는 pci 넘버링이 1-7까지 생성가능
        현재 세그먼트의 pci 넘버링을 매개변수로 받음
        pci 넘버링 숫자만큼 loop를 돌아 모든 gen3타입의 인터페이스의 카운트를 확인하여 bypass pos(위치)를 찾아내고 반환함
    '''
    _total_count = 0
    try:
        for i in range(int(_pci_address)):
            if (i+1) < int(_pci_address):
                cmd = 'lspci |grep "82580 Gigabit Network" |grep 0{}:00 |wc -l'.format(i+1)
                _copper_count, _ = subprocess_open(cmd, 10)
                _copper_count = _copper_count.strip()
                if int(_copper_count) == 0:
                    cmd = 'lspci |grep "X710" |grep 0{}:00 |wc -l'.format(i+1)
                    _count, _ = subprocess_open(cmd, 10)
                    _count = _count.strip()
                    if int(_count) == 0:
                        _total_count += 0
                    else:
                        _total_count += int(_count)
                else:
                    _total_count += int(_copper_count)
    except Exception as e:
        logger.error("get_gen3_count() : {}".format(e))
        pass
        return False
    else:
        return _total_count



def set_bypass_state(seg_number, link_type, segment):
    if link_type == "10G_fiber" and segment.driver_type == "10G-GEN2":
        if 'slot' in segment.slot:
            bypass_state, _ = subprocess_open("sudo cat /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
            if bypass_state.strip('\n') == "0":
                segment.bypass_state = 'disabled'
            elif bypass_state.strip('\n') == "2":
                segment.bypass_state = 'enabled'
            else:
                segment.bypass_state = 'None'
        else:
            segment.bypass_state = 'None'
    elif link_type == "1G_fiber":
        if 'slot' in segment.slot:
            try:
                if int(segment.pci_width) == 4:
                    if compare_segment_pci_adress(segment):
                        bypass_state0, _ = subprocess_open("sudo cat /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
                        if bypass_state0.strip('\n') == "0":
                            segment.bypass_state = 'disabled'
                        elif bypass_state0.strip('\n') == "2":
                            segment.bypass_state = 'enabled'
                        else:
                            segment.bypass_state = 'None'                                
                    else:
                        bypass_state1, _ = subprocess_open("sudo cat /sys/class/misc/caswell_bpgen2/{}/bypass1".format(segment.slot), 10)
                        if bypass_state1.strip('\n') == "0":
                            segment.bypass_state = 'disabled'
                        elif bypass_state1.strip('\n') == "2":
                            segment.bypass_state = 'enabled'
                        else:
                            segment.bypass_state = 'None'
            except ValueError as e:
                pass
        else:
            segment.bypass_state = 'None'
    elif link_type == "10G_fiber" and segment.driver_type == "10G-GEN3":
        try:
            if G.gen3_count > 0:
                if segment.slot == "slot0":                
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_gen3_count = get_gen3_count(_pci_address)
                    set_gen3_bypass_state(_total_gen3_count/2, segment)
                else:
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_gen3_count = get_gen3_count(_pci_address)
                    # print(_total_gen3_count/2)
                    set_gen3_bypass_state(_total_gen3_count/2, segment)

        except Exception as e:
            logger.error(e)
    else:
        try:
            if G.copper_count > 0:
                if "I210" in segment.driver_type:
                    if segment.name == "segment1":
                        bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp0/bypass', 10)
                    if segment.name == "segment2":
                        bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp1/bypass', 10)

                    if segment.name == "segment3":
                        bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp2/bypass', 10)
                    if segment.name == "segment4":
                        bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp3/bypass', 10)

                    if bypass_state.strip('\n') == "n":
                        segment.bypass_state = 'disabled'
                    elif bypass_state.strip('\n') == "b":
                        segment.bypass_state = 'enabled'
                    else:
                        segment.bypass_state = 'None'
                else:
                    if segment.slot == "slot0":
                        if ":00.0" in segment.pci_address:
                            _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                            _total_gen3_count = get_gen3_count(_pci_address)
                            set_gen3_bypass_state(_total_gen3_count/2, segment)
                        if ":00.2" in segment.pci_address:
                            _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                            if int(_pci_address) == 1:
                                _total_gen3_count = get_gen3_count(_pci_address)
                                set_gen3_bypass_state(_total_gen3_count/2 + 1, segment)
                    else:
                        if ":00.0" in segment.pci_address:
                            _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                            _total_gen3_count = get_gen3_count(_pci_address)
                            set_gen3_bypass_state(_total_gen3_count/2, segment)
                        if ":00.2" in segment.pci_address:
                            _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                            _total_gen3_count = get_gen3_count(_pci_address)
                            set_gen3_bypass_state(_total_gen3_count/2 + 1, segment)

                # for i in xrange(G.copper_count):
                #     bypass_state, _ = subprocess_open('cat /sys/class/bypass/g3bp{}/bypass'.format(i), 10)
                #     if bypass_state.strip('\n') == "n":
                #         segment.bypass_state = 'disabled'
                #     elif bypass_state.strip('\n') == "b":
                #         segment.bypass_state = 'enabled'
                #     else:
                #         segment.bypass_state = 'None'
            else:
                logger.info("There is no copper interface..")
        except Exception as e:
            logger.error(e)
            pass


def bypass(segment, action="disable"):
    if segment.link_type == "10G_fiber" and segment.driver_type == "10G-GEN2":
        fiber_module, _ = subprocess_open("sudo lsmod | grep network_bypass | awk '{ print $1 }'", 10)
        i2c_module, _ = subprocess_open("sudo lsmod | grep i2c_i801 | awk '{ print $1 }'", 10)
        if i2c_module is "":
            subprocess_open("sudo modprobe i2c-i801", 10)
            logger.info("insert mod i2c-i801")

        if fiber_module is "":
            subprocess_open("sudo insmod /opt/stm/bypass_drivers/portwell_fiber/driver/network-bypass.ko board={}".format(G.board), 10)
            logger.info("insert mod /opt/stm/bypass_drivers/portwell_fiber/driver/network-bypass.ko")
        # if bypass and thread is alive
        if 'slot' in segment.slot:
            if segment.bypass_state == "enabled" and action == "disable":
                subprocess_open("sudo echo 0 > /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
                subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot0".format(segment.slot), 10)
                subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe0".format(segment.slot), 10)
                logger.info("disable {} fiber bypass0 in {}!".format(segment.name, segment.slot))

            # if normal and thread is dead
            if segment.bypass_state == "disabled" and action == "enable":
                subprocess_open("sudo echo 2 > /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
                subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot0".format(segment.slot), 10)
                subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe0".format(segment.slot), 10)
                logger.info("enable {} fiber bypass0 in {}!".format(segment.name, segment.slot))
        else:
            logger.error("Cannot bypass {}, because there is no slot number in {}".format(action, segment.name))
    elif segment.link_type == "1G_fiber":
        fiber_module, _ = subprocess_open("sudo lsmod | grep network_bypass | awk '{ print $1 }'", 10)
        i2c_module, _ = subprocess_open("sudo lsmod | grep i2c_i801 | awk '{ print $1 }'", 10)
        if i2c_module is "":
            subprocess_open("sudo modprobe i2c-i801", 10)
            logger.info("insert mod i2c-i801")

        if fiber_module is "":
            subprocess_open("sudo insmod /opt/stm/bypass_drivers/portwell_fiber/driver/network-bypass.ko board={}".format(G.board), 10)
            logger.info("insert mod /opt/stm/bypass_drivers/portwell_fiber/driver/network-bypass.ko")  
        if 'slot' in segment.slot:
            if segment.bypass_state == "enabled" and action == "disable":
                try:
                    if int(segment.pci_width) == 4:
                        if compare_segment_pci_adress(segment):
                            subprocess_open("sudo echo 0 > /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot0".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe0".format(segment.slot), 10)
                            logger.info("disable {} fiber bypass0 in {}!".format(segment.name, segment.slot))                              
                        else:
                            subprocess_open("sudo echo 0 > /sys/class/misc/caswell_bpgen2/{}/bypass1".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot1".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe1".format(segment.slot), 10)
                            logger.info("disable {} fiber bypass1 in {}!".format(segment.name, segment.slot)) 
                except Exception as e:
                    pass

            if segment.bypass_state == "disabled" and action == "enable":
                try:
                    if int(segment.pci_width) == 4:
                        if compare_segment_pci_adress(segment):
                            subprocess_open("sudo echo 2 > /sys/class/misc/caswell_bpgen2/{}/bypass0".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot0".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe0".format(segment.slot), 10)
                            logger.info("disable {} fiber bypass0 in {}!".format(segment.name, segment.slot))                              
                        else:
                            subprocess_open("sudo echo 2 > /sys/class/misc/caswell_bpgen2/{}/bypass1".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot1".format(segment.slot), 10)
                            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe1".format(segment.slot), 10)
                            logger.info("disable {} fiber bypass1 in {}!".format(segment.name, segment.slot)) 
                except Exception as e:
                    pass
        else:
            segment.bypass_state = 'None'
            logger.error("Cannot bypass {}, because there is no slot number in {}".format(action, segment.name))
            if segment.bypass_state == "disabled" and action == "enable":
                for i in range(4):
                    if compare_segment_pci_adress(segment):
                        subprocess_open("sudo echo 0 > /sys/class/misc/caswell_bpgen2/{}/bypass0".format(i), 10)
                        subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot0".format(i), 10)
                        subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe0".format(i), 10)
                        logger.info("disable {} fiber bypass0 in {}!".format(segment.name, i))                              
                    else:
                        subprocess_open("sudo echo 0 > /sys/class/misc/caswell_bpgen2/{}/bypass1".format(i), 10)
                        subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/nextboot1".format(i), 10)
                        subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/{}/bpe1".format(i), 10)
                        logger.info("disable {} fiber bypass1 in {}!".format(segment.name, i))             
    elif segment.link_type == "10G_fiber" and segment.driver_type == "10G-GEN3":
            lsmod, _ = subprocess_open("sudo /sbin/lsmod | grep caswell_bpgen3", 10)
            if lsmod is "":
                    subprocess_open("sudo /sbin/insmod /opt/stm/bypass_drivers/portwell_kr/src/driver/caswell-bpgen3.ko", 10)
                    logger.info("insert module caswell-bpgen3.ko for 10G-GEN3")
            if segment.bypass_state == "enabled" and action == "disable":
                if segment.slot == "slot0":
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_count = get_gen3_count(_pci_address)
                    if os.path.isdir("/sys/class/bypass/g3bp{}".format(_total_count/2)):
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_count/2), 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_count/2), 10)
                        logger.info("disable {} g3bp{} 10G-GEN3 bypass!".format(segment.name, _total_count/2))                    
                    # set_gen3_bypass_state(0, segment)
                else:
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_count = get_gen3_count(_pci_address)
                    if os.path.isdir("/sys/class/bypass/g3bp{}".format(_total_count/2)):
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_count/2), 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_count/2), 10)
                        logger.info("disable {} g3bp{} 10G-GEN3 bypass!".format(segment.name, _total_count/2))                            
            if segment.bypass_state == "disabled" and action == "enable":
                if segment.slot == "slot0":
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_count = get_gen3_count(_pci_address)
                    if os.path.isdir("/sys/class/bypass/g3bp{}".format(_total_count/2)):
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_count/2), 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_count/2), 10)
                        logger.info("disable {} g3bp{} 10G-GEN3 bypass!".format(segment.name, _total_count/2))                    
                    # set_gen3_bypass_state(0, segment)
                else:
                    _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                    _total_count = get_gen3_count(_pci_address)
                    if os.path.isdir("/sys/class/bypass/g3bp{}".format(_total_count/2)):
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_count/2), 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_count/2), 10)
                        logger.info("disable {} g3bp{} 10G-GEN3 bypass!".format(segment.name, _total_count/2))                
    else:
        lsmod, _ = subprocess_open("sudo /sbin/lsmod | grep caswell_bpgen3", 10)
        if lsmod is "":
                subprocess_open("sudo /sbin/insmod /opt/stm/bypass_drivers/portwell_kr/src/driver/caswell-bpgen3.ko", 10)
                logger.info("insert module caswell-bpgen3.ko for copper")
        if segment.bypass_state == "enabled" and action == "disable":
            if "I210" in segment.driver_type:
                    if segment.name == "segment1":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp0/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp0/nextboot", 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp0/bypass", 10)
                        logger.info("disable {} g3bp0 copper bypass!".format(segment.name))
                    elif segment.name == "segment2":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp1/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp1/nextboot", 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp1/bypass", 10)
                        logger.info("disable {} g3bp1 copper bypass!".format(segment.name))
                    elif segment.name == "segment3":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp2/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp2/nextboot", 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp2/bypass", 10)
                        logger.info("disable {} g3bp2 copper bypass!".format(segment.name))                        
                    elif segment.name == "segment4":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp3/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp3/nextboot", 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp3/bypass", 10)
                        logger.info("disable {} g3bp3 copper bypass!".format(segment.name))                                                
                    else:
                        logger.info("cannot check segment 3 ~ N in FC1000 !")
            else:
                if segment.slot == "slot0":
                    if ":00.0" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("disable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))

                    if ":00.2" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                        if int(_pci_address) == 1:
                            _total_gen3_count = get_gen3_count(_pci_address)
                            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2 + 1), 10)
                            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2 + 1), 10)
                            subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2 + 1), 10)
                            logger.info("disable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2 + 1))
                            # set_gen3_bypass_state(_total_gen3_count/2 + 1, segment)
                else:
                    if ":00.0" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("disable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))                        
                        # set_gen3_bypass_state(_total_gen3_count/2, segment)
                    if ":00.2" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo n > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("disable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))                         
                        # set_gen3_bypass_state(_total_gen3_count/2 + 1, segment)

        if segment.bypass_state == "disabled" and action == "enable":
            if "I210" in segment.driver_type:
                    if segment.name == "segment1":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp0/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp0/nextboot", 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp0/bypass", 10)
                        logger.info("enable {} g3bp0 copper bypass!".format(segment.name))
                    elif segment.name == "segment2":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp1/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp1/nextboot", 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp1/bypass", 10)
                        logger.info("enable {} g3bp1 copper bypass!".format(segment.name))
                    elif segment.name == "segment3":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp2/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp2/nextboot", 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp2/bypass", 10)
                        logger.info("enable {} g3bp2 copper bypass!".format(segment.name))
                    elif segment.name == "segment4":
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp3/func", 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp3/nextboot", 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp3/bypass", 10)
                        logger.info("enable {} g3bp3 copper bypass!".format(segment.name))                                                
                    else:
                        logger.info("cannot check segment 5 ~ N in FC1000 !")
            else:            
                if segment.slot == "slot0":
                    if ":00.0" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("enable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))

                    if ":00.2" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                        if int(_pci_address) == 1:
                            _total_gen3_count = get_gen3_count(_pci_address)
                            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2 + 1), 10)
                            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2 + 1), 10)
                            subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2 + 1), 10)
                            logger.info("enable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2 + 1))
                            # set_gen3_bypass_state(_total_gen3_count/2 + 1, segment)
                else:
                    if ":00.0" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.0", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("enable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))                        
                        # set_gen3_bypass_state(_total_gen3_count/2, segment)
                    if ":00.2" in segment.pci_address:
                        _pci_address = segment.pci_address.replace("0000:0", "").replace(":00.2", "")
                        _total_gen3_count = get_gen3_count(_pci_address)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(_total_gen3_count/2), 10)
                        subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(_total_gen3_count/2), 10)
                        logger.info("enable {} g3bp{} copper bypass!".format(segment.name, _total_gen3_count/2))            


def bypass_all_interfaces():
    # fiber
    for i in range(4):
        if os.path.isfile("/sys/class/misc/caswell_bpgen2/slot{}/bypass0".format(i)):
            subprocess_open("sudo echo 2 > /sys/class/misc/caswell_bpgen2/slot{}/bypass0".format(i), 1)
            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/slot{}/nextboot0".format(i), 1)
            logger.info("enable fiber bypass0 in slot{}!".format(i))

        if os.path.isfile("/sys/class/misc/caswell_bpgen2/slot{}/bypass1".format(i)):
            subprocess_open("sudo echo 2 > /sys/class/misc/caswell_bpgen2/slot{}/bypass1".format(i), 1)
            subprocess_open("sudo echo 1 > /sys/class/misc/caswell_bpgen2/slot{}/nextboot1".format(i), 1)
            logger.info("enable fiber bypass1 in slot{}!".format(i))

    # copper
    for i in range(8):
        if os.path.isdir("/sys/class/bypass/g3bp{}".format(i)):
            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/func".format(i), 1)
            subprocess_open("sudo echo 1 > /sys/class/bypass/g3bp{}/nextboot".format(i), 1)
            subprocess_open("sudo echo b > /sys/class/bypass/g3bp{}/bypass".format(i), 1)
            logger.info("enable gen3(copper or 10g_gen3) bypass in g3bp{}!".format(i))     


def logging_state():
    logging_line(lines=164)
    logger.info("{0:7}{1:13}{2:13}{3:15}{4:15}{5:13}{6:13}{7:15}{8:13}{9:15}{10:13}{11:9}".format(
    "[slot]", "[seg_name]", "[seg_state]", "[ext_int_name]", "[ext_state]", "[ext_admin]", "[peer]", 
    "[peer_state]", "[peer_admin]", "[bypass_state]", "[link_type]", "[pci_width]"
    ))
    for i in xrange(G.segment_size):
        G.segments[i].log_segment_state()
    logger.info("{0:16}{1:16}".format("[model_type]", "[cores_per_interface]"))
    logger.info("{0:16}{1:16}".format(G.model_type, G.cores_per_interface))
    logging_line(lines=164)


make_logger()


while not G.is_api:
    try:
        api = saisei_api(server=G.host, port=G.port, user=G.userid, password=G.passwd)
    except Exception as e:
        logger.error('api: {}'.format(e))
        pass
    else:
        G.is_api=True
    sleep(2)


# @timer
def main():
    saisei_class = "interfaces/"
    select_attrs = ["name", "actual_direction", "state", "admin_status", "pci_address", "interface_id", "type", "peer"]
    with_attrs = ["type"]
    with_vals = ["ethernet"]
    
    response = get_interface_info(saisei_class, select_attrs, with_attrs, with_vals)
    try:
        G.interface_size = response['size']
        G.interface_size = int(G.interface_size)
        G.segment_size = int(G.interface_size)/2
        G.model_type = get_parameter_info('model')
    except Exception as e:
        logger.error(e)
        G.segment_size = 0
        pass
            
    
    # make segments 
    for i in xrange(G.segment_size):
        G.segments.append(Segment())

    while not G.stm_status:
        response = get_interface_info(saisei_class, select_attrs, with_attrs, with_vals)

        if int(response['size']) > 1:
            G.stm_status = True
        else:
            G.stm_status = False
            logger.error("There is no segment in saisei, plz check.. will sleep 5s..")
        sleep(5)

    while True:
        check_segment_state()
        G.copper_count = 0
        G.gen3_count = 0
        if G.is_api:
            for segment in G.segments:
                if segment.link_type == "copper":
                    G.copper_count += 1
                if segment.driver_type == "10G-GEN3":
                    G.gen3_count += 1
                if segment.segment_state == "Activated":
                    bypass(segment, action="disable")
                else:
                    bypass(segment, action="enable")
            logging_state()
        else:
            logger.error("stm api is not running, run all interface into bypass!")
            # subprocess_open("sudo /etc/stmfiles/files/scripts/enable_bypass_for_monitor.sh", 10)
            bypass_all_interfaces()
        # sleep(2)


if __name__ == "__main__":
    try:        
        main()
    except KeyboardInterrupt:
        logger.info("The script is terminated by interrupt!")
        logger.info("Bye!!")
        sys.exit()