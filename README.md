# EmbExp-BalRob - embedded experiment balancing robot
This an embedded experiment that is meant to be a physical demonstrator for execution time analysis.

The build infrastructure is shared with [`EmbExp-ProgPlatform`](https://github.com/kth-step/EmbExp-ProgPlatform) in order to integrate with analysis in [`HolBA`](https://github.com/kth-step/HolBA).

This repository contains the source code for a simple LPC11C24 based balancing robot as well as a companion tool `RemoteControl`.
The mechanical setup is inspired by the following instructions:
[Arduino Self-Balancing Robot](https://www.instructables.com/id/Arduino-Self-Balancing-Robot-1/).

![Balancing Robot](doc/img1.jpg)


## Getting started
Setup a [`HolBA`](https://github.com/kth-step/HolBA) environment with the appropriate gcc.
Install the tool egypt by running `make callgraph-install`.
Install the package graphviz with `apt` for example.

Compile by running `make`. View the dependency graph by running `make callgraph-full`.


## Write flash memory in the robot
- One shell with `make connect`
- Another one with `make clean run`


## Connect to the robot
- Configure serial port path in library file `pyRC/lib/balrob.py`
- Run monitor `./pyRC/info.py`
- Run remote commander `./pyRC/remotecontrol.py`


## Most annoying things that should be fixed
- I2C seems not to reset properly, sometimes stuck at boot, happens at each reset with optimization level 3 (O3), works with cold boot
- Remote loading of optimized and unoptimized code is not compatible for some reason (for example mixing optimized code in flash with remote loaded unoptimized code)

