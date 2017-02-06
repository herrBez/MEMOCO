/**
* @file TSPSolution.h
* @brief TSP solution
*
*/

#ifndef TSPSOLUTION_H
#define TSPSOLUTION_H

#include <vector>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include "TSP.h"
using namespace std;

/**
* TSP Solution representation: ordered sequence of nodes (path representation)
*/
class TSPSolution {
 public:
  std::vector<int> sequence;
  double fitness;
  double value;
 public:
	/**
	 * returns an instance of a TSPSolution
	 */
	TSPSolution(){
	}
  
	/**
	 * check whether the solution is feasible or not, used for debug purposes and test
	 * in order to check if it was all OK.
	 * @return true if this istance is feasible, false otherwise
	 */
	bool isFeasible(){
		vector <int> v = sequence;
		sort(v.begin(), v.end());
		if(v[0] != 0 || v[1] != 0)
			return false;
		for(uint i = 2; i < v.size(); i++){
			if(v[i] != (int) i-1)
				return false;
		}
		return true;
	}

  
	  /** build a standard solution as the sequence <0, 1, 2, 3 ... n-1, 0>
		* @param tsp TSP instance
		* @return the created TSP instance
		*/
	  TSPSolution(const TSP& tsp) {
		sequence.reserve(tsp.n + 1);
		for (int i = 0; i < tsp.n; ++i) {
		  sequence.push_back(i);
		}
		sequence.push_back(0);
	  }
	  /** Copy constructor
		* build a solution from another
		* @param tspSol TSP solution
		* @return ---
		*/
	  TSPSolution(const TSPSolution& tspSol) {
		sequence.reserve(tspSol.sequence.size());
		for (uint i = 0; i < tspSol.sequence.size(); ++i) {
		  sequence.push_back(tspSol.sequence[i]);
		}
		fitness = tspSol.fitness;
		value = tspSol.value;
	  }

 public:
	  /** print the TSPSolution
		*/
	  void print(void) {
		for (uint i = 0; i < sequence.size(); i++) {
		  printf("%2d ", sequence[i]);
		}
		cout << " v = " << value << " f = " << fitness;
	  }
  /** 
   * perform a deep copy of the given TSPSolution. (Overload operator '=' for this class)
   * @param right TSP solution to get into
   * @return ---
   */
  TSPSolution& operator=(const TSPSolution& right) {
    // Handle self-assignment:
    if (this == &right) return *this;
    for (uint i = 0; i < sequence.size(); i++) {
      sequence[i] = right.sequence[i];
    }
    fitness = right.fitness;
    value = right.value;
    return *this;
  }
  
  
};

#endif /* TSPSOLUTION_H */
