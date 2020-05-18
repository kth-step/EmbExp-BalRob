#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import serial

import balrob

with serial.Serial(balrob.serialdevice, 9600, timeout=None) as ser:
	print("="*10)
	print("m,on/off")
	print("e,0/1/2")
	print("a,+/-")
	print("v[,{filename},0/1]")
	print("="*10)
	print("")
	try:
		while True:
			s = input("choose: ")
			c = s.split(",")
			if c[0] == "m":
				balrob.set_motor(ser, c[1] == "on")

			elif c[0] == "e":
				x = 0
				try:
					x = int(c[1])
				except ValueError:
					pass
				balrob.set_exec(ser, x)

			elif c[0] == "a":
				a = 1 if c[1] == "+" else -1
				balrob.add_angle(ser, a)

			elif c[0] == "v":
				if s == c[0]:
					c = [c[0], "output/balrob.elf.reloadtext", 0]
				loc = -1
				try:
					x = int(c[2])
					assert (0 <= x) and (x <= 1)
					loc = x
				except:
					pass
				if loc >= 0:
					balrob.send_binary(ser, c[1], loc, True)

			else:
				print(f"unknown command - {c[0]}")
	except KeyboardInterrupt:
		pass

