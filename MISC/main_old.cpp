/**
 * @file main.cpp
 * @brief 
 */

#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"
#include<time.h>

//Default values for N and the MAX_COST, used in order to render the model more flexible
#ifndef DEFAULT_N
	#define DEFAULT_N 5
#endif

#ifndef DEFAULT_MAX_COST
	#define DEFAULT_MAX_COST 10
#endif


using namespace std;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];		
const int NAME_SIZE = 512;
char name[NAME_SIZE];



	
void setupLP(CEnv env, Prob lp, int & numVars, const int N, const int MAX_COST)
{	
	int current_var_position 	= 0;
	/* Initialize three 2 dimensional vectors mapX, mapY, C */
	vector< vector<int> > mapX(N, vector<int> (N));
	vector< vector<int> > mapY(N, vector<int> (N));
	vector< vector<int> > C(N, vector<int> (N));

	
	/* Initialize randomly the time taken by the drill to move from node i to node j */
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			if (i == j)
				C[i][j] = 0;
			C[i][j] = 1;	
		}
	}
	
	//add x variables
	// x_ij = amount of the flow shipped from i to j 
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			
			
			char xtype = 'I';
			double obj = 0.0;
			double lb = 0.0;
			double ub = CPX_INFBOUND;
			snprintf(name, NAME_SIZE, "x_%d_%d", i, j);
			char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
			mapX[i][j] = current_var_position;
			current_var_position++;
		}
	}

	//add y variables
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){

			char ytype = 'B';
			double obj = C[i][j]; //In the object function the coefficients are exactly C[ij]
			double lb = 0.0;
			double ub = 1.0;
			snprintf(name, NAME_SIZE, "y_%d_%d", i, j);
			char* yname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &ytype, &yname );
			mapY[i][j] = current_var_position++;		
		}
	}
	
	//Creating constraint Nr 1 sum_{j:(0,j)} x_0j = |N|
	//Creating scope in order to avoid future name conflicts (I will use idx other times)
	{
		vector<int> idx(N);
		vector<double> coef(N);
		for(int j = 0; j < N; j++){
			idx[j] = mapX[0][j];
			coef[j] = 1.0;
		}
		char sense = 'E';
		int matbeg = 0;
		double rhs = N;
		snprintf(name, NAME_SIZE, "A");
		char* cname = (char*)(&name[0]);
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &rhs, &sense, &matbeg, &idx[0], &coef[0], 0, &cname);
	}
	
	{
		for(int k = 1; k < N; k++){
			vector<int> idx(2*N - 2);
			vector<double> coef(2*N - 2);
			int l = 0;
			for(int i = 0; i < N; i++){
				if(i == k)
					continue;
				cout << "(" << i << "," << k << ")" << mapX[i][k] << endl;
				idx[l] 			= mapX[i][k];		
				coef[l] 		= +1.0;
				l++;
			}
			for(int j = 0; j < N; j++){
				if(j == k)
					continue;
				cout << "(" << k << "," << j << ")" << mapX[k][j] << endl;

				idx[l] 			= mapX[k][j];
				coef[l] 		= -1.0;
				l++;
			}
			cout << k << "size = " << idx.size() << endl;
			
			char sense = 'E';
			double rhs = 1.0;
			int matbeg = 0.0;
			snprintf(name, NAME_SIZE, "B_%d", k);
			char* cname = (char*)(&name[0]);

			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, l, &rhs, &sense, &matbeg, &idx[0], &coef[0], 0, &cname );
		}
	}
	
	
	//Creating constraint sum_{j:(i,j) \in A} y_ij = 1 for all i \in A
	{
		for(int i = 0; i < N; i++){
			vector<int> idy(N);	
			vector<double> coef(N);		
			for(int j = 0; j < N; j++){
				idy[j] = mapY[i][j];
				coef[j] = 1.0;
			}
			
			char sense = 'E';
			double rhs = 1.0;
			int matbeg = 0;
			snprintf(name, NAME_SIZE, "C%d",i);
			char* cname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &rhs, &sense, &matbeg, &idy[0], &coef[0], 0, &cname);
		}
	}
	
	{
		for(int j = 0; j < N; j++){
			vector<int> idy(N);	
			vector<double> coef(N);	

			for(int i = 0; i < N; i++){
				idy[i] = mapY[i][j];
				coef[i] = 1.0;
			}
			char sense = 'E';
			double rhs = 1.0;
			int matbeg = 0;
			snprintf(name, NAME_SIZE, "D%d", j);
			char* cname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &rhs, &sense, &matbeg, &idy[0], &coef[0], 0, &cname);
		}
	}

	
	
	


	// Creating constraint x_ij - |N| y_ij <= 0
	{
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				vector<int> id(2);
				vector<double> coef(2);
				id[0] = mapX[i][j];
				id[1] = mapY[i][j]; // sigma(i,last)		
				coef[0] = 1.0;
				coef[1] = -N;
				char sense = 'L';
				int matbeg = 0;
				double rhs = 0;
				snprintf(name, NAME_SIZE, "FOO_%d_%d", i,j);
				char* cname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, 2, &rhs, &sense, &matbeg, &id[0], &coef[0], 0, &cname);			
			}
		}
	}
	
 	CHECKED_CPX_CALL( CPXwriteprob, env, lp, "Model.lp", 0 ); 
	CHECKED_CPX_CALL(CPXsolwrite, env, lp, "Model.sol");
	cout << status << endl; 	
}


void get_command_line_parameters(int argc, char const * argv[], int * N, int * max_cost){
	*N = (argc < 2)?DEFAULT_N:(int) strtol(argv[1], NULL, 0);	
	*max_cost = (argc < 3)?DEFAULT_MAX_COST:(int) strtol(argv[2], NULL, 0);
}

int main (int argc, char const *argv[])
{
	int N, max_cost;
	//In order to enable "random"
	srand(time(NULL));
	get_command_line_parameters(argc, argv, &N, &max_cost);	
	DECL_ENV( env );
    DECL_PROB( env, lp );
	int numVars;
	setupLP(env, lp, numVars, N, max_cost );
	

	
	return 0;
}
