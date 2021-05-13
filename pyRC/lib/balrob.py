import struct
import time

import serial


def get_balrob_comm_serial():
	serialdevice = "/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0"
	return serial.Serial(serialdevice, 9600, timeout=None)

def decode_package(ser, handler):
	n_failpacks = 0
	while True:
		x = None
		while True:
			x = ser.read()[0]
			if x == 0x55:
				break
			print(f"aaaaaa - {x}")

		x = ser.read()[0]
		if x != 0xAA:
			print(f"aaaaaa22222 - {x}")
			continue

		m_ch = ser.read()[0]
		m_len = ser.read()[0]
		m = ser.read(m_len)

		m_ok = True
		m_ok = m_ok and (ser.read()[0] == 0x88)
		m_ok = m_ok and (ser.read()[0] == 0x11)

		if m_ok:
			handler(m_ch, m)
		else:
			n_failpacks += 1
			print(f"wrong packets = {n_failpacks}")

def parse_pid_pack(m):
	#print(m)
	#print(len(m))
	s = struct.unpack("<LLLffffBxxx", m)
	p = {
		"pid_sampletime": s[0],
		"pid_handlertime": s[1],
		"pid_counter": s[2],

		"angle": s[3],
		"error": s[4],
		"errorDiff": s[5],
		"errorSum": s[6],

		"last_noyield": s[7]
	}
	return p

def trydecode(m):
	try:
		return m.decode()
	except:
		return m

def package_handler(ch, m):
	if ch == 0:
		print(f"inf - {trydecode(m)}")
	elif ch == 1:
		print(f"dbg - {trydecode(m)}")
	elif ch == 2:
		print(f"err - {trydecode(m)}")
	elif ch == 10:
		print("pid")
		print(parse_pid_pack(m))
	else:
		print(f"unhandled channel {ch} - {m}")

def send_data(ser, ch, m):
	m_len = len(m)
	if m_len >= 255:
		raise Exception(f"message too long ({m_len}): {m}")

	m1 = struct.pack("<BBBB", 0x55, 0xAA, ch, m_len)
	m2 = struct.pack("<BB", 0x88, 0x11)

	msg = m1 + m + m2
	#print(msg)

	ser.write(msg)
	time.sleep(0.3)

def set_motor(ser, is_on):
	m = struct.pack("<l", 1 if is_on else 0)
	send_data(ser, 50, m)

def set_pid_info(ser, is_on):
	m = struct.pack("<l", 1 if is_on else 0)
	send_data(ser, 51, m)

def set_exec(ser, v):
	m = struct.pack("<l", v)
	send_data(ser, 80, m)

def add_angle(ser, a):
	m = struct.pack("<f", a)
	send_data(ser, 71, m)

def reset_uploads(ser):
	m = struct.pack("<l", 0)
	send_data(ser, 81, m)

def send_code(ser, mloc, dat, verify_data = True):
	c = 82 if verify_data else 83
	m = struct.pack("<L", mloc)
	send_data(ser, c, m+dat)

def send_binary(ser, filename, idx, verify_data = True):
	s_procedure = "verification" if verify_data else "writing"
	print(f"start {s_procedure} - {filename} - {idx}")
	reset_uploads(ser)

	#send_code(ser, 0x0, b"\xb0\xb5")
	#send_code(ser, 0x2, b"\x98\xb0")
	#return

	print("")
	with open(filename, "rb") as f:
		filelength = 0xa00
		for _ in range(idx):
			f.read(filelength)
		chunksize = 200
		n_processed = 0
		for addr in range(0xa00):
			if addr % chunksize != 0:
				continue
			mloc = addr + idx * 0xa00
			n_remaining = filelength - n_processed
			n_curchunk = min(chunksize, n_remaining)
			dat = f.read(n_curchunk)
			n_processed += len(dat)
			send_code(ser, mloc, dat, verify_data)
			print(f"\r{n_processed/filelength*100:.{1}f}%", end = '')
	print("")
	print(f"done {s_procedure} - {filename} - {idx}")



def set_pid_kp(ser, v):
	m = struct.pack("<f", v)
	send_data(ser, 60, m)

def set_pid_ki(ser, v):
	m = struct.pack("<f", v)
	send_data(ser, 61, m)

def set_pid_kd(ser, v):
	m = struct.pack("<f", v)
	send_data(ser, 62, m)

def set_angle(ser, v):
	m = struct.pack("<f", v)
	send_data(ser, 70, m)

