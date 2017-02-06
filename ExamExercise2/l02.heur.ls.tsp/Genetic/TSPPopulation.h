#ifndef __TSP_POPULATION__H__
#define __TSP_POPULATION__H__
#include "TSPSolution.h"
#include "TSPSolverGA.h"
using namespace std;

/**
 * Class that models the population of solution.
 * Substantially it is a wrapper to a vector of TSPSolution.
 */
class TSPPopulation
{
public:
	vector < TSPSolution > member;
	
	/* Get an init random population */
	TSPPopulation(TSP & tsp, TSPSolverGA tspga, int N){
		
		TSPSolution tmp(tsp);		
		for(int i = 0; i < N; i++){
			tmp.initRnd(tsp);

			member.push_back(tmp);
		}
	}
	
	
	
	/**
	 * prints the population (used only for debug purposes
	 */
	void printPopulation();

};
#endif //__TSP_POPULATION__H__
