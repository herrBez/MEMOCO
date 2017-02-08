#include "TSPSolverGA.h"
#include <algorithm>    // std::min
#include <cmath>
#include <numeric>
using namespace std;


TSPSolution * TSPSolverGA::ricombination(TSP tsp, vector< TSPSolution > population, int * parents, int R){
	TSPSolution * newGen = new TSPSolution[R];
	for(int i = 0; i < R; i+=2){ 
		newGen[i].reserve(tsp);
		newGen[i+1].reserve(tsp);
		int parent1 = parents[i];
		int parent2 = parents[i+1];
		//Selecting two different cut points for order crossover
		int rI = (rand() % (tsp.n-2))+1;
		int rII;
		while((rII = (rand() % (tsp.n-2))+1) == rI){
			rII = (rand() % (tsp.n-2))+1;
		}
		int start = min(rI, rII);
		int end = max(rI, rII);
		getOffSpring(tsp, newGen[i], population[parent1], population[parent2], start, end);
		getOffSpring(tsp, newGen[i+1], population[parent2], population[parent1], start, end);
		if(rand() % 100 < 10){//Probability 20%
			localSearch(tsp, newGen[i]);
		}
		if(rand() % 100 < 10) {//Probability 20%
			localSearch(tsp, newGen[i+1]);
		}
		
	}
	return newGen;
}


int * TSPSolverGA::selection(vector < TSPSolution > currPopulation, int R, SelectionMethod sMethod) {
	int * parent = new int[R];
	/* If Monte-Carlo is used */
	double sum = 0.0;
	if(sMethod == SelectionMethod::MONTE_CARLO){
		sum = accumulate(currPopulation.begin(), currPopulation.end(), 0.0, 
									[](double a, TSPSolution b) {
											return a + b.fitness;
									});
	}
		
	for(int i = 0; i < R; i+=2){
		parent[i] = getParent(currPopulation, sum, sMethod);
		parent[i+1] = parent[i]; //Assuring that parent2 is different from parent 1
		while(parent[i+1] == parent[i]){
			parent[i+1]=getParent(currPopulation, sum, sMethod);
		}
	}
		
	return parent;
}

int TSPSolverGA::getParent(vector < TSPSolution > population, double sum, SelectionMethod sMethod){
	switch(sMethod){
		case SelectionMethod::N_TOURNAMENT:
			return nTournamentSelection(population, 4);
			
		case SelectionMethod::LINEAR_RANKING:
			
			return  linearRankingSelection(population, true);
		case SelectionMethod::MONTE_CARLO:
			
		default: return monteCarloSelection(population, &sum);
	}
}





bool TSPSolverGA::solve ( const TSP& tsp , vector< TSPSolution > & currPopulation, TSPSolution& bestSol, unsigned int groupSize,
						   unsigned int variation) {
	
	unsigned int N = currPopulation.size();	
	
	int nonImprovingIterations = 0;
	int R = ceil(sqrt(N));
	if(R % 2 == 1)
		R++;
	double bestValue = currPopulation[0].value;
	int it = 0;
	while(nonImprovingIterations < 500) { //Stopping criterion
		it++;
		
		/* Select R parents */	
		int * setOfParents = selection(currPopulation, R, SelectionMethod::MONTE_CARLO);
		/* Returns the new generation, composed of R new individuals. It perform also the mutation*/
		TSPSolution * newGen = ricombination(tsp, currPopulation, setOfParents, R);
		/* Perform some mutation in order to maintain the diversity */
		mutation(newGen, tsp, R, 0.1);
		/* Perform a local search on some randomly choosen individuals */
		train(newGen, tsp, R, 0.001);
		
		/* Add the children to the population */
		for(int i = 0; i < R; i++){
			currPopulation.push_back(newGen[i]);
		}
		
		/* Sort the population */		
		if(R < (int) N/3) 
			insertionSortPopulation(currPopulation);
		else 
			sortPopulation(currPopulation);
		
		/* removing the worst values */
		while(currPopulation.size() != N){
			currPopulation.pop_back();
		}
		
		 nonImprovingIterations++;
		 if(bestValue != currPopulation[0].value){
			bestValue = currPopulation[0].value;
			nonImprovingIterations = 0;
		 }
		 delete[] newGen;
		 delete[] setOfParents;
	}
	bestSol = currPopulation[0];
	return true;
}

