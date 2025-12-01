#!/bin/bash

# Compile
g++ -g -O0 -I . interrupts_EhimareIsoa_AshfaqulAlam_EP.cpp -o bin/interrupts_EP

# Run all 20 test cases
for i in {1..20}
do
    ./bin/interrupts_EP "Input/test case $i.txt"
    cp execution.txt "Output/Output_EP/test case ${i}_output.txt"
done
