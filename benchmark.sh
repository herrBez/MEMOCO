#!/bin/bash
echo "Benchmark.."
array=`seq 3 48`

#rm tsp_instance_*_random.dat
#rm tsp_instance_*_cirlce.dat
#rm benchmark_result
#rm benchmark_random.txt
#rm benchmark_circle.txt

#for i in $array; do
#	#echo "Generating random instance with ${i} elements"
#	#InstanceGenerator/instance_generator ${i} r 
#	echo "Generating instance with ${i} elements divided in 4 circles"
#	InstanceGenerator/instance_generator ${i} c
#done

for i in {49..50}; do
	echo "Generating instance with ${i} elements divided in 4 circles"
	InstanceGenerator/instance_generator ${i} c
done

echo "set terminal png"
for i in {49..50}; do 
	#echo "Processing ${i} random"
	#ExamExercise/main /tmp/tsp_instance_${i}_random.dat -b >> benchmark_random.txt
	echo "Processing ${i} circle"
	
	
	avg=0
	num=10
	for j in {0..10}; do 
		echo "Doing ${i} - ${j}"
		tmp=( $(./ExamExercise/main /tmp/tsp_instance_${i}_circle.dat -b) ) # >> benchmark_circle.txt
		echo ${tmp[1]}
		
		avg=`echo "scale=6; $avg + ${tmp[1]}" | bc`
	done
	avg=`echo "scale=6; $avg/$num" | bc`
	echo "Average = $avg"
	echo "${i} ${avg}" >> /tmp/benchmark_circle.txt
	
done 

echo "output wrote in files : benchmark_random.txt, benchmark_circle.txt"
echo "Done"
