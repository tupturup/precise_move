#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from time import sleep
import os
import sys
import serial
from flask import Flask, redirect, url_for, request, render_template, jsonify, flash, Response, g
import MySQLdb
from flaskext.mysql import MySQL
import sqlite3
from sqlalchemy import or_
import jinja2
from sql import session, engine
from models import Target
from forms import TargetForm
import flask_sijax
from pars import parseString
from commands import run
#from formulas import canIrun
#import mysql.connector as mc


path = os.path.join('.', os.path.dirname(__file__), 'static/js/sijax/')

app = Flask(__name__)
app.config['SIJAX_STATIC_PATH'] = path
app.config['SIJAX_JSON_URI'] = '/static/js/sijax/json2.js'
flask_sijax.Sijax(app)


app.secret_key = 'some_secret'

Target.metadata.create_all(engine)


class InvalidUsage(Exception):
    status_code = 400
    def __init__(self, message, status_code=None, payload=None):
        Exception.__init__(self)
        self.message = message
        if status_code is not None:
            self.status_code = status_code
            self.payload = payload

            def to_dict(self):
                rv = dict(self.payload or ())
                rv['message'] = self.message
                return rv


@app.errorhandler(InvalidUsage)
def handle_invalid_usage(error):
    response = jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


@app.route('/')
def index():
    targets = session.query(Target).all()
    return render_template('index.html', targets=targets)


@app.route('/targets', methods=['GET'])
def targets():
    targets = session.query(Target).all()
    return render_template('targets.html', targets=targets)


@app.route('/add', methods=['POST', 'GET'])
def add_numbers():
    form = TargetForm(request.form)
    error = None
    result  = None
    if request.method == 'POST':
        if form.validate():
            target = Target(form.tgt_name.data, form.value_x.data, form.value_y.data, form.value_z.data)
            flash('You sucessfully added a new target!')
            session.add(target)
            session.commit()
            return redirect(url_for('index'))
        else:
            error = "Fields are not filled correctly!"

    return render_template('target_form.html', form=form, method="POST", action="/add", error=error, submit_text="Save")


@app.route("/targets/<int:tgt_id>", methods=['GET'])
def show_target(tgt_id):
    target = session.query(Target).get(tgt_id)
    return render_template('show_target.html', target=target)

@app.route("/targets/<int:tgt_id>/edit", methods=['GET', 'POST'])
def edit_target(tgt_id):
    form = TargetForm(request.form)

    if request.method == 'GET':
        form = TargetForm(request.form, session.query(Target).get(tgt_id))
    if request.method == 'POST':
        if form.validate():
            tgt_edited = Target(form.tgt_name.data, form.value_x.data, form.value_y.data, form.value_z.data)
            flash(u'You sucessfully edited target %s' % form.tgt_name.data)
            target_db = session.query(Target).get(tgt_id)
            target_db.tgt_name = tgt_edited.tgt_name
            target_db.value_x = tgt_edited.value_x
            target_db.value_y = tgt_edited.value_y
            target_db.value_z = tgt_edited.value_z
            session.commit()
            return redirect(url_for('index'))
        else:
            error = "Fields are not filled correctly!"
    return render_template('target_form.html', form=form, method="POST", action="/targets/%s/edit" % tgt_id, submit_text="Save changes")


@app.route("/targets/<int:tgt_id>/delete", methods=['GET'])
def delete_target(tgt_id):
    target = session.query(Target).get(tgt_id)
    session.delete(target)
    session.commit()
    return redirect(url_for('index'))


@app.route("/targets/<int:tgt_id>/run", methods=['GET', 'POST'])
def run_target(tgt_id):
    targets = session.query(Target).all()
    result = None
    target_db = session.query(Target).get(tgt_id)

    result = run(target_db)

    return render_template('index.html', result=result, targets=targets)


# @flask_sijax.route(app, "/dev_ide")
# def dev_ide():
#     targets = session.query(Target).all()
#     def say_hi(obj_response):
#        obj_response.alert("hi")
#     if g.sijax.is_sijax_request:
#        # Sijax request detected - let Sijax handle it
#        g.sijax.register_callback('say_hi', say_hi)
#        return g.sijax.process_request()
#     return render_template('dev_ide.html')



@app.route("/search_results", methods=['GET'])
def search_results():
    q = request.values['q']
    targets = session.query(Target).filter(
    or_(Target.tgt_name.ilike("%" + q + "%"), Target.value_x.ilike("%" + q + "%"), Target.value_y.ilike("%" + q + "%"), Target.value_z.ilike("%" + q + "%")))
    return render_template('search_results.html', targets=targets, q=q)


