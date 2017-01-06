/**
 * @file main.cpp
 * @brief 
 */


#include <stdexcept>
#include <ctime>
#include <sys/time.h>
#include "TSP.h"
#include "TSPSolution.h"
#include "TSPSolverGA.h"
//#include "TSPSolver.h"
using namespace std;
// error status and messagge buffer
int status;
char errmsg[255];


int main (int argc, char const *argv[])
{
  try
  {
    if (argc < 2) throw runtime_error("usage: %s filename.dat");  /// new parameters for TS
    //srand(time(NULL));
    
    TSP tspInstance; 
    tspInstance.read(argv[1]);
    //TSPSolution aSolution(tspInstance);
    //aSolution.print();
    //clock_t t1,t2;
    //t1 = clock();
    //struct timeval  tv1, tv2;
   // gettimeofday(&tv1, NULL);
    
    TSPSolverGA tspSolver;
    //tspSolver.initRnd(aSolution);
    vector < TSPSolution > initialPopulation;
	int S = 15; //=POPULATION SIZE
	tspSolver.getInitPopulation(tspInstance, initialPopulation, S);
	
	cout << "INIT POPULATION FITTEST = ";
	initialPopulation[0].print();
	cout << endl;
    TSPSolution bestSolution(tspInstance);
   
    tspSolver.solve(tspInstance, initialPopulation, bestSolution); /// new parameters for TS
	cout << "BEST SOLUTION " << endl;
	bestSolution.print();
	cout << endl;
	cout << (bestSolution.isFeasible()?"FEASIBLE":"NOT FEASIBLE");
    //t2 = clock();
    //gettimeofday(&tv2, NULL);
		
   /* cout << "FROM solution: "; 
    aSolution.print();
    cout << "(value : " << tspSolver.evaluate(aSolution,tspInstance) << ")\n";
    cout << "TO   solution: "; 
    bestSolution.print();
    cout << "(value : " << tspSolver.evaluate(bestSolution,tspInstance) << ")\n";
    cout << "in " << (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)) << " seconds (user time)\n";
    cout << "in " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds (CPU time)\n";*/
    
  }
  catch(exception& e)
  {
   cout << ">>>EXCEPTION: " << e.what() << std::endl;
  }
  return 0;
}
