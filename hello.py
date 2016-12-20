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

#mysql = MySQL()
app = Flask(__name__)

# MySQL configurations
#app.config['MYSQL_DATABASE_USER'] = 'precibeo'
#app.config['MYSQL_DATABASE_PASSWORD'] = 'precibeo'
#app.config['MYSQL_DATABASE_DB'] = 'TargetList'
#app.config['MYSQL_DATABASE_HOST'] = 'localhost'
#mysql.init_app(app)




@app.route('/')
def index():
    return render_template('index.html')


@app.route('/targets')
def targets():
    return render_template('targets.html')


@app.route('/_add_numbers', methods=['POST', 'GET'])
def add_numbers():

    ser = serial.Serial(port='COM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=3)

    x = str(request.args.get('x', 0, type=int))
    y = str(request.args.get('y', 0, type=int))
    z = str(request.args.get('z', 0, type=int))
    t = str(request.args.get('t', 0, type=str))

    #b = request.query_string('b')
    sss = 'XJ'+ x +','+ y + ',' + z + '\r'
    ser.write(sss)
    #sleep(1)
    #ser.write('XJ500,-100,0\r')
    out = ser.read(30)

    x = float(x)
    y = float(y)
    z = float(z)
    t = str(t)

    if x and y and z and t:
        #conn = mysql.connect()
        ##cursor = conn.cursor()
        #cursor = mysql.get_db().cursor()
        #cursor.callproc('sp_createTarget', (x, y, z, t))
        #data = cursor.fetchall()
        #conn.close()
        target_data = [x, y, z, t]

    try:
        connection = mc.connect (host = "localhost",
                             user = "precibeo",
                             passwd = "precibeo",
                             db = "targetlist")
    except mc.Error as e:
        print("Error %d: %s" % (e.args[0], e.args[1]))
        sys.exit(1)

    cursor = connection.cursor()


    try:
        # there is a table named "tableName"
        sql_command = """
        CREATE TABLE employee (
        staff_number INTEGER PRIMARY KEY,
        fname VARCHAR(20),
        lname VARCHAR(30),
        gender CHAR(1),
        joining DATE,
        birth_date DATE);"""

        cursor.execute(sql_command)


    staff_data = [ ("qqqqq", "a", "m", "1961-10-25"),
                   ("qweq", "Schiller", "m", "1955-08-17"),
                   ("ewqeqwe", "Wall", "f", "1989-03-14"),
                   ]

    for staff, p in enumerate(staff_data):
        format_str = """INSERT INTO employee (staff_number, fname, lname, gender, birth_date)
        VALUES ({staff_no}, '{first}', '{last}', '{gender}', '{birthdate}');"""

        sql_command = format_str.format(staff_no=staff, first=p[0], last=p[1], gender=p[2], birthdate = p[3])
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
