#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import threading
import time
import struct
import random
import json

from datetime import datetime
import os

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

def bytes_to_base64(bs):
	import base64
	b64str = base64.b64encode(bs).decode('utf-8')
	return b64str

def base64_to_bytes(b64str):
	import base64
	bs = base64.b64decode(b64str.encode("ascii"))
	return bs

print("generating inputs")
inputs_list = [gen_random_inputs_binary() for _ in range(1000)]
experiment_results = []
now = datetime.now()
now_str = now.strftime("%Y-%m-%d_%H-%M-%S")
results_dir = "experiment_results"
experiment_results_filename = f"{results_dir}/{now_str}.json"
if not os.path.isdir(results_dir):
	os.mkdir(results_dir)

print("starting experiments")
overall_start_time = time.time()
try:
	with open(experiment_results_filename + "_log", "w") as f_log:
		f_log.write("[\n")
		with balrobcomm.BalrobComm() as bc:
			# loop through several inputs
			for inputs in inputs_list:
				#inputs = dict_to_inputs(fixed_inputs_dict_2)
				#inputs = gen_random_inputs_binary()
				inputs = set_inputs_motor_on(inputs)
				#print(inputs_to_dict(inputs)["motor_on"])

				# prepare inputs for storing
				inputs_s = bytes_to_base64(inputs)

				# run the experiment
				cycles = None
				time_diff = None
				try:
					start_time = time.time()
					cycles = execute_experiment(bc, inputs)
					time_diff = time.time() - start_time
					print(f"==========>>>>> {cycles} (exp time: {time_diff:.2f}s)")
				finally:
					# store inputs and cycle count
					time_diff_s = None if time_diff == None else f"{time_diff:.2f}"
					experiment_result = (inputs_s, cycles, time_diff_s)
					experiment_results.append(experiment_result)
					f_log.write(json.dumps(experiment_result))
					f_log.write(",\n")
					f_log.flush()
		f_log.write("null\n]\n")

except KeyboardInterrupt:
	print("")
finally:
	with open(experiment_results_filename, "w") as f:
		json.dump(experiment_results, f)
	time_diff = time.time() - overall_start_time
	print(40 * "=")
	print(f"    overall benchmarking time: {time_diff:.2f}s")
	print(40 * "=")

print("terminating.")

