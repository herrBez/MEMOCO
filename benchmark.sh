#!/bin/bash
echo "Benchmark.."
array=(16 20 25 30 35 40)

rm tsp_instance_*_random.dat
rm tsp_instance_*_square.dat
rm benchmark_result

for i in ${array[@]}; do
	echo "Generating random instance with ${i} elements"
	InstanceGenerator/instance_generator ${i} r 
	echo "Generating instance with ${i} elements divided in 4 squares"
	InstanceGenerator/instance_generator ${i} q 
done

for i in ${array[@]}; do 
	ExamExercise/main tsp_instance_${i}_random.dat -b /tmp/benchmark_result_${i}_random.txt
	ExamExercise/main tsp_instance_${i}_square.dat -b /tmp/benchmark_result_${i}_random.txt
done 

echo "Done"
