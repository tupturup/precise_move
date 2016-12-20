#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from time import sleep
import sys
import serial
from flask import Flask, redirect, url_for, request
from flask import render_template, jsonify
import MySQLdb
from flaskext.mysql import MySQL
import sqlite3
import mysql.connector as mc

app = Flask(__name__)


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

    try:
        connection = mc.connect (host = "localhost",
                             user = "precibeo",
                             passwd = "precibeo",
                             db = "targetList")
    except mc.Error as e:
        print("Error %d: %s" % (e.args[0], e.args[1]))
        sys.exit(1)

    cursor = connection.cursor()

    # there is a table named "tableName" INT(10)
    sql_command = """ CREATE TABLE IF NOT EXISTS `tbl_target` (
      `tgt_id` BIGINT AUTO_INCREMENT,
      `tgt_name` INT(10),
      `value_x` INT(10) NULL,
      `value_y` INT(10) NULL,
      `value_z` INT(10) NULL,
      PRIMARY KEY (`tgt_id`));"""

    cursor.execute(sql_command)

    format_str = """INSERT INTO tbl_target (tgt_name, value_x, value_y, value_z)
    VALUES ('{tgtname}', {x}, {y}, {z});"""
    sql_command = format_str.format(tgtname=t_name, x=x, y=y, z=z)
    #sql_command = ('INSERT INTO tbl_target (tgt_name, value_x, value_y, value_z) VALUES (?, ?, ?, ?);', (t_name, x, y, z))

    print(sql_command)
    cursor.execute(sql_command)
    connection.commit()
    cursor.close()
    connection.close()

    return jsonify(result=out)
    #return  redirect(url_for('__main__'))


if __name__ == "__main__":
    print('oh hello')
    #sleep(10)
    sys.stdout.flush()
    app.debug = True
    app.run()
