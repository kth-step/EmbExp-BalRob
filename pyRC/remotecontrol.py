#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import serial

import balrob

with serial.Serial(balrob.serialdevice, 9600, timeout=None) as ser:
	print("m_on/off, ")
	print("="*10)
	print("")
	try:
		while True:
			s = input("choose: ")
			if s == "m_on":
				balrob.set_motor(ser, True)
			elif s == "m_off":
				balrob.set_motor(ser, False)
			else:
				print("unknown command")
	except KeyboardInterrupt:
		pass

