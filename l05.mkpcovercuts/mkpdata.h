/**
 * @file mkpdata.h
 * @brief data for multi-knapsack problem (from file)
 */


#ifndef MKP_CLASS
#define MKP_CLASS

#include <iostream>
#include <fstream>

class MKP
{
public:
	MKP() : m(0), n(0) {}
	int m;
	int n;
	std::vector<double> capacities;
	std::vector<double> profits;
	std::vector< std::vector<double> > weights;
	void read(const char* filename)
	{
		std::ifstream in(filename);
		// read size
		in >> m;
		in >> n;
		std::cout << "m = " << m << std::endl;
		std::cout << "n = " << n << std::endl;
		// read capacities
		for (int i = 0; i < m; i++)
		{
			double cap;
			in >> cap;
			capacities.push_back(cap);
		}
		// read profits
		for (int i = 0; i < n; i++)
		{
			double p;
			in >> p;
			profits.push_back(p);
		}
		// read weights
		weights.resize(m);
		for (int i = 0; i < m; i++)
		{
			weights[i].reserve(n);
			for (int j = 0; j < n; j++)
			{
				double w;
				in >> w;
				weights[i].push_back(w);
			}
		}
		in.close();
	}
};


#endif
