#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import serial

import balrob

with serial.Serial(balrob.serialdevice, 9600, timeout=None) as ser:
	print("m-on/off")
	print("r-0/1/2")
	print("="*10)
	print("")
	try:
		while True:
			s = input("choose: ")
			if s == "m-on":
				balrob.set_motor(ser, True)
			elif s == "m-off":
				balrob.set_motor(ser, False)
			elif s == "r-0":
				balrob.set_exec(ser, 0)
			elif s == "r-1":
				balrob.set_exec(ser, 1)
			elif s == "r-2":
				balrob.set_exec(ser, 2)
			else:
				print("unknown command")
	except KeyboardInterrupt:
		pass

