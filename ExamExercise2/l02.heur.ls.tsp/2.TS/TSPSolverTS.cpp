/**
 * @file TSPSolver.cpp
 * @brief TSP solver (neighborhood search)
 *
 */

#include "TSPSolver.h"
#include <iostream>

bool TSPSolver::solve ( const TSP& tsp , const TSPSolution& initSol , int tabulength , int maxIter , TSPSolution& bestSol )   /// TS: new param
{
  try
  {
    bool stop = false;
    int  iter = 0;

    ///Tabu Search
    tabuLength = tabulength;
    tabuList.reserve(tsp.n);
    initTabuList(tsp.n);
    ///
    
    TSPSolution currSol(initSol);
    double bestValue, currValue;
    bestValue = currValue = evaluate(currSol,tsp);
    TSPMove move;
    while ( ! stop ) {
      ++iter;                                                                                             /// TS: iter not only for displaying
      if ( tsp.n < 20 ) currSol.print();
      std::cout << " (" << iter << ") value " << currValue << "\t(" << evaluate(currSol,tsp) << ")";      /// TS: iter
      
      double bestNeighValue = currValue + findBestNeighbor(tsp,currSol,iter,move);                        /// TS: iter
      //if ( bestNeighValue < currValue ) {                                                               /// TS: replace stopping and moving criteria
      //  bestValue = currValue = bestNeighValue;                                                         ///
      //  currSol = swap(currSol,move);                                                                   ///
      //  stop = false;                                                                                   ///
      //} else {                                                                                          ///
      //  stop = true;                                                                                    ///
      //}                                                                                                 ///
      
      if ( bestNeighValue >= tsp.infinite ) {                                                             // TS: stop because all neighbours are tabu
        std::cout << "\tmove: NO legal neighbour" << std::endl;                                           //
        stop = true;                                                                                      //
        continue;                                                                                         //
      }                                                                                                   //
      
      std::cout << "\tmove: " << move.from << " , " << move.to;
      
      tabuList[currSol.sequence[move.from]] = iter;                                                       /// TS: update tabu list
      tabuList[currSol.sequence[move.to]]   = iter;                                                       ///
      currSol = swap(currSol,move);                                                                       /// TS: always the best move
      currValue = bestNeighValue;                                                                         /// 
      if ( currValue < bestValue - 0.01 ) {                                                               /// TS: update incumbent (if better -with tolerance- solution found)
        bestValue = currValue;                                                                            ///
        bestSol = currSol;                                                                                ///
        std::cout << "\t***";                                                                             ///
      }                                                                                                   /// 
      
      if ( iter > maxIter ) {                                                                             /// TS: new stopping criteria
        stop = true;                                                                                      ///
      }                                                                                                   ///
      std::cout << std::endl;
    }
    //bestSol = currSol;                                                                                  /// TS: not always the neighbor improves over 
                                                                                                          ///     the best available (incumbent) solution 
  }
  catch(std::exception& e)
  {
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
    return false;
  }
  return true;
}

TSPSolution& TSPSolver::swap ( TSPSolution& tspSol , const TSPMove& move ) 
{
  TSPSolution tmpSol(tspSol);
  for ( int i = move.from ; i <= move.to ; ++i ) {
    tspSol.sequence[i] = tmpSol.sequence[move.to-(i-move.from)];
  }
  return tspSol;
}


double TSPSolver::findBestNeighbor ( const TSP& tsp , const TSPSolution& currSol , int currIter , TSPMove& move )     /// TS: use currIter
/* Determine the NON-TABU *move* yielding the best 2-opt neigbor solution
 */
{
  double bestCostVariation = tsp.infinite;

  // intial and final position are fixed (initial/final node remains 0)
  for ( uint a = 1 ; a < currSol.sequence.size() - 2 ; a++ ) {
    int h = currSol.sequence[a-1];
    int i = currSol.sequence[a];
    for ( uint b = a + 1 ; b < currSol.sequence.size() - 1 ; b++ ) {
      int j = currSol.sequence[b];
      int l = currSol.sequence[b+1];
      if ( (currIter - tabuList[i] <= tabuLength) && (currIter - tabuList[j] <= tabuLength) ) continue; /// TS: tabu check (just one among many ways of doing it...)
      double neighCostVariation = - tsp.cost[h][i] - tsp.cost[j][l]
                                  + tsp.cost[h][j] + tsp.cost[i][l] ;
      if ( neighCostVariation < bestCostVariation ) {
        bestCostVariation = neighCostVariation;
        move.from = a;
        move.to = b;
      }
    }
  }
  return bestCostVariation;
}
