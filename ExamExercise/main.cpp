/**
 * @author Mirko Bez
 * @file main.cpp
 * 
 * Usage: ./main <filename.dat>
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include "cpxmacro.h"
#include <getopt.h>
#include <unistd.h>
using namespace std;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];		
const int NAME_SIZE = 512;
char name[NAME_SIZE];
bool output_required = false;
bool print = false;


/** Struct containing the long options */
static struct option long_options[4] = {	
	{"help", no_argument, 0, 'h'},
	{"print", no_argument, 0, 'p'},
	{"output", no_argument, 0, 'o'},
	{0, 0, 0, 0},
};



/**
 * set up the model for CPLEX
 * @param env the cplex enviroment
 * @param lp the cplex problem
 * @param N the number of nodes of the TSP
 * @param C the matrix containing the costs from i to j
 * @param mapY used in order to have the result outside the setup function.
 */	
void setupLP(CEnv env, Prob lp, int N, vector< vector<double> > C, vector< vector<int> > & mapY)
{	
	
	int current_var_position 	= 0;
	vector< vector<int> > mapX(N, vector<int> (N));
	
	/* 
	 * adding x variables
	 * x_ij := amount of the flow shipped from i to j 
	 */ 
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(i == j)
				continue;
			
			char xtype = 'I';
			double obj = 0.0;
			double lb = 0.0;
			double ub = CPX_INFBOUND;
			snprintf(name, NAME_SIZE, "x_%d_%d", i, j);
			char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
			mapX[i][j] = current_var_position++;
		}
	}

	/* add y variables
	 * y_ij = 1 if the edge between node i and j is used 0 otherwise.
	 */
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(i == j)
				continue;
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
	//Creating additional scope in order to avoid future name conflicts (I will use idx other times)
	{
		vector<int> idx(N);
		vector<double> coef(N);
		//starting from without considering j
		for(int j = 1; j < N; j++){
			idx[j-1] = mapX[0][j];
			coef[j-1] = 1.0;
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
				idx[l] 			= mapX[i][k];		
				coef[l] 		= +1.0;
				l++;
			}
			for(int j = 0; j < N; j++){
				if(j == k)
					continue;
				idx[l] 			= mapX[k][j];
				coef[l] 		= -1.0;
				l++;
			}
			
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
			vector<double> coef(N, 1.0);		
			
			int l = 0;
			for(int j = 0; j < N; j++){
				if(i == j)
					continue;
				idy[l++] = mapY[i][j];
			}
			
			char sense = 'E';
			double rhs = 1.0;
			int matbeg = 0;
			snprintf(name, NAME_SIZE, "C%d",i);
			char* cname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, l, &rhs, &sense, &matbeg, &idy[0], &coef[0], 0, &cname);
		}
	}
	
	{
		for(int j = 0; j < N; j++){
			vector<int> idy(N-1);	
			vector<double> coef(N-1, 1.0);	

			int l = 0;
			for(int i = 0; i < N; i++){
			    if(i == j)
					continue;
			    idy[l++] = mapY[i][j];
			}
			char sense = 'E';
			double rhs = 1.0;
			int matbeg = 0;
			snprintf(name, NAME_SIZE, "D%d", j);
			char* cname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, l, &rhs, &sense, &matbeg, &idy[0], &coef[0], 0, &cname);
		}
	}

	
	
	


	// Creating constraint x_ij - |N| y_ij <= 0
	{
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				if(i == j)
					continue;
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
	if(output_required){
		CHECKED_CPX_CALL( CPXwriteprob, env, lp, "Model.lp", NULL ); 
	}
}

/**
 * fetch the y_ij variables from CPLEX, put the value in an array and print the values (as in the 
 * metaheuristic version). Called only when the option --print is given.
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param N the number of nodes of the TSP
 * @param mapY map containing the (CPLEX) index of the y variables of the problem
 * used in order to fetch the values of the y variables and print the tsp solution on the screen.
 */
void fetch_and_print_y_variables(CEnv env, Prob lp, int N, vector< vector<int> > mapY){
	int begin =  mapY[0][1]; //First y index
	int end = mapY[N-1][N-2]; //Last y index
	int length = end - begin + 1;
	double * y = (double *) malloc(sizeof(double) * length);
	CHECKED_CPX_CALL( CPXgetx, env, lp, y, begin, end);
	cout << "OPTIMAL SOLUTION = ";
	int k = 0;
	
	for(int i = 1; i < N; i++){
		
		if(round(y[i-1]) == 1.0){ //Searching in the first line the (only) 1, i.e. the first city to visit
			k = i;
			cout << "0" << " " << k << " ";
			break;
		}
	}
	
	int counter = 0;
	while(k != 0 && counter < N+2) { //untill reaching 0 the starting city and the number of cities is correct
		counter++;
		int start_index = k*N - k;
		for(int i = 0; i < N; i++){
			if(k == i) {
				start_index -= 1;
				continue;
			}
			if(round(y[start_index+i]) == 1.0){
				k = i;
				cout << k << " ";
				break;
			}
		}
		
	}
	
	cout << endl;
	free(y);
}


