#ifndef __TSP_SOLVER_SIMULATED_ANNEALING__H__
#define __TSP_SOLVER_SIMULATED_ANNEALING__H__
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <map>
#include "TSPSolution.h"

#define MONTE_CARLO 0

using namespace std;

class TSPSolverGA {
public:
	/** Constructor */
	TSPSolverGA() {
	}
	/**
	 * evaluate a solution
	 * @param sol: solution to be evaluated
	 * @param TSP: TSP data
	 * @return the value of the solution
	 */
	double evaluate(const TSPSolution& sol, const TSP& tsp) const {
		double total = 0.0;
		for (uint i = 0; i < sol.sequence.size() - 1; ++i) {
			int from = sol.sequence[i];
			int to = sol.sequence[i + 1];
			total += tsp.cost[from][to];
		}
		return total;
	}

	void printPopulation(vector< TSPSolution > population) {
		std::for_each(population.begin(), population.end(), [] (TSPSolution p)
		{
			p.print();
			cout << endl;
		});
	}
	void sortPopulation(vector < TSPSolution > & population) {
		sort(population.begin(), population.end(), [] (const TSPSolution a, const TSPSolution b) {
			return a.fitness > b.fitness;
		});
	}
	void insertionSortPopulation(vector < TSPSolution > & population){
		for (uint i = 0; i < population.size(); i++){
			uint j = i;
		
			while (j > 0 && population[j].fitness > population[j-1].fitness){
				TSPSolution temp(population[j]);
				population[j] = population[j-1];
				population[j-1] = temp;
				j--;
			}
		}
	}

	int getParent(vector < TSPSolution > population , double sum);
	
	
	
	bool getInitPopulation(TSP tsp, std::vector<TSPSolution> & population,
			int N) {
		
		/* 0 1 2 .. n-1 0 */
		TSPSolution tmp(tsp);
		tmp.value = evaluate(tmp, tsp);
		tmp.fitness = 1.0/tmp.value;
		population.push_back(tmp);
		for (int i = 1; i < N-1; i++) {

			initRnd(tmp);
			tmp.value = evaluate(tmp, tsp);
			tmp.fitness = 1.0 / tmp.value;
			population.push_back(tmp);

		}
		
		/* 0 n-1 .. 2 1 0 */
		int len = tmp.sequence.size()-1;
		for(int i = 0; i < len; i++){
			tmp.sequence[len-i] = i;
		}
		tmp.value = evaluate(tmp, tsp);
		tmp.fitness = 1.0/tmp.value;
		population.push_back(tmp);
		
		sortPopulation(population);

		return true;
	}

	/**
	 * initialize a solution as a random sequence by random swaps
	 * @param sol solution to be initialized
	 * @return true if everything OK, false otherwise
	 */
	bool initRnd(TSPSolution& sol) {
		
		for (uint i = 1; i < sol.sequence.size(); ++i) {
			// intial and final position are fixed (initial/final node remains 0)
			int idx1 = rand() % (sol.sequence.size() - 2) + 1;
			int idx2 = rand() % (sol.sequence.size() - 2) + 1;
			int tmp = sol.sequence[idx1];
			sol.sequence[idx1] = sol.sequence[idx2];
			sol.sequence[idx2] = tmp;
		}

		//std::cout << "### "; sol.print(); std::cout << " ###" << std::endl;
		return true;
	}
	/* Order Crossover */
	void getOffSpring(TSP tsp, TSPSolution & child, TSPSolution& parent1,  TSPSolution& parent2, 
			int start, int end) {
		#ifdef VERBOSE
		cout << "OFFSPRING GENERATION -- ORDERED CROSSOVER " << start << " " << end  << endl;
		
		parent1.print();
		cout << endl;
		parent2.print();
		cout << endl;
		cout << "----------------------" << endl;
		#endif
		
		child = parent1;
		std::map< int, int > indexes;
		
		
		
		for(int i = 0; i < end-start+1; i++){
			int val = parent1.sequence[start+i];
			
			int key = 0;
			for(unsigned int j = 0; j < parent2.sequence.size(); j++){
				if(val == parent2.sequence[j]) {
					key = j;
					break;
				}
			} 
			indexes.insert(make_pair(key, val));
		}
		#ifdef SUPER_VERBOSE
		std::for_each(indexes.begin(), indexes.end(), [] (std::pair<int, int> a) 
		{ 
			cout << "(" << a.first << "," << a.second <<")" << endl;
		});
		#endif
		int i = start;
		for (std::map<int,int>::iterator it=indexes.begin(); it!=indexes.end(); ++it)
			child.sequence[i++] = it->second;
		
		if((double) rand() / RAND_MAX <= 1){
			#ifdef VERBOSE
			cout << "MUTATION" << endl;
			#endif
			int a = (rand() % (child.sequence.size() - 2)) + 1;
			int b = (rand() % (child.sequence.size() - 2)) + 1;
			int tmp = child.sequence[a];
			child.sequence[a] = child.sequence[b];
			child.sequence[b] = tmp;
		}
		
		#ifdef VERBOSE
		cout << endl;
		child.print();
		cout << endl;
		cout << endl;
		cout << "END -- ORDERED CROSSOVER " << endl;
		#endif
		child.value = evaluate(child, tsp);
		/* Calculating the value from the father. Without calculating the whole sum (end-start) < N 
		for (int i = 0; i <= (end - start + 1); i++) {
			child.value -= tsp.cost[parent1.sequence[start + i - 1]][parent1.sequence[start	+ i]];
			child.value += tsp.cost[child.sequence[start + i - 1]][child.sequence[start + i]];
		}*/
		child.fitness = 1.0 / child.value;
	}

	/**
	 * search for a good tour by neighbourhood search
	 * @param TSP TSP data
	 * @param initSol initial solution
	 * @param bestSol best found solution (output)
	 * @return true id everything OK, false otherwise
	 */
	 bool solve ( const TSP& tsp , vector< TSPSolution > & currPopulation, TSPSolution& bestSol, unsigned int groupSize = 4,
						   unsigned int variation = 10);

};

#endif /* END __TSP_SOLVER_SIMULATED_ANNEALING__H__ */
