#!/usr/bin/env python3


import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), "lib"))

import balrob

with balrob.get_balrob_comm_serial() as ser:
	balrob.decode_package(ser, balrob.package_handler)

