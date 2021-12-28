#!/bin/bash

gcc -O0 main.c -o main && taskset -c 0 ./main
