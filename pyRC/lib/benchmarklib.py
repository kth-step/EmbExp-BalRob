
import time
import struct
import random

import balrob


# running a single experiment (start with elementary steps)
# ===============================================================
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

def run_experiment_f2(bc, i, a, b):
	assert(type(a) == float)
	assert(type(b) == float)
	assert(0 <= i and i <= 1)

	m = struct.pack("<ff", a, b)

	bc.send_message((102 + i, m))
	# expect result and then ok(102 + i)

	#print("collecting cycles results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"cyclesres")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	cycles = int(m.decode(), 16)

	#print("collecting computation results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"res")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	res = int(m.decode(), 16)
	(res,) = struct.unpack("<f", struct.pack("<L", res))

	#print("ok(102 + i)")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == f"ok{102+i}".encode("ascii"))

	print((cycles, res))
	return (cycles, res)

def run_experiment_motor_set(bc, a, b):
	assert(type(a) == int)
	assert(type(b) == int)

	m = struct.pack("<ll", a, b)

	bc.send_message((105, m))
	# expect result and then ok(105)

	#print("collecting cycles results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"cyclesres")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	cycles = int(m.decode(), 16)

	#print("ok(105)")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"ok105")

	return cycles

def run_experiment_motor_set_l(bc, a):
	assert(type(a) == int)

	m = struct.pack("<l", a)

	bc.send_message((106, m))
	# expect result and then ok(106)

	#print("collecting cycles results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"cyclesres")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	cycles = int(m.decode(), 16)

	#print("ok(106)")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"ok106")

	return cycles

def run_experiment_motor_prep_input(bc, a):
	assert(type(a) == int)

	m = struct.pack("<l", a)

	bc.send_message((107, m))
	# expect result and then ok(107)

	#print("collecting cycles results")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"cyclesres")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	cycles = int(m.decode(), 16)

	#print("ok(107)")
	(ch, m) = bc.recv_message()
	assert(ch == 0)
	assert(m == b"ok107")

	return cycles

# composition of elementary steps
def execute_experiment(bc, inputs):
	#print("sending inputs")
	send_inputs(bc, inputs)

	#print("running experiment")
	cycles = run_experiment(bc)

	#print("receiving messages until ready for inputs")
	wait_until_ready(bc)
	return cycles

# composition of elementary steps (fadd)
def execute_experiment_fadd(bc, a, b):
	#print("running experiment")
	(cycles, res) = run_experiment_f2(bc, 0, a, b)
	#print(f"================================>>> {a} + {b} = {res}")
	#python_res = struct.unpack("<f", struct.pack("<f", a+b))[0]
	#print(f"{res - python_res} ({a}, {b}, {res}, {python_res})")
	#assert(python_res == res)
	return cycles

# composition of elementary steps (fdiv)
def execute_experiment_fdiv(bc, a, b):
	#print("running experiment")
	(cycles, res) = run_experiment_f2(bc, 1, a, b)
	#print(f"================================>>> {a} / {b} = {res}")
	#python_res = struct.unpack("<f", struct.pack("<f", a/b))[0]
	#print(f"{res - python_res} ({a}, {b}, {res}, {python_res})")
	#assert(python_res == res)
	return cycles

# composition of elementary steps (motor_set)
def execute_experiment_motor_set(bc, a, b):
	#print("running experiment")
	cycles = run_experiment_motor_set(bc, a, b)
	return cycles

# composition of elementary steps (motor_set_l)
def execute_experiment_motor_set_l(bc, a):
	#print("running experiment")
	cycles = run_experiment_motor_set_l(bc, a)
	return cycles

# composition of elementary steps (motor_prep_input)
def execute_experiment_motor_prep_input(bc, a):
	#print("running experiment")
	cycles = run_experiment_motor_prep_input(bc, a)
	return cycles





# inputs conversions
# ===============================================================
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

def bytes_to_base64(bs):
	import base64
	b64str = base64.b64encode(bs).decode('utf-8')
	return b64str

def base64_to_bytes(b64str):
	import base64
	bs = base64.b64decode(b64str.encode("ascii"))
	return bs



# inputs tweaking
# ===============================================================
def set_inputs_motor_on(inputs):
	arr = bytearray(inputs)
	arr[4*(6) + 1*(1)] = 1
	return bytes(arr)
def set_inputs_msg_flag(inputs):
	arr = bytearray(inputs)
	arr[4*(6)] = 1
	return bytes(arr)



# helper
# ===============================================================
def gen_float_32(sign, exp, mant):
	assert(0x0 <= sign and sign <= 0x1)
	assert(0x0 <= exp  and exp  <= 0xff)
	assert(0x0 <= mant and mant <= 0x7fffff)
	as_int = (sign << 31) | (exp << 23) | (mant << 0)
	(floatval,) = struct.unpack(">f", struct.pack(">L", as_int))
	return floatval

