#!/usr/bin/env python

import base64
import os
import sys

from flask import Flask, request, Response, make_response
from flask.ext.api import status

app = Flask(__name__)

@app.route('/')
def login():
    byte_str = request.args.get('bytes', '0')
    num_bytes = int(byte_str)
    return base64.b64encode(os.urandom(num_bytes))

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=4950)
