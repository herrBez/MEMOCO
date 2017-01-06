/**
 * @file TSP.h
 * @brief TSP data
 *
 */

#ifndef TSP_H
#define TSP_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;
/**
 * Class that describes a TSP instance (a cost matrix, nodes are identified by integer 0 ... n-1)
 */
class TSP
{
public:
  TSP() : n(0) , infinite(1e10) { }
  int n; //number of nodes
  
  std::vector< vector < double > > cost;
  void read(const char* filename)
  {
	  std::vector< std::pair<double, double> > cities;
	  std::ifstream in(filename);
		// read size
		string line = "";
		for(int i = 0; i < 7; i++){
			getline(in,line);
		}
		getline(in, line);
		//cout << line << endl;
		while(line != "EOF"){
			int l = 0;
			double a;
			double b;
			sscanf(line.c_str(), "%d %lf %lf", &l, &a, &b);
			cities.push_back(std::make_pair(a,b));
			cout << line << endl;
			getline(in,line);
		}
		cout << cities.size() << endl;
		n = cities.size();
		cost.resize(n);
		
		for (int i = 0; i < n; i++) {
			cost[i].reserve(n);
		}
		cout << "calculating distance .. ";
		distance(cities);
		cout << "done. " << endl;
		cout << "Writing to file" << endl;
		
		
		in.close();
		writeToFile("/tmp/tspInstance.txt");
  }
  double infinite; // infinite value (an upper bound on the value of any feasible solution
  void writeToFile(const char * name){
	  std::ofstream myfile;
	  myfile.open(name);
	  myfile << n << endl;
	  for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			myfile << cost[i][j] << " ";
		}
		cout << "Line " << i <<  endl;
		myfile << endl;
	  }
	  myfile.close();
	 
  }
  private : 
	void distance(vector < pair < double, double > > v) {
		
		for(uint i = 0; i < v.size(); i++){
			for(uint j = 0; j < v.size(); j++){
				if(i == j) {
					cost[i][j] = 0.0;
				} else {
					cost[i][j] = 0;
					cost[i][j] = sqrt((v[i].first - v[j].first)*(v[i].first - v[j].first) +
					             (v[i].second - v[j].second)*(v[i].second - v[j].second));
				}
				
			}
			//cout << i << " Finished " << endl;
		}
		
	
	}
	
	
};

#endif /* TSP_H */