def gen_rand_float():
	sign = random.randint(0x0, 0x1)
	exp  = random.randint(0x0, 0xff)
	mant = random.randint(0x0, 0x7fffff)
	return gen_float_32(sign, exp, mant)

def gen_rand_int32():
	v = random.randint(-2147483648, 2147483647)
	return v


# example inputs (as dictionary)
# ===============================================================
fixed_inputs_dict_1 = {
	"kp"          : 0.0,
	"ki"          : 0.0,
	"kd"          : 0.0,
	"angleLast"   : 0.0,
	"errorLast"   : 0.0,
	"errorSum"    : 0.0,
	"msg_flag"    : 1,
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
	"motor_on"    : 1,
	"angleTarget" : 12.0,
	"pid_counter" : 128,
	"accX"        : 1024,
	"accZ"        : -1500,
	"gyrY"        : -2048
}

minfloat = gen_float_32(0x1, 0xfe, 0x7fffff)
maxfloat = gen_float_32(0x0, 0xfe, 0x7fffff)
highfloat = gen_float_32(0x0, 0xcc, 0x777777)
nanfloat = gen_float_32(0x0, 0xff, 1)
fixed_inputs_dict_3 = {
	"kp"          : nanfloat,
	"ki"          : nanfloat,
	"kd"          : nanfloat,
	"angleLast"   : nanfloat,
	"errorLast"   : nanfloat,
	"errorSum"    : nanfloat,
	"msg_flag"    : 1,
	"motor_on"    : 1,
	"angleTarget" : 0.0,
	"pid_counter" : 128,
	"accX"        : 0,
	"accZ"        : 0,
	"gyrY"        : 0
}

fixed_inputs_dict_4 = {
	"kp"          : 37112590336.0,
	"ki"          : -4.3451322651937495e-39,
	"kd"          : -1.5269077360130874e+21,
	"angleLast"   : 0.7656412124633789,
	"errorLast"   : -255705184.0,
	"errorSum"    : -1.9250302879678766e-23,
	"msg_flag"    : 251,
	"motor_on"    : 1,
	"angleTarget" : 1.0282247330339519e-23,
	"pid_counter" : 4258990454,
	"accX"        : -20554,
	"accZ"        : -9103,
	"gyrY"        : -7592
}

fixed_inputs_dict_5 = {
	"kp"          : 37112590336.0,
	"ki"          : -4.3451322651937495e-39,
	"kd"          : -1.5269077360130874e+2,
	"angleLast"   : 0.7656,
	"errorLast"   : -2557051.0,
	"errorSum"    : -1000.9250302879678766e-23,
	"msg_flag"    : 1,
	"motor_on"    : 1,
	"angleTarget" : 1.0282247330339519e-23,
	"pid_counter" : 0,
	"accX"        : -31588,
	"accZ"        : -8103,
	"gyrY"        : -7592
}


inputs_types = {
	"kp"          : (float, 32),
	"ki"          : (float, 32),
	"kd"          : (float, 32),
	"angleLast"   : (float, 32),
	"errorLast"   : (float, 32),
	"errorSum"    : (float, 32),
	"msg_flag"    : (int, False, 8),
	"motor_on"    : (int, False, 8),
	"angleTarget" : (float, 32),
	"pid_counter" : (int, False, 32),
	"accX"        : (int, True, 16),
	"accZ"        : (int, True, 16),
	"gyrY"        : (int, True, 16)
}

# inputs generators
# ===============================================================
inputs_bin_len = len(dict_to_inputs(fixed_inputs_dict_1))
def gen_random_inputs_binary():
	inputs = bytes(random.getrandbits(8) for _ in range(inputs_bin_len))
	return inputs

def gen_random_input_binary_k_variation(inputs):
	length = 4*(3)
	arr = bytearray(inputs)
	arr_var = bytearray(random.getrandbits(8) for _ in range(length))
	return bytes(arr_var + arr[length:])

"""
def gen_rand_distribution(t):
	if t == (float, 32):
		# float("-inf"), float("inf")
		minfloat = gen_float_32(0x1, 0xfe, 0x7fffff)
		maxfloat = gen_float_32(0x0, 0xfe, 0x7fffff)
		return random.uniform(minfloat, maxfloat)
	elif t == (int, False, 8):
		return 0
	elif t == (int, False, 32):
		return 0
	elif t == (int, True, 16):
		return 0
	else:
		raise Exception("cannot handle datatype")
def gen_random_inputs_distribution():
	inputs_types = dict([(k,type(v)) for k,v in fixed_inputs_dict_1.items()])
	print(inputs_types)
	raise Exception("")
	inputs = bytes(random.getrandbits(8) for _ in range(inputs_bin_len))
	return inputs
"""

