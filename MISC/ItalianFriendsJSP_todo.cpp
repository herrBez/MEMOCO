/**
 * @file ItalianFriendsJSP.cpp
 * @brief 
 */

#include <cstdio>
#include <iostream>
#include <vector>
#include "cpxmacro.h"

using namespace std;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];

// data
const int I = 4; // I: friends
const int K = 4; // K: journal
// sigma is the journal read by i in the position k 
const char nameI[I] = { 'A', 'B', 'C', 'D' };	
const char nameK[K] = { 'R', 'M', 'S', 'G' };

//Release time 0.0 = 8.30 B = 8.45 C = 8.45 
const double R[I] = { 0.0, 15.0, 15.0, 60.0 };
//Time needed for I to read k
const double D[I*K] = {	
  60.0, 2.0, 30.0, 5.0,
  25.0, 3.0, 75.0, 10.0,
  10.0, 5.0, 15.0, 30.0,
  1.0, 1.0, 1.0, 90.0 
};
// Sigma paremether -> person in position i will read in the k position the journal
const int S[I*K] = { 
  0, 2, 1, 3,
  2, 1, 0, 3,
  1, 2, 0, 3,
  3, 0, 2, 1 
};
// M is a very large constant -> should be high enough and it is better to take it smaller as possible.
double M; //TO BE INITIALIZED!!!
			
const int NAME_SIZE = 512;
char name[NAME_SIZE];
	
void setupLP(CEnv env, Prob lp, int & numVars )
{
	// determine a feasible value for M [ max_{i in I} R_i + sum_{i,k} D_{i,k} ]
	// M is the sum over all the durations	
	M = 0;
	for (int i = 0; i < I; ++i ) 
		if ( R[i] > M ) //After the last people is wake up
			M = R[i];

	for (int i = 0; i < I; i++) {
		for (int k = 0; k < K; k++) {
			M += D[i * K + k];
		}
	}

	// M is the time the last people is waked up + sum over all the duration

	const int h_init = 0; // first index for h vars
	// add h vars
	for (int i = 0; i < I; i++)
	{
		for (int k = 0; k < K; k++)
		{
			char htype = 'C';
			double obj = 0.0;
			double lb = 0.0;
			double ub = CPX_INFBOUND;
			snprintf(name, NAME_SIZE, "h_%c_%c", nameI[i], nameK[k]);
			char* hname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &htype, &hname );
		}
	}

	// add x vars
	const int x_init = CPXgetnumcols(env, lp); // first index for x vars
	for (int k = 0; k < K; k++)
	{
		for (int i = 0; i < I; i++)
		{
			for (int j = 0; j < I; j++)
			{
				if(i != j){
					//1 if i precedes j on K
					char xtype = 'B';
					double lb = 0.0; //lower bound
					double ub = 1.0; //upper bound
					double obj = 0.0;
					snprintf(name, NAME_SIZE, "x_%c_%c_%c", nameI[i], nameI[j], nameK[k]);
					char* xname = (char*)(&name[0]);
					CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
				}
				
				
				
			}
		}
	}
	
	// add y var
	const int yIdx = CPXgetnumcols(env, lp); // index of y var
	{
		char ytype = 'C';
		double lb = 0.0;
		double ub = CPX_INFBOUND;
		double obj = 1.0;
		snprintf(name, NAME_SIZE, "y");
		char* yname = (char*)(&name[0]);
		CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &ytype, &yname );
	}
	numVars = CPXgetnumcols(env, lp);
	
	// add finish-time constraints [ y >= h_{i S[i,|K|]} + D_{i S[i,|K|]} ] variables on the left side --> y -h_{i S[i,|K|]} >= D_{i S[i,|K|]} 
	for (int i = 0; i < I; i++)
	{
		std::vector<int> idx(2);
		idx[0] = yIdx;
		idx[1] = h_init + i*K + S[i*K + (K-1)]; // sigma(i,last)
		std::vector<double> coef(2);
		coef[0] = 1.0;
		coef[1] = -1.0;
		char sense = 'G';
		int matbeg = 0;
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idx.size(), &D[i*K + S[i*K + (K-1)]], &sense, &matbeg, &idx[0], &coef[0], 0, 0 );
	}
	
	// add wake-up constraints [ h_{i S[i,1]} >= R_i ]
	for(int i = 0; i < I; i++){
		int idx = h_init + i*K + S[i*K + 0]; // sigma(i,first)
		double coef = 1.0;
		char sense = 'G';
		int matbeg = 0;
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, 1, &R[i], &sense, &matbeg, &idx, &coef, 0, 0 );
	}
	
	// add non-overlapping constraints between consecutive newspapers [ h_{i S[i,l]} >= h_{i S[i,l-1]} + D_{i S[i,l-1]} ] 
	// I have to put h_{i S[i,l]} - h_{i S[i,l-1]} >= D_{i S[i,l-1]}
	for (int i = 0; i < I; i++)
	{
		for (int l = 1; l < K; l++)
		{
			std::vector<int> idx(2);
			idx[0] = h_init + i*K + S[i*K + l]; //h[i S[i,l]]
			idx[1] = h_init + i*K + S[i*K + (l-1)]; // sigma(i,last)
			std::vector<double> coef(2);
			coef[0] = 1.0;
			coef[1] = -1.0;
			char sense = 'G';
			int matbeg = 0;
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idx.size(), &D[i*K + S[i*K + l-1]], &sense, &matbeg, &idx[0], &coef[0], 0, 0 );	
		}
	}
	
	// add non-overlapping constraints between persons on the same newspaper 
	int xIdx = x_init;
	for (int k = 0; k < K; k++)
	{
		for (int i = 0; i < I; i++)
		{
			for (int j = 0; j < I; j++)
			{
				if ( i==j ) continue;
				
				// 1 [ h_{i k} >= h_{j k]} + D_{j k]} - M x_ijk ]
				std::vector<int> idx(3);
				idx[0] = h_init + i*K + k;
				idx[1] = h_init + j*K + k;
				idx[2] = xIdx;
				std::vector<double> coef(3);
				coef[0] = 1.0;
				coef[1] = -1.0;
				coef[2] = M;
				char sense = 'G';
				int matbeg = 0;
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idx.size(), &D[j*K + k], &sense, &matbeg, &idx[0], &coef[0], 0, 0 );
				
				// 2 [ h_{j k} >= h_{i k]} + D_{i k]} - M (1 -x_ijk) ]
				//h_{j k} - h_{i k]} + M (1 -x_ijk) >= D_{i k]}
				idx[0] = h_init + j*K + k;				
				idx[1] = h_init + i*K + k;
				idx[2] = xIdx;				
				
				coef[0] = 1.0;
				coef[1] = -1.0;				
				coef[2] = -M;				
				double rhs = D[i*K + k] - M;
	CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idx.size(), &rhs, &sense, &matbeg, &idx[0], &coef[0], 0, 0 );

				
				
				
				xIdx++; // we have as many constraints as many x variables
					// and the nested loops for constraints adding 
					// follow the same order as the one for x variable adding
			}
		}
	}
	
	// print (debug)
	CHECKED_CPX_CALL( CPXwriteprob, env, lp, "ItalianFriendsJSP.lp", 0 );
}

int main (int argc, char const *argv[])
{
	DECL_ENV( env );
   DECL_PROB( env, lp );
	

	
	return 0;
}
