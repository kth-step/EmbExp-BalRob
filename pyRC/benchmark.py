#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import threading
import time
import struct

import balrobcomm
import balrob


def wait_until_ready(bc):
	# print until wait4inputs
	while True:
		(ch, m) = bc.recv_message()
		if ch != 0:
			balrob.package_handler(ch, m)
			continue
		m_ = balrob.trydecode(m)
		if m_ == "wait4inputs":
			break
		print(f"inf - {m_}")

	time.sleep(1)
	assert(bc.recv_available() == 0)

def send_inputs(bc, inputs):
	keylist = [
		"kp", "ki", "kd", "angleLast", "errorLast", "errorSum",
		"msg_flag", "motor_on", "angleTarget", "pid_counter",
		"accX", "accZ", "gyrY"
	]
	structpattern = "<ffffffBBxxfLhhhxx"
	s = list(map(lambda k: inputs[k], keylist))
	m = struct.pack(structpattern, *s)
	bc.send_message((101, m))

	time.sleep(1)
	print(bc.recv_available())
	assert(bc.recv_available() == 1)

	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"ok101")

def run_experiment(bc):
	bc.send_message((100, b""))
	# expect ok100, need error/timeout detection and handling here
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"ok100")

	#print("collecting results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"cyclesres")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	cycles = int(m.decode(), 16)
	return cycles

def execute_experiment(bc, inputs):
	print("receiving messages until ready for inputs")
	wait_until_ready(bc)

	print("sending inputs")
	send_inputs(bc, inputs)

	print("running experiment")
	cycles = run_experiment(bc)
	print(cycles)

fixed_inputs1 = {
	"kp"          : 0.0,
	"ki"          : 0.0,
	"kd"          : 0.0,
	"angleLast"   : 0.0,
	"errorLast"   : 0.0,
	"errorSum"    : 0.0,
	"msg_flag"    : 0,
	"motor_on"    : 0,
	"angleTarget" : 0.0,
	"pid_counter" : 0,
	"accX"        : 0,
	"accZ"        : 0,
	"gyrY"        : 0
}

fixed_inputs2 = {
	"kp"          : 0.0,
	"ki"          : 0.0,
	"kd"          : 0.0,
	"angleLast"   : 0.0,
	"errorLast"   : 0.0,
	"errorSum"    : 0.0,
	"msg_flag"    : 1,
	"motor_on"    : 0,
	"angleTarget" : 12.0,
	"pid_counter" : 128,
	"accX"        : 1024,
	"accZ"        : -1500,
	"gyrY"        : -2048
}

try:
	with balrobcomm.BalrobComm() as bc:
		# loop through several inputs
		while True:
			inputs = fixed_inputs2
			execute_experiment(bc, inputs)

except KeyboardInterrupt:
	pass

print("terminating.")

