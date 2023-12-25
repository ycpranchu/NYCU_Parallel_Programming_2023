#!/bin/sh
make
time -p ./PPF testcase/case_300.in testcase/connect_300.in
# python3 check.py