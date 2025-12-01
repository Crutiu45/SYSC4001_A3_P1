#!/bin/bash

# Compile
g++ -g -O0 -I . interrupts_EhimareIsoa_AshfaqulAlam_RR.cpp -o bin/interrupts_RR

# Run all 20 test cases
for i in {1..20}
do
    ./bin/interrupts_RR "Input/test case $i.txt"
    cp execution.txt "Output/Output_RR/test case ${i}_output.txt"
done
