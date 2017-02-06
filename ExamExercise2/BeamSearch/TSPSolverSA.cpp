#include "TSPSolverSA.h"
#include <algorithm>    // std::min
#include <cmath>
using namespace std;


bool TSPSolverSA::solve ( const TSP& tsp , const TSPSolution& initSol , TSPSolution& bestSol, const int T_0, const int maxIter) {
	TSPSolution currSol(initSol);
	bestSol = initSol;
	int acceptedWorseningSolution = 0;
    srand(time(NULL));
    double T = 500;
    double MIN_TEMPERATURE = 1.0;
    int iterationsAtConstantT = 0;
	while(T > MIN_TEMPERATURE){
		if(iterationsAtConstantT == 30){
			iterationsAtConstantT = 0;
			T = T * 0.95;
		}
		iterationsAtConstantT++;
		bool isToReset = true;
		
		/* Finding a random neighbour by swapping 2 elements */
		int a = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
		int b = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
		
		cout << a << "," << b << endl;
		currSol.print();
		currSol = currSol.swap(tsp, a, b);
		currSol.print();
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
	return true;
	
}

