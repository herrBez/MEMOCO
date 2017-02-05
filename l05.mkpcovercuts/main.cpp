/**
 * multi knapsack?
 * @file main.cpp
 * @brief implementation of a branch-and-cut method via Cplex (cut)callback
 */

#include <iostream>
#include <stdexcept>
#include "cpxmacro.h"
#include "knapsack.h"

using namespace std;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];


#include "mkpdata.h"
#include "cutcallback_cover.h" 

int main (int argc, char const *argv[])
{
	try
	{
		if (argc < 2) throw std::runtime_error("usage: ./main filename.dat [use_cover]");
		bool separateCoverCuts = (argc > 2);
		// init
		DECL_ENV( env );        // master problem
		DECL_PROB( env, lp );
		MKP data;
		data.read(argv[1]);
		
		DECL_ENV( env2 );       // prepare slave problem (separation)
		KPminSolver kp(env2);
		
		// setup LP (multi-knapsack 0/1 = generic binary linear programming model with max obj.func. and LE constraints)
		// add vars
		std::vector<char> xtype(data.n, 'B');
		CHECKED_CPX_CALL( CPXnewcols, env, lp, data.n, &(data.profits[0]), 0, 0, &xtype[0], 0 );
		CPXchgobjsen(env, lp, CPX_MAX);
		// add constraints
		int matbeg = 0;
		char sense = 'L';
		std::vector<int> idx(data.n);
		for (int i = 0; i < data.n; i++) idx[i] = i;
		for (int i = 0; i < data.m; i++)
		{
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, data.n, &(data.capacities[i]), &sense, &matbeg, &idx[0], &(data.weights[i][0]), 0, 0 );
		}
		CHECKED_CPX_CALL( CPXwriteprob, env, lp, "mkp.lp", 0 );
		// Act as a pure branch and bound method
		// set appropriate parameters for CPLEX engine (master problem)
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_CUTPASS, -1 );
		      //upper limit on the number of cutting plane passes CPLEX performs when solving the root node of a MIP model
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_HEURFREQ, -1 );
		      //how often (number of interval) to apply the periodic heuristic
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_PREIND, CPX_OFF );
		      //apply presolver to simplify and reduce problems before the problem is solved
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_MIPCBREDLP, CPX_OFF );
		      //MIP CallBack access node information: original model (off) or reduced, presolved model (on, default)
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_MIPDISPLAY, 4);
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_SCRIND, CPX_ON);
		CHECKED_CPX_CALL( CPXsetintparam, env, CPX_PARAM_MIPINTERVAL, 1);
		      //log mip process info
		
		// set cut callback
/* 
 * Cplex provides two different callbacks to run user defined code after each linear
 *  relaxation has been solved, depending on the fact that the LP relaxation 
 *  is fractional or integer:
 *    - CPXsetusercutcallbackfunc         (if the LP solution is fractional)
 *    - CPXsetlazyconstraintcallbackfunc  (if the LP solution is integer)
 *  In our case, we need CPXsetusercutcallbackfunc to activate a function that
 *  separates cover inequalities: in fact, cover inequality can be violated only
 *  by fractional solutions (integer feasible solutions do not violate any cover
 *  inequality).
 *  CPXsetlazyconstraintcallbackfunc is used when a separation procedure has to run
 *  on integer feasible solutions, for example to implement contraints generation 
 *  approaches, like the ones you have seen for TSP): the calling mechanism is the
 *  same as CPXsetusercutcallbackfunc. 
 */
		CB_UserDataHandle * ptr_mydata = NULL;
		if (separateCoverCuts)
		{
			ptr_mydata = new CB_UserDataHandle(&data, &kp);
			  // data for cut callback
			CHECKED_CPX_CALL( CPXsetusercutcallbackfunc, env, &myCutCallback, (void*)ptr_mydata );
			  // "activate" the cut callback (method to provide user-defined cuts). Parameters
			  //    - env (usual)
			  //    - myCutCallback (the function called each time)
			  //    - ptr_mydata (user-defined data passed to the function "myCutCallback")

		}
		
		// solve
		CHECKED_CPX_CALL( CPXmipopt, env, lp );
		double objval;
		CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
		std::cout << "Objval: " << objval << std::endl;
		std::cout << "Nodes:  " << CPXgetnodecnt(env, lp) << std::endl;
		
		// free
		if (ptr_mydata != NULL) delete ptr_mydata;
		CPXfreeprob(env, &lp);
		CPXcloseCPLEX(&env);
		CPXcloseCPLEX(&env2);
		
	}
	catch(std::exception& e)
	{
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
	}
	return 0;
}

