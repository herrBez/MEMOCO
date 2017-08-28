#ifndef __TSP_SOLVER_SIMULATED_ANNEALING__H__
#define __TSP_SOLVER_SIMULATED_ANNEALING__H__
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <map>
#include "TSPSolution.h"

enum SelectionMethod{
	MONTE_CARLO,
	LINEAR_RANKING,
	N_TOURNAMENT,
};



using namespace std;

class TSPSolverGA {
private:

	void localSearch(TSP tsp, TSPSolution & sol, int it=1000){
		TSPSolution currSol (sol);
		TSPSolution tmpSol (sol);
		for(int i = 0; i < it; i++){
			tmpSol = currSol;
			/* Finding a random neighbour by swapping 2 elements */
			int a = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
			int b = 1 + (rand() % (tsp.n-1)); // Index between [1, n-1]
			tmpSol.swap(tsp, a, b);
			if(tmpSol.value < currSol.value)
				currSol = tmpSol;
		}
		sol = currSol;
	}
	
	void train(TSPSolution * sol, TSP tsp, const int R, double probability = 0.1){
		for(int i = 0; i < R; i++){
			//double p = ((double) rand()) / RAND_MAX;
			//if(p <= probability){
				localSearch(tsp, sol[i]);
			//}
		}
	}
	
	/**
	 * performs the substitution of the population preserving the best N elements if it is an intensification phase
	 * otherwise it maintain the better 2 solutions and choose randomly the elements to keep.
	 * @param currPopulation the current population
	 * @param N the size of the population to keep
	 * @param intensification true if it is an intensification phase, false otherwise 
	 */
	void substitution(vector < TSPSolution > & currPopulation, unsigned int N, bool intensification);
	
	void mutation(TSPSolution * sol, TSP tsp, int R, double probability = 0.001){
		for(int i = 0; i < R; i++){
			double r = ((double)rand()) / RAND_MAX;
			for(int j = 0; j < tsp.n; j++){
				if(r <= probability){
					int a = (rand() % (tsp.n-2))+1;
					int b = (rand() % (tsp.n-2))+1;
					sol[i].swap(tsp, a, b);
				}
			}
		}
	}
	/**
	 * perfrom a montecarlo selection 
	 * p_i := f_i / sum_{k=0}^{N} f_k
	 * @param population
	 * @return the (population) index of the selected element
	 */
	int monteCarloSelection(vector < TSPSolution > population, void * _sum) {
		double r1 = (double) rand() / RAND_MAX;
		int parent = -1;
		double sum = *(double*)_sum;
		double prev_val = 0.0;
		double val = population[0].fitness/sum; 
		for(unsigned int i = 0; i < population.size(); i++){
			if(prev_val < r1 && r1 <= val) {
				parent = i;
				break;
			} 
			prev_val = val;
			val += (population[i].fitness/sum);
		}
		return parent;
	}
	/**
	 * perfrom a linear ranking  selection 
	 * p_i := 2*sigma_i / N(N+1)
	 * @param population
	 * @param sorted a flag that indicate if population is already sorted
	 * @return the (population) index of the selected element
	 */
	int linearRankingSelection(vector < TSPSolution > population, bool sorted = false) {
		double r1 = (double) rand() / RAND_MAX;
		
		if(!sorted)
			sortPopulation(population);
			
		int parent = -1;
		double * prob = new double[population.size()];
		int N = population.size();
		for(int i = 0; i < N; i++){
			prob[i] = (2.0*(N-i)) / (N*(N+1));
		}
		double prev_val = 0.0;
		double val = prob[0];
		for(unsigned int i = 0; i < population.size(); i++){
			if(prev_val < r1 && r1 <= val) {
				parent = i;
				break;
			}
			prev_val = val;
			val += (prob[i]);
		}
		delete[] prob;
		return parent;
	}
	/**
	 * perform a k-tournament selection. It selects randomly k elements from the population
	 * and return the best one
	 * @param population 
	 * @param k the size of the population
	 */
	int nTournamentSelection(vector < TSPSolution > population, int k = 2) {

		int N = population.size();
		int parent = -1;
		int * pool = new int[k];
		
		bool * alreadySelected = new bool[N];
		for(int i = 0; i < N; i++){
			alreadySelected[i] = false;
		}
		for(int i = 0; i < k; i++){
			int r = rand() % N;
			while(alreadySelected[r]){
				r = rand() % N;
			}
			alreadySelected[r] = true;
			pool[i] = r;
		}
		int best_index = pool[0];
		double best_fitness = population[pool[0]].fitness;
		for(int i = 1; i < k; i++){
			if(best_fitness < population[pool[i]].fitness){
				best_index = pool[i];
				best_fitness = population[pool[i]].fitness;
			}
		}
		parent = best_index;
	
		
		delete[] alreadySelected;
		delete[] pool;
		return parent;
	}

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
	/**
	 * print the given population. (Debug)
	 * @param population the population to print
	 */
	void printPopulation(vector< TSPSolution > population) {
		std::for_each(population.begin(), population.end(), [] (TSPSolution p)
		{
			p.print();
			cout << endl;
		});
	}
	
