#!/bin/bash
./order_generator.out
../../../pin -t obj-intel64/scheduler_rl_v2.dylib -- ./idioms/idiom2_ex1.out
python3 learning_idiom2.py