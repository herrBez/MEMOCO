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
 * Class that solves an arbitrary (min-form) binary knapsack problem with Cplex
 */
class KPminSolver
{
public:
	/** Constructor */
	KPminSolver(Env _env) : env(_env) {}
	/**
	 * Main method: solve the knapsack problem:
	 * \min p*x
	 * w*x >= W
	 * x \in \{0,1\}^n
	 * @param solution optimal primal solution
	 * @param objval optimal objective value of solution
	 * @return true if the solve was successful, false otherwise
	 */
	bool exec(const std::vector<double>& w, const std::vector<double>& p, double W, std::vector<double>& solution, double& objval);

protected:
	Env env;	
};

#endif /* KNAPSACK_H */
