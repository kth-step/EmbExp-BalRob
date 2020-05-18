#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import serial

import balrob

with serial.Serial(balrob.serialdevice, 9600, timeout=None) as ser:
	print("="*10)
	print("p,on/off")
	print("m,on/off")
	print("e,0/1/e")
	print("a,+/-")
	print("v/w[,{filename},0/1]")
	print("="*10)
	print("shorthands")
	print("="*10)
	print("(v/w)(0/1)")
	print("e(0/1/e)")
	print("="*10)
	print("")
	try:
		while True:
			s = input("choose: ")

			if s == "v0":
				s = "v,output/balrob.elf.reloadtext,0"
			elif s == "v1":
				s = "v,output/balrob.elf.reloadtext,1"
			elif s == "w0":
				s = "w,output/balrob.elf.reloadtext,0"
			elif s == "w1":
				s = "w,output/balrob.elf.reloadtext,1"

			elif s == "e0":
				s = "e,0"
			elif s == "e1":
				s = "e,1"
			elif s == "ee":
				s = "e,e"

			c = s.split(",")
			if c[0] == "p":
				balrob.set_pid_info(ser, c[1] == "on")
			elif c[0] == "m":
				balrob.set_motor(ser, c[1] == "on")

			elif c[0] == "e":
				x = 99
				try:
					x = int(c[1])
				except ValueError:
					pass
				balrob.set_exec(ser, x)

			elif c[0] == "a":
				a = 1 if c[1] == "+" else -1
				balrob.add_angle(ser, a)

			elif c[0] == "v" or c[0] == "w":
				if s == c[0]:
					c = [c[0], "output/balrob.elf.reloadtext", 1]
				loc = -1
				try:
					x = int(c[2])
					assert (0 <= x) and (x <= 1)
					loc = x
				except:
					pass
				if loc >= 0:
					balrob.send_binary(ser, c[1], loc, c[0] == "v")

			else:
				print(f"unknown command - {c[0]}")
	except KeyboardInterrupt:
		pass

