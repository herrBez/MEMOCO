/**
 * @file knapsack.h
 * @brief Knapsack Solver (using Cplex)
 *
 */

#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "cpxmacro.h"
#include <vector>

/**
 * Class that solves an arbitrary integer knapsack problem with Cplex
 */
class KPSolver
{
public:
	/** Constructor */
	KPSolver(Env _env) : env(_env) {}
	
	/**
	 * Main method: solve the knapsack problem:
	 * \max profits*x
	 * weights*x <= capacity
	 * x >= 0 integer
	 * @param solution    optimal solution x
	 * @param objval      optimal objective value of solution
	 * @return true if the solve was successful, false otherwise
	 */
	bool exec(const std::vector<double>& weights, const std::vector<double>& profits, double capacity, std::vector<double>& solution, double& objval);
protected:
	Env env;	
};

#endif /* KNAPSACK_H */
