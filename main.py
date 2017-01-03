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


@app.route('/target/add', methods=['POST', 'GET'])
def add_numbers():
    form = TargetForm(request.form)

    if request.method == 'POST' and form.validate():
        target = Target(form.tgt_name.data, form.value_x.data, form.value_y.data, form.value_z.data)

        ser = serial.Serial(port='COM2', baudrate=115200,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            timeout=3)

        sss = 'XJ'+ str(int(target.value_x)) +','+ str(int(target.value_y)) + ',' + str(int(target.value_z)) + '\r'
        ser.write(sss)

        session.add(target)
        session.commit()
        return redirect(url_for('index'))
    return render_template('target_form.html', form=form, action="/target/add")
    #return jsonify(result=out)
    #return  redirect(url_for('__main__'))

@app.route("/target/<int:tgt_id>/edit", methods=['GET', 'POST'])
def edit_target(tgt_id):
    form = TargetForm(request.form)
    if request.method == 'GET':
        form = TargetForm(request.form, session.query(Target).get(tgt_id))
    if request.method == 'POST' and form.validate():
        tgt_edited = Target(form.tgt_name.data, form.value_x.data, form.value_y.data, form.value_z.data)
        target_db = session.query(Target).get(tgt_id)
        target_db.tgt_name = tgt_edited.tgt_name
        target_db.value_x = tgt_edited.value_x
        target_db.value_y = tgt_edited.value_y
        target_db.value_z = tgt_edited.value_z
        session.commit()
        return redirect(url_for('index'))
    return render_template('target_form.html', form=form, action="/target/%s/edit" % tgt_id, submit_text="Save")

@app.route("/target/<int:tgt_id>/delete")
def delete_target(tgt_id):
    target = session.query(Target).get(tgt_id)
    session.delete(target)
    session.commit()
    return redirect(url_for('index'))

@app.route("/target/<int:tgt_id>/run")
def run_target(tgt_id):
    target_db = session.query(Target).get(tgt_id)
    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)

    sss = 'XJ'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r'
    ser.write(sss)
    out = ser.read(30)

    return redirect(url_for('index'))

if __name__ == "__main__":
    print('oh hello')
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
