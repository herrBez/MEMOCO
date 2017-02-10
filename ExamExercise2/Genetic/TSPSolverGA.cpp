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
		int rII = rI;
		if(tsp.n < 3)
		while((rII = (rand() % (tsp.n-2))+1) == rI){
			rII = (rand() % (tsp.n-2))+1;
		}
		int start = min(rI, rII);
		int end = max(rI, rII);
		getOffSpring(tsp, newGen[i], population[parent1], population[parent2], start, end);
		getOffSpring(tsp, newGen[i+1], population[parent2], population[parent1], start, end);
		
		
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
		parent[i] = getParent(currPopulation, R, sum, sMethod);
		parent[i+1] = parent[i]; //Assuring that parent2 is different from parent 1
		while(parent[i+1] == parent[i]){
			parent[i+1]=getParent(currPopulation, R, sum, sMethod);
		}
	}
		
	return parent;
}

int TSPSolverGA::getParent(vector < TSPSolution > population, int R, double sum, SelectionMethod sMethod){
	switch(sMethod){
		case SelectionMethod::N_TOURNAMENT:
			return nTournamentSelection(population, R);
			
		case SelectionMethod::LINEAR_RANKING:
			
			return  linearRankingSelection(population, true);
		case SelectionMethod::MONTE_CARLO:
			
		default: return monteCarloSelection(population, &sum);
	}
}


void TSPSolverGA::substitution(vector < TSPSolution > & currPopulation, unsigned int N, bool intensification){
	if(intensification){
		/* removing the worst values */
		while(currPopulation.size() != N){
			currPopulation.pop_back();
		}
	} else {
		int maintainGoodSolution = 2;
		while(currPopulation.size() != N){
			currPopulation.erase(currPopulation.begin() + maintainGoodSolution + (rand()%currPopulation.size()));
		}
	}	
	
}


bool TSPSolverGA::solve ( const TSP& tsp , vector< TSPSolution > & currPopulation, TSPSolution& bestSol,
						   int R) {
	
	unsigned int N = currPopulation.size();	
	if(tsp.n <= 3){
		localSearch(tsp, currPopulation[0], 100);
		bestSol = currPopulation[0];
		return true;
	}
	int nonImprovingIterations = 0;
	R = (int)ceil(sqrt(N));
	if(R % 2 == 1) // Render R multiple of 2
		R++;

	double bestValue = currPopulation[0].value;

	int it = 0;
	bool intensification = false;
	int intensification_counter = 0;
	while(nonImprovingIterations < 500) { //Stopping criterion
		it++;
		int * setOfParents = NULL;
		if(intensification){
			/* Select R parents */	
			setOfParents = selection(currPopulation, R, SelectionMethod::MONTE_CARLO);
		} else {
			setOfParents = selection(currPopulation, R, SelectionMethod::N_TOURNAMENT);
		}
		
		/* Returns the new generation, composed of R new individuals.*/
		TSPSolution * newGen = ricombination(tsp, currPopulation, setOfParents, R);
		
		if(!intensification){
			/* Perform some mutation in order to maintain the diversity */
			mutation(newGen, tsp, R, 0.2);
		} else {
			
			mutation(newGen, tsp, R, 0.001);
			/* Perform a local search on some randomly choosen individuals */
			train(newGen, tsp, R, 0.02);
			
		}
		/* Add the children to the population */
		for(int i = 0; i < R; i++){
			currPopulation.push_back(newGen[i]);
		}
		
		/* Sort the population - Insertion sort has good performance when it is already sorted
		 * It is based upon the assumption that R << N*/		
		insertionSortPopulation(currPopulation);
		
		substitution(currPopulation, N, intensification);
		
		nonImprovingIterations++;
		if(bestValue != currPopulation[0].value){
			bestValue = currPopulation[0].value;
			nonImprovingIterations = 0;
		}
		if(nonImprovingIterations == 499){
			nonImprovingIterations = 0;
			intensification = !intensification;
			intensification_counter++;
		}
		delete[] newGen;
	    delete[] setOfParents;
	    if(intensification_counter == 2)
			break;
	}
	localSearch(tsp, currPopulation[0],1000);
	bestSol = currPopulation[0];
	return true;
}

