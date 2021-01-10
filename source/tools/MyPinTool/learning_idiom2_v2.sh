#!/bin/bash
for i in {1..100}
do
   ./order_generator.out
	../../../pin -t obj-intel64/scheduler_rl_v2.dylib -- ./idioms/idiom2_ex1.out
	python3 learning_idiom2_v2.py
done