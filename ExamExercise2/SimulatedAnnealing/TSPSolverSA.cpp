#include "TSPSolverSA.h"
#include <algorithm>    // std::min
#include <cmath>
using namespace std;

/* Simulated Annealing */
bool TSPSolverSA::solve ( const TSP& tsp , const TSPSolution& initSol , TSPSolution& bestSol, const double start_temperature, const int max_it=30) {
	TSPSolution currSol(initSol);
	bestSol = initSol;
	int acceptedWorseningSolution = 0;
    double T = start_temperature;
    double MIN_TEMPERATURE = 1.0;
    int iterationsAtConstantT = 0;
	int it = 0;
	//Repeating the process untill reaching the temperature
	while(T > MIN_TEMPERATURE){
		if(iterationsAtConstantT == max_it){
			iterationsAtConstantT = 0;
			T = T * 0.95;
		}
		it++;
		iterationsAtConstantT++;
		bool isToReset = true;
		
		/* Finding a random neighbour by swapping 2 elements */
		int a = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
		int b = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
		while(a == b){ //iterate as long as a = b
			b = 1 + (rand() % (tsp.n-1));
		}
		
		currSol = currSol.swap(tsp, a, b);
		
		//Accept it unconditionally
		if(currSol.value <= bestSol.value){
			bestSol = currSol;
			
		} else {
			//Accept it only if 
			
			
			double p = min(1.0, exp(-(currSol.value - bestSol.value)/T));
			
			
			double r = ((double) rand() / (RAND_MAX));
			
			
			
			if(r <= p){
				acceptedWorseningSolution++;
				isToReset = false;
			} 
		}
		if(isToReset){
			currSol = bestSol;
		}
		
	}
	cout << "Worsening Solutions accepted = " << acceptedWorseningSolution << endl;
	cout << "TOTAL ITERATION" << it << endl;
	return true;
	
}

