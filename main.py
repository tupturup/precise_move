#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from time import sleep
import os
import sys
import serial
from flask import Flask, redirect, url_for, request, render_template, jsonify, flash, Response
import MySQLdb
from flaskext.mysql import MySQL
import sqlite3
from sqlalchemy import or_

from sql import session, engine
from models import Target
from forms import TargetForm
#import mysql.connector as mc

app = Flask(__name__)
app.secret_key = 'some_secret'

Target.metadata.create_all(engine)


from flask import jsonify

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
    #promijeniti imena targets i target da se ne zbunjujem


@app.route('/targets')
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
            ser = serial.Serial(port='COM2', baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=3)

            sss = 'XJ'+ str(int(target.value_x)) +','+ str(int(target.value_y)) + ',' + str(int(target.value_z)) + '\r'
            ser.write(sss)
            result = ser.read(30)

            session.add(target)
            session.commit()
            return redirect(url_for('index'))
        else:
            error = "Fields are not filled correctly!"

    return render_template('target_form.html', form=form, action="/add", error=error)
    #return jsonify(result=out)
    #return  redirect(url_for('__main__'))


@app.route("/edit/<int:tgt_id>", methods=['GET', 'POST'])
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
    return render_template('target_form.html', form=form, action="/%s/edit" % tgt_id, submit_text="Save")


@app.route("/delete/<int:tgt_id>")
def delete_target(tgt_id):
    target = session.query(Target).get(tgt_id)
    session.delete(target)
    session.commit()
    return redirect(url_for('index'))
    #return ("Deleted")


@app.route("/run/<int:tgt_id>", methods=['GET', 'POST'])
def run_target(tgt_id):
    targets = session.query(Target).all()
    result = None
    target_db = session.query(Target).get(tgt_id)

    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)
    sss = 'XJ'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r'
    ser.write(sss)
    result = ser.read(30)
    #resp = Response(response=result, status=200, mimetype="application/json")
    #resp = Response(response=result, status=200, mimetype="text/html")
    #return redirect(url_for('index', result=result))
    #return jsonify(result=result, targets=targets)
    #return (resp)
    return render_template('index.html', result=result, targets=targets)


@app.route("/dev_ide")
def dev_ide():
    return render_template('dev_ide.html')


if __name__ == "__main__":
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
