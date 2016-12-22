#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from time import sleep
import os
import sys
import serial
from flask import Flask, redirect, url_for, request, render_template, jsonify
import MySQLdb
from flaskext.mysql import MySQL
import sqlite3
from sqlalchemy import or_

from sql import session, engine
from models import Target, Path
#import mysql.connector as mc

app = Flask(__name__)

Target.metadata.create_all(engine)

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/targets')
def targets():
    return render_template('targets.html')


@app.route('/_add_numbers', methods=['POST', 'GET'])
def add_numbers():

    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)

    x = str(request.args.get('x', type=int))
    y = str(request.args.get('y', type=int))
    z = str(request.args.get('z', type=int))
    t_name = str(request.args.get('t_name', type=str))

    sss = 'XJ'+ x +','+ y + ',' + z + '\r'
    ser.write(sss)
    out = ser.read(30)

    form = TargetForm(request.form)
    if request.method == 'POST' and form.validate():
        target = Target(form.name.data)
        session.add(target)
        session.commit()


    return jsonify(result=out)
    #return  redirect(url_for('__main__'))


if __name__ == "__main__":
    print('oh hello')
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
