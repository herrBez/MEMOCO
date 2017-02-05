/**
 * @file knapsack.cpp
 * @brief Knapsack Solver (using Cplex)
 *
 */

#include "knapsack.h"
#include <iostream>

bool KPminSolver::exec(const std::vector<double>& w, const std::vector<double>& p, double W, std::vector<double>& solution, double& objval)
{
	try
	{
		DECL_PROB( env, lp );
		
		// add vars
		int n = w.size();
		std::vector<char> xtype(n, 'B');
		CHECKED_CPX_CALL( CPXnewcols, env, lp, n, &p[0], 0, 0, &xtype[0], 0 );
		
		// add constraint
		int matbeg = 0;
		char sense = 'G';
		std::vector<int> idx(n);
		for (int i = 0; i < n; i++) idx[i] = i;
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, n, &W, &sense, &matbeg, &idx[0], &w[0], 0, 0 );
		
		// solve
		CHECKED_CPX_CALL( CPXmipopt, env, lp );
		
		// get solution
		solution.resize(n);
		CHECKED_CPX_CALL( CPXgetx, env, lp, &solution[0], 0, n - 1 );
		CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
		
		CPXfreeprob(env, &lp);
	}
	catch(std::exception& e)
	{
		std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
		return false;
	}
	return true;
}

