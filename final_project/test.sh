#!/bin/sh
make
time -p ./serial_PPF testcase/case.in testcase/connect.in
# python3 check.py