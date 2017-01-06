/**
* @file TSPSolution.h
* @brief TSP solution
*
*/

#ifndef TSPSOLUTION_H
#define TSPSOLUTION_H

#include <vector>

#include "TSP.h"

/**
* TSP Solution representation: ordered sequence of nodes (path representation)
*/
class TSPSolution
{
public:
  std::vector<int>		sequence;
  double value;
public:
  /** Constructor 
  * build a standard solution as the sequence <0, 1, 2, 3 ... n-1, 0>
  * @param tsp TSP instance
  * @return ---
  */
  TSPSolution( const TSP& tsp ) {
    sequence.reserve(tsp.n + 1);
    for ( int i = 0; i < tsp.n ; ++i ) {
      sequence.push_back(i);
    }
    sequence.push_back(0);
    value = 0;
  }
  /** Copy constructor 
  * build a solution from another
  * @param tspSol TSP solution
  * @return ---
  */
  TSPSolution( const TSPSolution& tspSol ) {
    sequence.reserve(tspSol.sequence.size());
    for ( uint i = 0; i < tspSol.sequence.size(); ++i ) {
      sequence.push_back(tspSol.sequence[i]);
    }
    value = tspSol.value;
  }
public:
  /** print method 
  * @param ---
  * @return ---
  */
  void print ( void ) {
	  
    for ( uint i = 0; i < sequence.size(); i++ ) {
      std::cout << sequence[i] << " ";
    }
    std::cout << "v=" << value << std::endl;
  }
  /** assignment method 
  * copy a solution into another one
  * @param right TSP solution to get into
  * @return ---
  */
  TSPSolution& operator=(const TSPSolution& right) {
    // Handle self-assignment:
    if(this == &right) return *this;
    for ( uint i = 0; i < sequence.size(); i++ ) {
      sequence[i] = right.sequence[i];
    }
    value = right.value;
    return *this;
  }
  
  TSPSolution& swap(const TSP & tsp, int a, int b){
	  value = value - tsp.cost[sequence[a-1]][sequence[a]];
	  value = value - tsp.cost[sequence[a]][sequence[a+1]];
	  value = value - tsp.cost[sequence[b-1]][sequence[b]];
	  value = value - tsp.cost[sequence[b]][sequence[b+1]];
	  int tmp = sequence[a];
	  sequence[a] = sequence[b];
	  sequence[b] = tmp;
	  value = value + tsp.cost[sequence[a-1]][sequence[a]];
	  value = value + tsp.cost[sequence[a]][sequence[a+1]];
	  value = value + tsp.cost[sequence[b-1]][sequence[b]];
	  value = value + tsp.cost[sequence[b]][sequence[b+1]];
	  return *this;
  }
};

#endif /* TSPSOLUTION_H */
