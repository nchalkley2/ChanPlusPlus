#!/usr/bin/env python

import sys
import os
import bcrypt
import pprint
import base64
from pymongo import MongoClient

from flask import Flask, request, Response, make_response
from flask.ext.api import status

import requests
from redef import redef

# Open the database
client = MongoClient('mongodb://localhost:27017')
db = client.chan

app = Flask(__name__)

# redefine os.urandom for bcrypt.gensalt()
def serv_rand(num_bytes):
    r = requests.get("http://10.0.0.131:4950/?bytes=" + str(int(num_bytes)))
    return base64.b64decode(r.text)

rd_f1 = redef(os, 'urandom', serv_rand)

@app.route('/register')
def register():
    username = request.args.get('username')
    password = request.args.get('password')

    if (username == None or password == None):
        return "Username or password was not passed in!"

    if (db.users.find({"username": username}).count() == 0):
        salt = bcrypt.gensalt()
        hashed_password = bcrypt.hashpw(password.encode('utf8'), salt)
        db.users.insert({
            "username": username,
            "password": hashed_password,
            "sessions": []
        })
        return "Created account!", status.HTTP_200_OK
    else:
        return Response("User already exists", status=500, mimetype='text/plain')

@app.route('/login')
def login():
    username = request.args.get('username')
    password = request.args.get('password')

    if (username == None or password == None):
        return "Username or password was not passed in!"

    user = db.users.find_one({'username': username})
    if (user != None):
        hashed_password = user['password']
        if (bcrypt.checkpw(password.encode(), hashed_password.encode())):
            resp = make_response("Login successful", 200)
            session_id = base64.b64encode(os.urandom(64), '.^')
            db.users.update({'username': username}, {'$push': {'sessions':session_id}})
            #user.update({'$push': {'sessions':session_id}})
            resp.set_cookie('session_id', username + "$" + session_id)
            return resp
        else:
            return Response("Password incorrect", status=500, mimetype='text/plain')
    else:
        return Response("User doesn't exist", status=500, mimetype='text/plain')

@app.route('/check-session')
def check_session():
    in_session_id = request.args.get('id')

    if (in_session_id == None):
        return "Session ID was not passed in!"

    username_id = in_session_id.split('$')
    username   = username_id[0]
    session_id = username_id[1]

    user = db.users.find_one({'username': username})
    if (user != None):
        if (session_id in user['sessions']):
            return Response("Session id correct", status=200)

    return Response("Session id incorrect", status=500)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
