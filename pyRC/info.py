#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import serial

import balrob

with serial.Serial(balrob.serialdevice, 9600, timeout=None) as ser:
	balrob.decode_package(ser, balrob.package_handler)

