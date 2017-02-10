#!/bin/bash

##
## Argument 1 = function to execute Argument 2 = instance
##


function execute_ten_times_and_compute_average {	
	avg=0
	echo "# Exec " >> /tmp/benchmark_value.txt
	for j in {1..10}; do 
		echo "Doing ${i} - ${j}"
		tmp=($(${1} ${2} ${3})) # >> benchmark_circle.txt
		
		echo ${tmp[1]}
		echo ${tmp[2]} >> /tmp/benchmark_value.txt
		avg=`echo "scale=6; $avg + ${tmp[1]}" | bc`
	doneq
	
	avg=`echo "scale=6; $avg/10" | bc`
	echo $avg
}


avgTSP12=`execute_ten_times_and_compute_average ./ExamExercise2/Genetic/main ./Sample/tsp12.dat -b `
echo $avgTSP12
avgTSP60=`execute_ten_times_and_compute_average ./ExamExercise2/Genetic/main ./Sample/tsp60.dat -b`
echo $avgTSP60
avgRealWorld=`execute_ten_times_and_compute_average ./ExamExercise2/Genetic/main ./Sample/RealWorldExample.dat -b`
echo $avgRealWorld


#rm tsp_instance_*_random.dat
#rm tsp_instance_*_cirlce.dat
#rm benchmark_result
#rm benchmark_random.txt
#rm benchmark_circle.txt

#for i in $array; do
#	echo "Generating random instance with ${i} elements"
#	InstanceGenerator/instance_generator ${i} r 
#	echo "Generating instance with ${i} elements divided in 4 circles"
#	InstanceGenerator/instance_generator ${i} c
#done

#for i in $array; do
#	echo "Generating instance with ${i} elements divided in 4 circles"
#	./InstanceGenerator/instance_generator ${i} r
#done

#array=`seq 40 40`
#for i in $array; do 
	#echo "Processing ${i} random"
	#ExamExercise/main /tmp/tsp_instance_${i}_random.dat -b >> benchmark_random.txt
#	echo "Processing ${i} circle"
	
#	avg=`execute_ten_times_and_compute_average ./ExamExercise2/Genetic/main /tmp/tsp_instance_${i}_circle.dat -b`
	
#	echo "Average = $avg"
#	echo "${i} ${avg}" >> /tmp/benchmark_circle_genetic.txt
#	
#done 

#echo "output wrote in files : benchmark_random.txt, benchmark_circle.txt"
echo "Done"
