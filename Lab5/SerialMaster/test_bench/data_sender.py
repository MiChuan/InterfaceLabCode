# -*- coding: utf-8 -*-

from serial_ctrl import *
import math
import time

if __name__ == "__main__":

    i  = 0.0
    port = open_port("COM1", 9600)
    while True:
        d = math.sin(i)
        i = (i+0.1)%360
        msg = str(d)
        print(msg)
        port.write(msg.encode('utf-8'))
        time.sleep(0.1)
