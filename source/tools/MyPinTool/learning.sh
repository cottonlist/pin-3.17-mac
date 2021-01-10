#!/bin/bash
./RandomGenerator.out
../../../pin -t obj-intel64/scheduler_rl.dylib -- ~/pin-3.16-mac/source/tools/Benchmarks/idiom1_ex1.out
python3 learning.py