#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import balrob

try:
	with balrob.get_balrob_comm() as ser:
		balrob.decode_package(ser, balrob.package_handler)
except KeyboardInterrupt:
	pass

print("terminating.")

