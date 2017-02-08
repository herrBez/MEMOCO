/**
 * @file main.cpp
 * @author Mirko Bez
 * @brief 
 */


#include <stdexcept>
#include <ctime>
#include <sys/time.h>
#include "TSP.h"
#include "TSPSolution.h"
#include "TSPSolverGA.h"

using namespace std;
// error status and messagge buffer
int status;
char errmsg[255];

/**
 * the main function
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main (int argc, char const *argv[])
{
	srand(time(NULL));
	bool exc_raised = false;
	try
	{
		if (argc < 2) throw runtime_error("usage: %s filename.dat");  /// new parameters for TS
		//srand(time(NULL));

		TSP tspInstance; 
		tspInstance.read(argv[1]);
		

		TSPSolverGA tspSolver;
		//tspSolver.initRnd(aSolution);
		vector < TSPSolution > initialPopulation;
		int S = 15; //=POPULATION SIZE
		//It returns the initial population sorted by decreasing fitness
		tspSolver.getInitPopulation(tspInstance, initialPopulation, S);

		cout << "INIT POPULATION FITTEST = ";
		if(tspInstance.n < 40){
			initialPopulation[0].print();
			cout << endl;
		} else {
			cout << "Init Best" << endl;
			cout << "Objval" << initialPopulation[0].value << endl;
			
		}
		
		
		TSPSolution bestSolution(tspInstance);

		
		clock_t t1,t2;
		t1 = clock();
		struct timeval  tv1, tv2;
		gettimeofday(&tv1, NULL);


		tspSolver.solve(tspInstance, initialPopulation, bestSolution); 
		
		
		t2 = clock();
		gettimeofday(&tv2, NULL);
		
		
		if(tspInstance.n < 40){
			cout << "BEST SOLUTION " << endl;
			bestSolution.print();
			cout << endl;
		} else {
			cout << "Best Solution" << endl;
			cout << bestSolution.value << endl;
			cout << bestSolution.fitness << endl;
		}
		
		cout << (bestSolution.isFeasible()?"FEASIBLE":"NOT FEASIBLE");
		

		
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
