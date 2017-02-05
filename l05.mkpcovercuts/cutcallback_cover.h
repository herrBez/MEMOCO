/**
 * @file cutcallback_cover.h
 * @brief data cutcallback implementation (header files)
 */
 
 /* Cplex offers the opportunity of modifying the standard branch-and-cut behaviour
  * by setting some "entry-points" at specified steps. One of the opportunities is
  * the CUT callback, which is called each time Cplex finishes solving an LP relaxation.
  * This is the right moment to separate user-defined cuts and add related inequalities.
  * If the problem is modified (added cuts) Cplex iterates LP-relaxation solution.
  */
  
#include "cpxmacro.h"
#include <vector>
#include "mkpdata.h"
#include "knapsack.h"

#ifndef CB_DATA_CLASS
#define CB_DATA_CLASS

class CB_UserDataHandle //UserData
/* Callbacks have a fixed prototype. As we may need to pass problem-dependent data
 * to the callback, the cut callback function has a parameter "void* cbuserdatahandle" pointing
 * to some user-defined data structure containing those data. In our case, we define
 * this CB_UserDataHandle class to contain all the data we want to make available to our
 * cut callback (namely the problem data and the slave solver pointer) 
 */
{
public:
	CB_UserDataHandle(MKP* _mkp, KPminSolver* _solver) : mkp(_mkp), solver(_solver) {}
	MKP* mkp;
	KPminSolver* solver;
};

// declaration of the function that will be called at the suitable entry-point
int myCutCallback(CEnv env, void* cbinternaldata, int wherefrom, void* cbuserdatahandle, int* useraction_p);

#endif
