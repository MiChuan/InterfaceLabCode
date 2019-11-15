# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import time
import hashlib
import hmac
import random
import json

import serial
import serial.tools.list_ports
import datetime
import threading

# 串口句柄
g_serial = None
# 串口已打开
g_port_is_open = False

# 接收线程运行标志位
g_rec_run = False

# 接收计数
g_rec_cnt = 0
# 发送计数
g_snd_cnt = 0

def log_print(str):
    """
    带时间日期的打印信息输出
    """
    now = datetime.datetime.now()
    otherStyleTime = now.strftime("%Y-%m-%d %H:%M:%S")
    msg = otherStyleTime + " - " + str
    print(msg)

def get_serial():
    """
    获取串口列表
    """
    plist = list(serial.tools.list_ports.comports())
    result = []
    if len(plist) > 0:
        for p in list(plist):
            result.append(p[0])
    return result

def open_port(ss, bps):
    """
    打开串口
    """
    port = None
    try:
        port = serial.Serial(ss, bps)
        if port.is_open:
            # self.log_print("串口打开成功：" + str(ss))
            pass
        else:
            log_print("串口打开失败：" + str(ss))
    except Exception:
        log_print("串口打开失败：" + str(ss))

    return port

def close_port(port):
    """
    关闭串口
    """
    try:
        port.close()
    except Exception:
        log_print("串口关闭失败：" + str(port))

def hex_str(argv):
    """
    #十六进制显示
    """
    result = ''  
    try:
        hLen = len(argv)  
        for i in range(hLen):  
            hvol = argv[i]
            hhex = '%02X'%hvol  
            result += hhex+' '  
    except:
        pass
    return result
