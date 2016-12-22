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
from models import Target
from forms import TargetForm
#import mysql.connector as mc

app = Flask(__name__)

Target.metadata.create_all(engine)

@app.route('/')
def index():
    targets = session.query(Target).all()
    return render_template('index.html', targets=targets)
    #promijeniti imena targets i target da se ne zbunjujem


@app.route('/targets')
def targets():
    targets = session.query(Target).all()
    return render_template('targets.html', targets=targets)


@app.route('/_add_numbers', methods=['POST', 'GET'])
def add_numbers():
    form = TargetForm(request.form)
    
    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)

    value_x = str(request.args.get('value_x', type=int))
    value_y = str(request.args.get('value_y', type=int))
    value_z = str(request.args.get('value_z', type=int))
    name = str(request.args.get('name', type=str))

    sss = 'XJ'+ value_x +','+ value_y + ',' + value_z + '\r'
    ser.write(sss)
    out = ser.read(30)


    if request.method == 'POST' and form.validate():
        target = Target(form.name.data, form.value_x.data, form.value_y.data, form.value_z.data)
        session.add(target)
        session.commit()
        return redirect(url_for('index'), form=form)
    return render_template('target_form.html', form=form, action="/_add_numbers")
    #return jsonify(result=out)
    #return  redirect(url_for('__main__'))


if __name__ == "__main__":
    print('oh hello')
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
