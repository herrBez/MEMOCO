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
		//TODO...
		
		// add constraint
		//TODO...
		
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
