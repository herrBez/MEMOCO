#ifndef __TSP_SOLVER_SIMULATED_ANNEALING__H__
#define __TSP_SOLVER_SIMULATED_ANNEALING__H__
#include <vector>
#include <iostream>


#include "TSPSolution.h"

/**
 * class that implements a TSPSolver that use simulated annealing
 */
class TSPSolverSA
{
public:
  /** Constructor */
  TSPSolverSA ( ) { }
  /**
   * evaluate a solution
   * @param sol: solution to be evaluated
   * @param TSP: TSP data
   * @return the value of the solution
   */
  double evaluate ( const TSPSolution& sol , const TSP& tsp ) const {
    double total = 0.0;
    for ( uint i = 0 ; i < sol.sequence.size() - 1 ; ++i ) {
      int from = sol.sequence[i]  ;
      int to   = sol.sequence[i+1];
      total += tsp.cost[from][to];
    }
    return total;
  }
  /**
   * initialize a solution as a random sequence by random swaps
   * @param sol solution to be initialized
   * @return true if everything OK, false otherwise
   */
  bool initRnd ( TSPSolution& sol ) {
    srand(time(NULL));
    for ( uint i = 1 ; i < sol.sequence.size() ; ++i ) {
      // intial and final position are fixed (initial/final node remains 0)
      int idx1 = rand() % (sol.sequence.size()-2) + 1;
      int idx2 = rand() % (sol.sequence.size()-2) + 1;
      int tmp = sol.sequence[idx1];
      sol.sequence[idx1] = sol.sequence[idx2];
      sol.sequence[idx2] = tmp;
    }
    //std::cout << "### "; sol.print(); std::cout << " ###" << std::endl;
    return true;
  }
  /**
   * simulated annealing
   * @param TSP TSP data
   * @param initSol initial solution
   * @param bestSol best found solution (output)
   * @param T_0 the start temperature
   * @param maxIter maximum number of iteration
   * @return true if everything OK, false otherwise
   */
  bool solve ( const TSP& tsp , const TSPSolution& initSol , TSPSolution& bestSol, const double T_0, const int maxIter );


  
};


#endif /* END __TSP_SOLVER_SIMULATED_ANNEALING__H__ */
