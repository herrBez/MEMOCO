/* TSPPopulation Implementation */
#include "TSPPopulation.h"

void TSPPopulation::printPopulation (){
	std::for_each(member.begin(), member.end(), [] (TSPSolution p) 
	{ 
		cout << "###";
		p.print();
		cout << "###" << endl;	
	});
}

