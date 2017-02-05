/**
 * @file cutcallback_cover.cpp
 * @brief data cutcallback implementation (function)
 */


#include "cutcallback_cover.h"

const double EPS = 1e-5;
  // as usual, we define a tolerance to cut violation, to avoid numerical issues

int myCutCallback(CEnv env, void* cbinternaldata, int wherefrom, void* cbuserdatahandle, int* useraction_p)
  // this is the fixed prototype (c-function type) for a cut callback. Parameters:
  //      - env
  //      - cbinternaldata (internal datastructures)
  //      - wherefrom (integer reporting from where this instance has been called - internal use)
  //      - cbuserdatahandle (pointer to user-defined data, set by the "activating" CPXsetcutcallbackfunc -- see main.cpp)
  //      - an integer number coding what to do after (for example, "fail" if unrecoverable problems occurred)
{
	// check if solution is fractional
	*useraction_p = CPX_CALLBACK_DEFAULT; ///among other values, CPX_CALLBACK_FAIL (meaning that problems occurred so that, for example, we can stop)

	//If this function is called, then the solution of the current LP relaxation is fractional
	// (this function has been activated with CPXsetusercutcallbackfunc): hence it makes sense
        // looking for violated cover inequalities (which are never violated by integer solutions)

	// prepare user defined data handler
	CB_UserDataHandle* mydata = static_cast<CB_UserDataHandle*>(cbuserdatahandle);
	
	// get fractional solution
	int n = mydata->mkp->n;
	std::vector<double> x(n);
	CHECKED_CPX_CALL( CPXgetcallbacknodex, env, cbinternaldata, wherefrom, &x[0], 0, n - 1 );
	
	// separate
	// we separate cover inequalities for each knapsack constraint
	int m = mydata->mkp->m;
	
	//// DONE: determine the obj.func. coefficient of the separation problem (the same for every constraint)
  	std::vector<double> obj(n);
	for (int j = 0; j < n; j++) {
	  obj[j] = 1.0 - x[j];
	}	/// the objective function of the separation problem is always the same, for every knapsack constraint
  	
	double objval = 1000;
	std::vector<double> y(n); // return values
	for (int i = 0; i < m; i++)
	{
		vector <double> weights = mydata->mkp->weights[i];
		double capacity = mydata->mkp->capacities[i];
		//TODO: for each constraint, solve the suitable KPmin problem and, in case, add the cut
    		//mydata->solver->exec(...
    	
    	mydata->solver->exec(mydata->mkp->weights[i], 
				obj, 
				capacity + 1.0,
				y,
				&objval);
    	 

		if (objval < (1.0 - EPS)) /*we want to be sure that the cover cut is violated: the larger EPS, the more we are sure!*/
		{
			// we have a violated cover
			std::vector<int> idx;
			std::vector<double> coef(n);
			for(int k = 0; k < n; k++)
				coef[k] = 1 - z[k];
			double rhs = -1.0;
			
			
			///TODO: fill arrays 'idx' and 'coef' for "addrow"-like call
			
			
			
			// add ONE contraint to the "master" problem
			CHECKED_CPX_CALL( CPXcutcallbackadd, env, cbinternaldata, wherefrom, 
				idx.size(),	// nonzeros count
				rhs,		// RHS
				'L', 		// sense
				&idx[0], 	// vector of cut variables indexes
				&coef[0], 	// vector of cut coefficients
				0 		// purgeable -> if it is not needed do not add it (CPLEX does it for you!)
			);
			//parameters: CPXCENVptr env, void * cbdata, int wherefrom, int nzcnt, double rhs, int sense, const int * cutindexes, const double * cutcoefficients, int purgeable
			std::cout << "Added 1 cover" << std::endl;
		}
	}
	
	std::cout << "round finished" << std::endl;
	*useraction_p = CPX_CALLBACK_SET;
				/// return value: means that we did something (cut added). 
				//    By doing so, the added cut is considered (for example, if we leave "*useraction_p = CPX_CALLBACK_DEFAULT;", 
				//    the added cut would be ignored)
	return 0;
}
