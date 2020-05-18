import struct

serialdevice = "/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0"

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

def package_handler(ch, m):
	if ch == 0:
		print(f"inf - {m}")
	elif ch == 1:
		print(f"dbg - {m}")
	elif ch == 2:
		print(f"err - {m}")
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

def set_motor(ser, is_on):
	m = struct.pack("<l", 1 if is_on else 0)
	send_data(ser, 50, m)

def set_exec(ser, v):
	m = struct.pack("<l", v)
	send_data(ser, 80, m)

def add_angle(ser, a):
	m = struct.pack("<f", a)
	send_data(ser, 71, m)


