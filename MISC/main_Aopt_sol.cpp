/**
 * @file main.cpp
 * @brief 
 */

#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"


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
	int * map_x 					= (int *) malloc(sizeof(int) * N * N);
	int * map_y 					= (int *) malloc(sizeof(int) * N * N);	
	int * C 							= (int *) malloc(sizeof(int) * N * N);
	for(int i = 0; i < N*N; i++){
		C[i] = rand() % MAX_COST;		
	}
	const int zero = 0;
	//add x variables
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			char xtype = 'I';
			double obj = 0.0;
			double lb = 0.0;
			double ub = CPX_INFBOUND;
			snprintf(name, NAME_SIZE, "x_%d_%d", i, j);
			char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
			map_x[i*N + j] = current_var_position++;
		}
	}

	//add y variables
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			char ytype = 'B';
			double obj = C[i*N + j]; //In the object function the coefficients are exactly C[ij]
			double lb = 0.0;
			double ub = 1.0;
			snprintf(name, NAME_SIZE, "y_%d_%d", i, j);
			char* yname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &ytype, &yname );
			map_y[i*N + j] = current_var_position++;		
		}
	}

	//Creating constraint Nr 1 sum_{j:(0,j)} x_0j = |N|
	//Creating scope in order to avoid future name conflicts (I will use idx other times)
	{
		int * idx 		= (int *) malloc(sizeof(int) * N);
		double * coef 	= (double *) malloc(sizeof(double) * N);
		for(int j = 0; j < N; j++){
			idx[j] = map_x[zero*N + j];
			coef[j] = 1.0;
		}
		char sense = 'E';
		int matbeg = 0;
		double lhs = N;
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &lhs, &sense, &matbeg, &idx[0], &coef[0], 0, 0 );
		free(idx);
		free(coef);	
	}
	
	//Creating constraint sum_{j:(i,j) \in A} y_ij = 1 for all i \in A
	{
		for(int i = 0; i < N; i++){
			std::vector<int> idy(N);	
			std::vector<double> coef(N);	
			const int row_index = i*N;	
			for(int j = 0; j < N; j++){
				idy[j] = map_y[row_index + j];
				coef[j] = 1.0;
			}
			char sense = 'E';
			double lhs = 1.0;
			int matbeg = 0;
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &lhs, &sense, &matbeg, &idy[0], &coef[0], 0, 0 );
		}
	}
	
	{
		for(int j = 0; j < N; j++){
			std::vector<int> idy(N);	
			std::vector<double> coef(N);	

			for(int i = 0; i < N; i++){
				idy[i] = map_y[i*N + j];
				coef[i] = 1.0;
			}
			char sense = 'E';
			double lhs = 1.0;
			int matbeg = 0;
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N, &lhs, &sense, &matbeg, &idy[0], &coef[0], 0, 0 );
		}
	}

	//Ottimizzazione A
	{
		for(int k = 1; k <= N; k++){
			std::vector<int> idx(N+N);
			std::vector<double> coef(N+N);
			for(int i = 0; i < N; i++){
				for(int j = 0; j < N; j++){
					if(!(k == i && k == j)){
						idx[i] 		= map_x[i*N + k];
						idx[N+j]		= map_x[k*N + j];
						coef[i] 		= 1.0;
						coef[N+j] 	= -1.0;			
					}	
				} 	
			}
			char sense = 'E';
			double lhs = 1.0;
			int matbeg = 1.0;
			// N - 1 because in each iteration I remove a pair of variables (+x_ii - x_ii) that is always 0 and not need to be computed
			// Totally I can spare N useless sums and subtractions
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, N-1, &lhs, &sense, &matbeg, &idx[0], &coef[0], 0, 0 );
		}


	}
	



	// Creating constraint x_ij - |N| y_ij <= 0
	{
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				std::vector<int> id(2);
				id[0] = map_x[i*N + j];
				id[1] = map_y[i*N + j]; // sigma(i,last)		
				std::vector<double> coef(2);
				coef[0] = 1.0;
				coef[1] = N;
				char sense = 'L';
				int matbeg = 0;
				double lhs = 0;
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, id.size(), &lhs, &sense, &matbeg, &id[0], &coef[0], 0, 0 );			
			}
		}
	}




 	CHECKED_CPX_CALL( CPXwriteprob, env, lp, "Model.lp", 0 );
	

	//Free allocated resources
	free(map_x); 					
	free(map_y);					
	free(C); 	
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
