
/**
 * @file main.cpp
 * @brief 
 */


#include <stdexcept>
#include <ctime>
#include <sys/time.h>
#include <string>
#include "LONGTSP.h"

using namespace std;
// error status and messagge buffer
int status;
char errmsg[255];


int main (int argc, char const *argv[])
{
  try
  {
    if (argc < 2) throw runtime_error("usage: %s filename.dat");  /// new parameters for TS
	TSP tspInstance; 
    tspInstance.read(argv[1]);
    
  }
  catch(exception& e)
  {
   cout << ">>>EXCEPTION: " << e.what() << std::endl;
  }
  return 0;
}
