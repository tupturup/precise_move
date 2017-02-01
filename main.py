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

    ser1 = serial.Serial(port='COM10', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)
    sss = 'X1J'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r'
    sss1 = 'X2J'+ str(int(target_db.value_x)) +','+ str(int(target_db.value_y)) + ',' + str(int(target_db.value_z)) + '\r'

    ser1.write(sss)
    ser1.write(sss1)
    result = ser1.read(30)

    ser1.close()

    return render_template('index.html', result=result, targets=targets)


@flask_sijax.route(app, "/dev_ide")
def dev_ide():
    targets = session.query(Target).all()
    def say_hi(obj_response):
       obj_response.alert("hi")
    if g.sijax.is_sijax_request:
       # Sijax request detected - let Sijax handle it
       g.sijax.register_callback('say_hi', say_hi)
       return g.sijax.process_request()
    return render_template('dev_ide.html')

@app.route("/search_results", methods=['GET'])
def search_results():
    q = request.values['q']
    targets = session.query(Target).filter(
    or_(Target.tgt_name.ilike("%" + q + "%"), Target.value_x.ilike("%" + q + "%"), Target.value_y.ilike("%" + q + "%"), Target.value_z.ilike("%" + q + "%")))
    return render_template('search_results.html', targets=targets, q=q)


# @app.route("/commands", methods=['POST', 'GET'])
# def run_commands():
#     targets = session.query(Target).all()
#     comm = request.values['comm']
#     pars = parseString(comm)
#     x, y, z = 0, 0, 0
#     lenx, leny, lenz = 0, 0, 0
#
#     if pars[0] == "goto":
#         x = pars[1]
#         if x[0:2] == "X=":
#             return len(x)
#             lenx = len(x)-2
#             return lenx
#         for i in a:
#             if i[0] == "X":
#                 return "wds"
#



if __name__ == "__main__":
    #sleep(10)
    #sys.stdout.flush()
    #app.debug = True
    app.run()