@app.route( "/dev_ide", methods=['GET'])
def dev_ide():
    targets = session.query(Target).all()
    speed = '350'
    return render_template('dev_ide.html', targets=targets, speed=speed)


@app.route("/dev_ide", methods=['POST'])
def run_commands():
    targets = session.query(Target).all()
    comm = request.values['comm']
    speed = request.values['speed']
    if speed:
        speed = str(speed)
    else:
        speed = '300'

    if comm:
        ser = serial.Serial(port='COM10', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)
        countsA = None
        countsB = None
        countsC = None
        countsX = None
        countsY = None
        countsZ = None
        broadcast = 'X127T1,2\r'
        okToRun = None
        okX = None
        okY = None
        f = open('tekst.txt', 'a')
        list = parseString(comm)
        #l = len(parsed)
        #pairs = l-1
        #if parsed[0] == 'goto':


        for parsed in list:
            l = len(parsed)

            #pairs = l-1
            if parsed[0] == 'goto':
                for i in range(1,len(parsed)-1):
                    if parsed[i] == 'S' or parsed[i] == 's':
                        speed = str(int(float(parsed[i+1])/5))
                    if parsed[i] == 'X' or parsed[i] == 'x':
                        countsX = 'X1T' + str(int(float(parsed[i+1])/1.25)) + ',1\r'
                    if parsed[i] == 'Y' or parsed[i] == 'y':
                        countsY = 'X2T' + str(int(float(parsed[i+1])/1.25)) + ',1\r'
                    # if parsed[i] == 'Z' or parsed[i] == 'z':
                    #     valueZ = float(parsed[i+1])
                    #     countsZ = 'X3T' + str(int(valueZ/1.25)) + ',1\r'
                    # if parsed[i] == 'A' or parsed[i] == 'a':
                    #     valueA = float(parsed[i+1])
                    #     countsA = 'X4T' + str(int(valueA/1.25)) + ',1\r'
                    # if parsed[i] == 'B' or parsed[i] == 'b':
                    #     valueB = float(parsed[i+1])
                    #     countsB = 'X5T' + str(int(valueB/1.25)) + ',1\r'
                    # if parsed[i] == 'C' or parsed[i] == 'c':
                    #     valueC = float(parsed[i+1])
                    #     countsC = 'X6T' + str(int(valueC/1.25)) + ',1\r'

                if countsX != None:
                    f.write('X\n')
                    ser.write(countsX)
                    #bytesToRead = ser.inWaiting()
                    f.write(ser.read(20))
                    ser.write('X1Y8,' + speed + '\r')
                    #while True:
                    #bytesToRead = ser.inWaiting()
                    f.write(ser.read(20))
                    str3 = 'X1U\r'
                    ser.write(str3)
                    #while True:
                    #bytesToRead = ser.inWaiting()
                    r = ser.read(10)
                    #return r

                    f.write(r)
                    checker = str(r)

                    c = int('0x' + str(checker[6]), 16)
                    d = int('0x' + str(checker[7]), 16)

                    checkedC = hex(c & int('0x2', 16))
                    checkedD = hex(d & int('0x4', 16))

                    if checkedD == 0x4:
                        okX = '1'
                    else:
                        if checkedC == 0x2:
                            okX = '0'
                        else:
                            okX = '1'


                #    okX = canIrun(countsX, speed)
                if countsY != None:
                    f.write('Y\n')
                    ser.write(countsY)
                    #bytesToRead = ser.inWaiting()
                    f.write(ser.read(20))
                    ser.write('X2Y8,' + speed + '\r')
                    #bytesToRead = ser.inWaiting()
                    f.write(ser.read(20))
                    str3 = 'X2U\r'
                    ser.write(str3)
                    #bytesToRead = ser.inWaiting()
                    r = ser.read(10)
                    f.write(r)
                    checker = str(r)

                    c = int('0x' + str(checker[6]), 16)
                    d = int('0x' + str(checker[7]), 16)

                    checkedC = hex(c & int('0x2', 16))
                    checkedD = hex(d & int('0x4', 16))

                    if checkedD == 0x4:
                        okY = '1'
                    else:
                        if checkedC == 0x2:
                            okY = '0'
                        else:
                            okY = '1'
                #while (okX or okY):
                if (okX == '1' or okY == '1'):
                    ser.write(broadcast)

                #else:
                #    return "problem"

    else:
        return "error"
    f.close()
    ser.close()
    return render_template('dev_ide.html', targets=targets, speed=speed )


if __name__ == "__main__":
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
