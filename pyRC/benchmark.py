#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import threading
import time
import struct
import random

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

	time.sleep(0.1)
	assert(bc.recv_available() == 0)

def send_inputs(bc, inputs):
	bc.send_message((101, inputs))

	time.sleep(0.1)
	#print(bc.recv_available())
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
	#print("receiving messages until ready for inputs")
	wait_until_ready(bc)

	#print("sending inputs")
	send_inputs(bc, inputs)

	#print("running experiment")
	cycles = run_experiment(bc)
	return cycles

fixed_inputs_dict_1 = {
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

fixed_inputs_dict_2 = {
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

keylist = [
	"kp", "ki", "kd", "angleLast", "errorLast", "errorSum",
	"msg_flag", "motor_on", "angleTarget", "pid_counter",
	"accX", "accZ", "gyrY"
]
structpattern = "<ffffffBBxxfLhhhxx"
def dict_to_inputs(d):
	s = list(map(lambda k: d[k], keylist))
	inputs = struct.pack(structpattern, *s)
	return inputs
def inputs_to_dict(inputs):
	s = struct.unpack(structpattern, inputs)
	assert(len(s) == len(keylist))
	d = dict(list(zip(keylist, s)))
	return d

def set_inputs_motor_on(inputs):
	arr = bytearray(inputs)
	arr[4*(6) + 1*(1)] = 1
	return bytes(arr)

inputs_bin_len = len(dict_to_inputs(fixed_inputs_dict_1))
def gen_random_inputs_binary():
	inputs = bytes(random.getrandbits(8) for _ in range(inputs_bin_len))
	return inputs

print("generating inputs")
inputs_list = [gen_random_inputs_binary() for _ in range(10)]

print("starting experiments")
try:
	with balrobcomm.BalrobComm() as bc:
		# loop through several inputs
		for inputs in inputs_list:
			#inputs = dict_to_inputs(fixed_inputs_dict_2)
			#inputs = gen_random_inputs_binary()
			inputs = set_inputs_motor_on(inputs)
			#print(inputs_to_dict(inputs)["motor_on"])

			start_time = time.time()
			cycles = execute_experiment(bc, inputs)
			time_diff = time.time() - start_time
			print(f"==========>>>>> {cycles} (exp time: {time_diff:.2f}s)")

except KeyboardInterrupt:
	print("")

print("terminating.")

