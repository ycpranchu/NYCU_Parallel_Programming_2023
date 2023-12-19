#!/bin/sh
make clean && make
time -p ./serial_PPF testcase/case.in 
./SolutionChecker testcase/case.in floorplan/partition_0.out
python3 check.py
