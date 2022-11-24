#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import threading
import time
import json

from datetime import datetime
import os

import balrobcomm
import benchmarklib


# beginning of the script
# ===============================================================
print("generating inputs")
special_inputs = benchmarklib.dict_to_inputs(benchmarklib.fixed_inputs_dict_5)
num_exps = 1000
inputs_list = [benchmarklib.gen_random_inputs_binary() for _ in range(num_exps)]
#inputs_list = [benchmarklib.gen_random_inputs_distribution() for _ in range(num_exps)]
#inputs_list = [special_inputs]
#inputs_list = [benchmarklib.gen_random_input_binary_k_variation(special_inputs) for _ in range(num_exps)]

experiment_results = []
now_str = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
results_dir = "experiment_results"
experiment_results_filename = f"{results_dir}/{now_str}.json"
if not os.path.isdir(results_dir):
	os.mkdir(results_dir)

if True:
	overall_start_time = time.time()
	min_exp = (9999999999, None)
	max_exp = (0, None)
	experiment_results = []
	try:
		with balrobcomm.BalrobComm() as bc:
			for _ in range(num_exps):
				#a = benchmarklib.gen_rand_float()
				#b = benchmarklib.gen_rand_float()
				#inputs_s = (a, b)
				#c = benchmarklib.gen_rand_int32()
				#d = benchmarklib.gen_rand_int32()
				c = benchmarklib.gen_rand_uint32()
				d = benchmarklib.gen_rand_uint32()
				#inputs_s = (c, d)
				#e = benchmarklib.gen_rand_uint32()
				#f = benchmarklib.gen_rand_uint32()
				#inputs_s = (c, d, e, f)
				#inputs_s = (c,)
				cycles = None
				try:
					#cycles = benchmarklib.execute_experiment_fadd(bc, *inputs_s)
					#cycles = benchmarklib.execute_experiment_fdiv(bc, *inputs_s)
					#cycles = benchmarklib.execute_experiment_motor_set(bc, *inputs_s)
					#cycles = benchmarklib.execute_experiment_motor_set_l(bc, *inputs_s)
					#cycles = benchmarklib.execute_experiment_motor_prep_input(bc, *inputs_s)
					#cycles = benchmarklib.run_experiment__reffunc_test4(bc)
					#cycles = benchmarklib.run_experiment__alignmenttestfun(bc, *inputs_s)
					#cycles = benchmarklib.run_experiment__mymodexp(bc, *inputs_s)
					inputs_s = ((2 ** 32) - 1, 1)
					cycles = benchmarklib.run_experiment_uidivmod(bc, *inputs_s)
					print(f"==========>>>>> {cycles}")
					if cycles > max_exp[0]:
						max_exp = (cycles, inputs_s)
					if cycles < min_exp[0]:
						min_exp = (cycles, inputs_s)
				finally:
					experiment_result = (inputs_s, cycles, None)
					experiment_results.append(experiment_result)
	except KeyboardInterrupt:
		print("")
	finally:
		with open(experiment_results_filename, "w") as f:
			json.dump(experiment_results, f)
		print("min cycles experiment:")
		print(min_exp)
		print("max cycles experiment:")
		print(max_exp)
		time_diff = time.time() - overall_start_time
		print(40 * "=")
		print(f"    overall benchmarking time: {time_diff:.2f}s")
		print(40 * "=")
	print("terminating.")
	sys.exit(0)

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
				#inputs = benchmarklib.set_inputs_msg_flag(inputs)
				#inputs = benchmarklib.set_inputs_motor_on(inputs)
				#print(inputs_to_dict(inputs)["motor_on"])

				# prepare inputs for storing
				inputs_s = benchmarklib.bytes_to_base64(inputs)

				# run the experiment
				cycles = None
				time_diff = None
				try:
					start_time = time.time()
					cycles = benchmarklib.execute_experiment(bc, inputs)
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

