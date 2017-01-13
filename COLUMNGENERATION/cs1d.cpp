/**
 * @file cs1d.cpp
 * @brief 
 *
 */

#include "cs1d.h"
#include "knapsack.h"
#include <cmath>
#include <iostream>
#include <fstream>

const double EPS = 1e-5; //tolerance for numeric issues

void Data::read(const char* filename)
{
	std::ifstream in(filename);
	// read data from file
	in >> m; // number of piece types
	in >> W; // stock length
	std::cout << "m = " << m << std::endl;
	std::cout << "W = " << W << std::endl;
	for (int i = 0; i < m; i++) { //for each piece type
		double l;
		double r;
		in >> l;	//length
		in >> r;	//demand
		L.push_back(l);
		R.push_back(r);
		std::cout << l << " " << r << std::endl;
	}
	in.close();
}

void CS1D::initMaster(const Data& data)
{
	// setup initial LP
	std::vector<double> obj(data.m, 1.0);
	CHECKED_CPX_CALL( CPXnewcols, env, lp, data.m, &obj[0], 0, 0, 0, 0 );
	std::vector<int> matbeg(data.m);
	std::vector<int> idx(data.m);
	std::vector<double> coef(data.m);
	std::vector<char> sense(data.m, 'G');
	for (int i = 0; i < data.m; i++)
	{
		matbeg[i] = i;
		idx[i] = i;
		coef[i] = floor(data.W / data.L[i]);
	}
	CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, data.m, data.m, &(data.R[0]), &sense[0], &matbeg[0], &idx[0], &coef[0], 0, 0 );
	
	// debug
	CHECKED_CPX_CALL( CPXwriteprob, env, lp, "initial.lp", NULL );
}

void CS1D::solveMasterLP(std::vector<double>& x, std::vector<double>& u, double& objval)
{
	// solve
	CHECKED_CPX_CALL( CPXlpopt, env, lp );
	// get current LP obj value
	CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
	// get current RESTRICTED LP PRIMAL solution
	int n = CPXgetnumcols(env, lp);
	x.resize(n);
	CHECKED_CPX_CALL( CPXgetx, env, lp, &x[0], 0, n - 1 );
	// get current RESTRICTED LP DUAL solution
	int m = CPXgetnumrows(env, lp);
	u.resize(m);
	CHECKED_CPX_CALL( CPXgetpi, env, lp, &u[0], 0, m - 1 );
}

bool CS1D::price(Env pricerEnv, const Data& data, const std::vector<double>& u)
{
	KPSolver kp(pricerEnv);
	std::vector<double> z;
	double value;
	
	kp.exec(data.L, u, data.W, z, value);
	
	if (value < (1.0 + EPS)) return false;
	    // if the value is 1.000001, the reduced cost would be - 0.000001. Because 
	    //  of possible numerical troubles, we consider this number as 0
	
	// add column
	std::vector<int> idx;
	std::vector<double> coef;
	int m = z.size();
	for(int i = 0; i < m; i++) 	{
		if (z[i] > EPS) { // because of numerical troubles, we may have for z 
		                  //  values like  +/-1e-10 (that is 0)
			idx.push_back(i); 	//the variable will appear in the i-th constraint...
			coef.push_back(z[i]);	// ... with coefficient z[i]
		}
	}
	double obj = 1.0;
	int matbeg = 0;
	CHECKED_CPX_CALL( CPXaddcols, env, lp, 1, idx.size(), &obj, &matbeg, &idx[0], &coef[0], NULL, NULL, NULL );
		//status = CPXaddcols (env, lp, ccnt, nzcnt, obj, cmatbeg, cmatind, cmatval, lb, ub, newcolname);
	return true;
}


void CS1D::branchAndBoundOnThePartialModel(std::vector<double>& x, double& objval)
// provides a heuristic (not necessarily optimal) integer solution
{
	// change to Integer Program
	int n = CPXgetnumcols(env, lp);
	std::vector<char> xtype(n, 'I');
	CHECKED_CPX_CALL( CPXcopyctype, env, lp, &xtype[0] );
	// solve
	CHECKED_CPX_CALL( CPXmipopt, env, lp );
	// get solution
	CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
	x.resize(n);
	CHECKED_CPX_CALL( CPXgetx, env, lp, &x[0], 0, n - 1 );
}

