#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from time import sleep
import sys
import serial
from flask import Flask, redirect, url_for, request
from flask import render_template, jsonify

app = Flask(__name__)

@app.route('/_add_numbers', methods=['POST', 'GET'])
def add_numbers():
    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)
    x = str(request.args.get('x', 0, type=int))
    y = str(request.args.get('y', 0, type=int))
    z = str(request.args.get('z', 0, type=int))
    #b = request.query_string('b')
    sss = 'XJ'+ x +','+ y + ',' + z + '\r'
    ser.write(sss)
    #sleep(1)
    #ser.write('XJ500,-100,0\r')
    out = ser.read(30)
    return jsonify(result=out)
    #return  redirect(url_for('__main__'))

@app.route('/')
def index():
    return render_template('index.html')


if __name__ == "__main__":
    print('oh hello')
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
