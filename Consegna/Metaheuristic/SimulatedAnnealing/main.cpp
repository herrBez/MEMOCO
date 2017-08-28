/**
 * @file main.cpp
 * @brief 
 */


#include <stdexcept>
#include <ctime>
#include <sys/time.h>
#include "TSP.h"
#include "TSPSolution.h"
#include "TSPSolverSA.h"
//#include "TSPSolver.h"
using namespace std;
// error status and messagge buffer
int status;
char errmsg[255];


/**
 * main function 
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main (int argc, char const *argv[])
{
	bool exc_raised = false;
  try
  {
    if (argc < 2) throw runtime_error("usage: %s filename.dat");  /// new parameters for TS
    
     srand(time(NULL));
    
    TSP tspInstance; 
    tspInstance.read(argv[1]);
    TSPSolution aSolution(tspInstance);
    
    aSolution.print();
    clock_t t1,t2;
    t1 = clock();
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    TSPSolverSA tspSolver;
    tspSolver.initRnd(aSolution);
    aSolution.value = tspSolver.evaluate(aSolution, tspInstance);
    
	
    TSPSolution bestSolution(tspInstance);
    double T_0 = 1000;
    int maxIter = 30; //iteration at constant temperature
    tspSolver.solve(tspInstance,aSolution, bestSolution, T_0 , maxIter); /// new parameters for TS
	
    t2 = clock();
    gettimeofday(&tv2, NULL);
		
    cout << "FROM solution: "; 
    aSolution.print();
    cout << "(value : " << tspSolver.evaluate(aSolution,tspInstance) << ")\n";
    cout << "TO   solution: "; 
    bestSolution.print();
    cout << "(value : " << tspSolver.evaluate(bestSolution,tspInstance) << ")\n";
    cout << "in " << (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)) << " seconds (user time)\n";
    cout << "in " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds (CPU time)\n";
    
  }
  catch(exception& e)
  {
   cout << ">>>EXCEPTION: " << e.what() << std::endl;
	exc_raised = true;
  }
  return exc_raised?EXIT_FAILURE:EXIT_SUCCESS;
}