/**
 * solve the model write the result in a file and returns the return value of the function
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param N the number of nodes of the TSP
 * @param mapY a map containing the (CPLEX) index of the y variables of the problem
 * used in order to retrieve the values of the y variables and print them on the screen.
 * @return objval the optimal solution
 */
double solve( CEnv env, Prob lp, int N, vector< vector<int> > mapY) {
	clock_t t1,t2;
    t1 = clock();
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
	
	CHECKED_CPX_CALL( CPXmipopt, env, lp );
	t2 = clock();
    gettimeofday(&tv2, NULL);
	double objval = 0.0;
	CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
	
	cout << "in " << (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6)) << " seconds (user time)\n";
    cout << "in " << (double)(t2-t1) / CLOCKS_PER_SEC << " seconds (CPU time)\n";
	
	
	cout << "Objval: " << objval << endl;
	if(print){
		fetch_and_print_y_variables(env, lp, N, mapY);
	} 
	if(output_required){	
		CHECKED_CPX_CALL( CPXsolwrite, env, lp, "Model.sol");
	}
	return objval;
}

/**
 * read a file in the speified format:
 * the first row contains N,  the number of nodes
 * the rest N rows contain the information about the distances (It can be euclidean, Manhattan ecc.)
 * @param filename the name of the file to read
 * @return return a matrix of size N*N containing the read costs
 */
vector< vector<double> > readFile(const char * filename){
	ifstream infile(filename);
	int N;
	if(!infile.is_open()){
		cerr << "Error. Could not open file " << filename << ". Exiting ..." << endl;
		exit(EXIT_FAILURE);
	}
	infile >> N;
	vector< vector<double> > C(N, vector<double> (N));

	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			infile >> C[i][j];
		}
	}
	return C;
}



/** 
 * print the help message
 * @param argv in order to get the name of the compiled program
 */
void print_help(char * argv[]){
	cout << "Usage : " << argv[0] << " <instance.dat> " << " [OPT]... " << endl;
	cout << endl;
	cout << "-h, --help\t\t\tprint this message and exit" << endl;
	cout << "-o, --output\t\t\twrite the solved problem in a file name Model.sol" << endl;
	cout << "-p, --print\t\t\tat the end print the list of cities, e.g. 0 1 2 3 4 0" << endl;
	cout << endl;
}


/**
 * function that according to the given options set the flags
 * 	--output, in order to print the solved problem in the given file
 *  --print, in order to print the TSP-Solution 0 <1, 2 .. n> 0
 * @param argc 
 * @param argv
 */
vector< vector <double> > get_option(int argc,  char * argv[]){
	int c;
	if(argc < 2) {
		print_help(argv);
		exit(EXIT_FAILURE);
	}
	// One can specify only the help flag without the instance file.
	if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
		print_help(argv);
	}
	optind = 2; //Starting from index 2, because the first place is destinated to the istance file.
	int option_index;
	while((c = getopt_long (argc, argv, "", long_options, &option_index)) != EOF) {
		switch(c){
			case 'h': print_help(argv); exit(EXIT_SUCCESS); break;
			case 'o': output_required=true; break;
			case 'p': print = true; break;
		}
    }
    return readFile(argv[1]);
}

/**
 * the main function
 * @param argc
 * @param argv[]
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main (int argc, char *argv[])
{
	//variable used only to exit with the appropriate state
	bool exc_arised = false;
	try { 
		
		//In order to enable "random" generation of numbers
		vector< vector<double> > C = get_option(argc, argv);	
		int N = C.size();
		vector< vector<int> > mapY(N, vector<int> (N));
		cout << "Size of problem: " << N << endl;
		DECL_ENV( env );
		DECL_PROB( env, lp );
		setupLP(env, lp, N, C, mapY);
		// find the solution
		solve(env, lp, N, mapY);
		// free-allocated resolve
		CPXfreeprob(env, &lp);
		CPXcloseCPLEX(&env);
	} catch (exception& e){
		cout << ">>Exception: " << e.what() << endl;
		exc_arised = true;
	}	
	return exc_arised?EXIT_FAILURE:EXIT_SUCCESS;
}
