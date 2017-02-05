 /**
 * @file cs1d.h
 * @brief 
 *
 */

#ifndef CS1D_H
#define CS1D_H

#include "cpxmacro.h"
#include <vector>

/**
 * Class that stores the data for a 1-D cutting stock problem
 * It also provides a method for reading an instance from file
 */

class Data
{
public:
	Data() : m(0) {}
	int m; // # of objects
	double W; // length of bar
	std::vector<double> L;
	std::vector<double> R;
	void read(const char* filename);
};

/**
 * Class that provides the building blocks for solving THE LINEAR RELAXATION 
 * 1-D cutting stock problem with a column generation approach
 */
class CS1D
{
public:
	/** Constructor */
	CS1D(Env _env, Prob _lp) : env(_env), lp(_lp) {}
	
	/**
	 * Init cplex Master LP with instance data
	 */
	void initMaster(const Data& data);
	
	/**
	 * Solve current "master" LP
	 * @param x optimal primal solution of current master
	 * @param u optimal dual solution of current master
	 * @param objval optimal objective value
	 */
	void solveMasterLP(std::vector<double>& x, std::vector<double>& u, double& objval);
	
	/**
	 * slave Problem 
	 * Solve the pricing problem associated with dual vector @param u
	 * @return true if the pricing was successful (in this case the master is automatically updated)
	 * false otherwise
	 */
	bool price(Env pricerEnv, const Data& data, const std::vector<double>& u);
	
	/**
	 * Perform cplex branch&cut on the current master
	 * @param x optimal primal solution
	 * @param objval optimal objective value
	 */
	void branchAndBoundOnThePartialModel(std::vector<double>& x, double& objval);

protected:
	// cplex data
	Env env;
	Prob lp;
};

#endif /* CS1D_H */