	/**
	 * sort the population using the std::sort method.
	 * It should be quicksort and therefore is not well-suited in case
	 * the population is already sorted.
	 * @param population the population to sort
	 */
	void sortPopulation(vector < TSPSolution > & population) {
		sort(population.begin(), population.end(), [] (const TSPSolution a, const TSPSolution b) {
			return a.fitness > b.fitness;
		});
	}
	
	/**
	 * sort the population using insertionSort. This method is well-suited
	 * in case the array is almost sorted. It is used when 2 children are added
	 * to the current population.
	 * @param population the population to sort
	 */
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


	/**
	 * return a parent chosen with the montecarlo selection.
	 * Sum is the sum of the fitnesses of the population.
	 * @param population
	 * @param R used only for N-tournament
	 * @param sum
	 */
	int getParent(vector < TSPSolution > population , int R, double sum, SelectionMethod s = SelectionMethod::MONTE_CARLO);
	/**
	 * performs the desired selection depending on the SelectionMethod given as parameter
	 * @param population
	 * @param R the number of parents to select (it is a multiple of 2)
	 * @param s the selection method - default MONTE_CARLO
	 * @return an array containing the indexes (referring to the population) of the parents
	 */
	int * selection(vector < TSPSolution > population, int R, SelectionMethod s = SelectionMethod::MONTE_CARLO);
	
	
	/**
	 * perfroms the ricombination
	 * @param tsp the instance of the tsp problem
	 * @param population 
	 * @param parents array generated by the selection function with the indexes of the parents
	 */
	TSPSolution * ricombination(TSP tsp, vector< TSPSolution > population, int * parents, int R);

	/**
	 * get an init population
	 * Idea: in order to guarantee "some initial" richness. I create
	 * a sequential instance from <0 1 ... n 0> and another
	 * <0 n ... 1 0> and then N-2 random instances
	 * @param tsp
	 * @param population the population to create
	 * @param N the cardinality of the population
	 */
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
	/**
	 * perform an order crossover
	 * @param tsp the problem instance
	 * @param child the child that is generated by the crossover
	 * @param parent1 the first parent
	 * @param parent2 the second parent
	 * @param start the first cut point
	 * @param end the second cut point
	 */
	void getOffSpring(TSP tsp, TSPSolution & child, TSPSolution& parent1,  TSPSolution& parent2, 
			int start, int end) {
		
		
		child = parent1;
		/* 
		 * this data-structure is sorted by the key, i.e. the first element 
		 * insert cost O(log n)
		 */
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
		
		int i = start;
		for (std::map<int,int>::iterator it=indexes.begin(); it!=indexes.end(); ++it)
			child.sequence[i++] = it->second;
		
		child.value = evaluate(child, tsp);
		
		if((double) rand() / RAND_MAX <= 1){
			
			int a = (rand() % (child.sequence.size() - 2)) + 1;
			int b = (rand() % (child.sequence.size() - 2)) + 1;
			child.swap(tsp, a, b);
		}
		
		child.fitness = 1.0 / child.value;

		
	}

	/**
	 * 
	 * @param TSP TSP data
	 * @param currSol current solution
	 * @param bestSol best found solution (output)
	 * @param R the number of new solutions generated in each iteration
	 * @return true id everything OK, false otherwise
	 */
	 bool solve ( const TSP& tsp , vector< TSPSolution > & currPopulation, TSPSolution& bestSol);

};

#endif /* END __TSP_SOLVER_SIMULATED_ANNEALING__H__ */
