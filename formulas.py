#!usr/bin/env python
from __future__ import print_function
from time import sleep
import os
import sys
import serial


def canIrun(countsX, hz):
    ser.write(countsX)
    ser.read(10)
    ser.write(hz)
    ser.read(10)
    str3 = 'X1U\r'
    ser.write(str3)
    r = ser.read(30)
    checker = str(r)
    c = int('0x' + str(checker[6]), 16)
    d = int('0x' + str(checker[7]), 16)

    checkedC = hex(c & int('0x2', 16))
    checkedD = hex(d & int('0x4', 16))

    if checkedD == 0x4:
        ok = 1
    else:
        if checkedC == 0x2:
            ok = 0
        else:
            ok = 1

    return ok
