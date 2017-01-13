#include "TSPSolverGA.h"
#include <algorithm>    // std::min
#include <cmath>
#include <numeric>
using namespace std;


int TSPSolverGA::getParent(vector < TSPSolution > population, double sum){
	double r1 = (double) rand() / RAND_MAX;
	
	int parent = -1;
	double prev_val = 0.0;
	/* LINEAR RANKING */



	/* MONTE CARLO SELECTION */
	double val = population[0].fitness/sum; 
	for(unsigned int i = 0; i < population.size(); i++){
		if(prev_val < r1 && r1 <= val) {
			parent = i;
		} 
		prev_val = val;
		val += (population[i].fitness/sum);
	}
	return parent;
}

bool TSPSolverGA::solve ( const TSP& tsp , vector< TSPSolution > & currPopulation, TSPSolution& bestSol, unsigned int groupSize,
						   unsigned int variation) {
	
	unsigned int N = currPopulation.size();	
	
	int nonImprovingIterations = 0;
	unsigned int R = groupSize;
	double bestValue = currPopulation[0].value;
	while(nonImprovingIterations < 500) {
		
		//sum_{k=1}^{N} f_k
		double sum = accumulate(currPopulation.begin(), currPopulation.end(), 0.0, 
										[](double a, TSPSolution b) {
												return a + b.fitness;
										});
		
		/* Generating R children */
		for(unsigned int i = 0; i < R; i+=2){
			int parent1 = getParent(currPopulation, sum);
			int parent2 = parent1;
			while(parent2 == parent1){
				parent2=getParent(currPopulation, sum);
			}
			TSPSolution offspring1(tsp), offspring2(tsp);
			int rI = (rand() % (tsp.n-2))+1;
			int rII;
			while((rII = (rand() % (tsp.n-2))+1) == rI){
				rII = (rand() % (tsp.n-2))+1;
			}
			
			int start = min(rI, rII);
			int end = max(rI, rII);
			getOffSpring(tsp, offspring1, currPopulation[parent1], currPopulation[parent2], start, end);
			getOffSpring(tsp, offspring2, currPopulation[parent2], currPopulation[parent1], start, end);
		
			currPopulation.push_back(offspring1);
			currPopulation.push_back(offspring2);
		}
		
	
		
		
		insertionSortPopulation(currPopulation);
		
		
		while(currPopulation.size() != N){
			currPopulation.pop_back();
		}
		
		
		/*#ifdef VERBOSE
			std::for_each(currPopulation.begin(), currPopulation.end(), [sum] (TSPSolution p) 
			{ 
				p.print();
				cout << endl;
				
			});
		 #endif*/
		 nonImprovingIterations++;
		 if(bestValue != currPopulation[0].value){
			bestValue = currPopulation[0].value;
			nonImprovingIterations = 0;
		 }
	}
	bestSol = currPopulation[0];
	return true;
}

