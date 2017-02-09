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
bool benchmark = true;
int main (int argc, char const *argv[])
{
	srand(time(NULL));
	bool exc_raised = false;
	try
	{
		if (argc < 2) throw runtime_error("Usage: %s filename.dat <S>");  /// new parameters for TS
		int S = 15; //Population size
		
		
		
		TSP tspInstance; 
		tspInstance.read(argv[1]);
		
		if(S > tspInstance.n)
			S = tspInstance.n;
		
		TSPSolverGA tspSolver;
		//tspSolver.initRnd(aSolution);
		vector < TSPSolution > initialPopulation;
		
		//It returns the initial population sorted by decreasing fitness
		tspSolver.getInitPopulation(tspInstance, initialPopulation, S);
		
		if(!benchmark){
			cout << "INIT POPULATION FITTEST = ";
			if(tspInstance.n < 40){
				initialPopulation[0].print();
				cout << endl;
			} else {
				cout << "Init Best" << endl;
				cout << "Objval" << initialPopulation[0].value << endl;
			}
		} 
		
		
		TSPSolution bestSolution(tspInstance);

		
		clock_t t1,t2;
		t1 = clock();
		struct timeval  tv1, tv2;
		gettimeofday(&tv1, NULL);


		tspSolver.solve(tspInstance, initialPopulation, bestSolution); 
		
		
		t2 = clock();
		gettimeofday(&tv2, NULL);
		
		if(!benchmark){
			if(tspInstance.n < 40){
				cout << "BEST SOLUTION " << endl;
				bestSolution.print();
				cout << endl;
			} else {
				cout << "Best Solution" << endl;
				cout << bestSolution.value << endl;
				cout << bestSolution.fitness << endl;
			}
		}
		
		
		
		//cout << (bestSolution.isFeasible()?"FEASIBLE":"NOT FEASIBLE");
		
		double user_time = (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6));
		double cpu_time = (double)(t2-t1) / CLOCKS_PER_SEC;
		
		if(!benchmark){
			cout << "in " << user_time << " seconds (user time)\n";
			cout << "in " << cpu_time << " seconds (CPU time)\n";
			cout << "Objval: " << bestSolution.value << endl;
		
		}
		
		
		if(benchmark){
			cout << tspInstance.n << "\t" << user_time << "\t" << bestSolution.value << endl;
		}

	}
	catch(exception& e)
	{
		cout << ">>>EXCEPTION: " << e.what() << std::endl;
		exc_raised = true;
	}
	return exc_raised?EXIT_FAILURE:EXIT_SUCCESS;
}
