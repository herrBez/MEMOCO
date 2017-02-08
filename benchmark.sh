#!/bin/bash
echo "Benchmark.."
array=`seq 1 48`

rm tsp_instance_*_random.dat
rm tsp_instance_*_square.dat
rm benchmark_result
rm benchmark_random.txt
rm benchmark_circle.txt

for i in $array; do
	echo "Generating random instance with ${i} elements"
	InstanceGenerator/instance_generator ${i} r 
	echo "Generating instance with ${i} elements divided in 4 squares"
	InstanceGenerator/instance_generator ${i} c
done

for i in $array; do 
	echo "Processing ${i} random"
	ExamExercise/main /tmp/tsp_instance_${i}_random.dat -b >> benchmark_random.txt
	echo "Processing ${i} circle"
	ExamExercise/main /tmp/tsp_instance_${i}_circle.dat -b >> benchmark_circle.txt
done 

echo "output wrote in files : benchmark_random.txt, benchmark_square.txt"
echo "Done"
