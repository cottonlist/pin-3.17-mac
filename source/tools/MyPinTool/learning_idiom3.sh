#!/bin/bash
./order_generator.out
../../../pin -t obj-intel64/scheduler_rl_v2.dylib -- ~/pin-3.16-mac/source/tools/Benchmarks/idiom3_ex1.out
python3 learning_idiom3.py