/**
 * @file knapsack.cpp
 * @brief Knapsack Solver (using Cplex)
 *
 */

#include "knapsack.h"
#include <iostream>

bool KPSolver::exec(const std::vector<double>& weights, const std::vector<double>& profits, double capacity, std::vector<double>& solution, double& objval)
{
	try
	{
		DECL_PROB( env, lp );
		
		// add vars
		int n = weights.size();
		std::vector<char> xtype(n, 'I');
		CHECKED_CPX_CALL( CPXnewcols, env, lp, n, &profits[0], NULL, NULL, &xtype[0], NULL );
		CPXchgobjsen(env, lp, CPX_MAX);
		
		// add constraint
		
		
		// solve
		CHECKED_CPX_CALL( CPXmipopt, env, lp );
		
		// get solution
		solution.resize(n);
		//TODO...
		
		CPXfreeprob(env, &lp);
	}
	catch(std::exception& e)
	{
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
		return false;
	}
	return true;
}
