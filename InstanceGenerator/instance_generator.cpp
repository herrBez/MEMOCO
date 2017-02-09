/**
 * @author Mirko Bez
 * @file instance_generator.cpp
 * 
 * Usage: ./main N <q|r|c> [size]
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <array>
#include <random>
#include <string.h>
#include <string>
		
using namespace std;






/**
 * struct used to simulate points
 */
struct Point{

    double x;
    double y;
	
	
    inline Point& operator=(Point other){
        x=other.x;
        y=other.y;
        return *this;
    }
	/**
	 * print the points
	 */
	void print(){
		cout << "(" << x << "," << y << ")" << endl;
	}
	/**
	 * calculats the euclidean distance between this and other
	 * @param other
	 * @return the manhattan distance between this and other
	 */
	double euclideanDistance(Point other){
		double dx = other.x - x;
		double dy = other.y - y;
		return dx*dx + dy*dy;
	}
	/**
	 * calculates the manhattan distance between this and other
	 * @param other
	 * @return the manhattan distance between this and other
	 */
	double manhattanDistance(Point other){
		double dx = abs(other.x - x);
		double dy = abs(other.y - y);
		return dx + dy;
	}
 };  

/**
 * print the output in gerber file format. 
 * You can view it online on this website http://www.gerber-viewer.com/.
 * @param p the set of drill points
 * @param N the number of drill points
 */
void printGerberFile(Point * p, const int N, const char * filename = "tsp_instance"){
	char * tmpBuffer = new char[128];
	sprintf(tmpBuffer, "/tmp/%s.gbr", filename);
	ofstream outfile(tmpBuffer);
    /*Format: Leading Zero ommitted, Absolute coordinate, 
     *X:5 digit in the integer part, 
     *Y:5digit in the fractional part 
	 */
	outfile << "%FSLAX66Y66*%" << endl; 
	
	outfile << "%MOMM*%" << endl; //set mm as unit
	outfile << "%TF.Part,Single*%" << endl; //single pbc
	outfile << "%LPD*%" << endl; //set polarity to dark
	outfile << "%TA.AperFunction,Other,ComponentDrill*%" << endl; 
	outfile << "%ADD11C,8.000*%" << endl; //size of hole is one mm
	
	outfile << "G01*" << endl;
	outfile << "D11*" << endl;
	
	
	for(int i = 0; i < N; i++){
		sprintf(tmpBuffer, "X%5.2fY%5.2fD03*", p[i].x, p[i].y); 
		outfile << tmpBuffer << endl;
	}
	//EOF
	outfile << "M02*" << endl;
	delete[] tmpBuffer;
}


/**
 * print the cost in a dat file
 * @param cost
 * @param N
 * 
 */
void printDatFile(double * cost, const int N, const char * file_name = "tsp_instance"){
	char * tmpBuffer = new char[128];
	sprintf(tmpBuffer, "/tmp/%s.dat", file_name);//In order to save my SSD
	ofstream outfile(tmpBuffer);
	outfile << N << endl;
	for(int i = 0; i < N; i++){
		for(int j  = 0; j < N; j++){
			outfile << cost[i*N + j] << " ";
		}
		outfile << endl;
	}
	outfile.close();
	delete[] tmpBuffer;
}

bool euclidean = false;

/**
 * calculate the distance between the points and return a (linearized) 2-dimensional array
 * @param N the number of points
 * @param p the points
 * @return an array containing the distances
 */
double * calculateDistance(const int N, Point * p){
	double * cost = new double[N*N];
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(i == j){
				cost[i*N + j] = 0;
			}
			
			cost[i*N + j] = euclidean?p[i].euclideanDistance(p[j]):p[i].manhattanDistance(p[j]);
		}
	}
	return cost;
}


/**
 * print the instance in a binary pbm file. The advantage with respect to its ASCII counterpart
 * is that instead of saving 1 or 0 in an (entire byte) it saves 1 or 0 in bits. So it is 8 times
 * smaller than its ASCII counterpart. 
 * 
 * @param size the size of the field
 * @param a the array containing the points to print
 * @param N the number of points
 */
void printBinaryPBM(const int size, Point * a, int N, const char * filename = "output.tmp"){
	char * tmpBuffer = new char[128];
	sprintf(tmpBuffer, "/tmp/%s.pbm", filename);
	int size_8 = (int) ceil(((double)size)/8) * 8; //approximate to next multiple of eight
	int byte_num = size_8 / 8;
	
	unsigned char * img = new unsigned char[size_8*byte_num];
	
	for(int i = 0; i < size_8*byte_num; i++){
		img[i] = 0;
	}
	for(int i = 0; i < N; i++){
		int index = (int) (rint(a[i].x) * size_8 + rint(a[i].y)); //real index
		int byte_id = (int)index/8; // identify the corresponding byte
		int byte_offset = index % 8; //identify the offset from bit 0 to the correct bit
		img[byte_id] |= (128 >> byte_offset);	
	}
	
	ofstream ofs(tmpBuffer);
	ofs << "P4" << endl;
	ofs << "#Image generated by instance_generator - Mirko Bez" << endl;
	ofs << size_8 << " " << size_8 << endl;
	for(int i = 0; i < size_8; i++){
		for(int j = 0; j < byte_num; j++){
			ofs << (unsigned char) img[i*byte_num + j];
		}
	}
	ofs.close();
	delete[] img;
	delete[] tmpBuffer;
}



/**
 * print the instance generated in a PBM file. 
 * (N.B. It is pure ASCII, please use the function printBinaryPBM)
 * @param size the size of the grid (I assume it is squared)
 * @param N the number of points
 */
void printPBM(const int size, Point * a, int N){
	unsigned char * img = new unsigned char[size*size];
	for(int i = 0; i < size*size; i++){
		img[i] = 0;
	}
	for(int i = 0; i < N; i++){
		img[(int) (rint(a[i].x) * size + rint(a[i].y))] = 1;
	}
	
	ofstream ofs("/tmp/output.pbm");
	ofs << "P1" << endl;
	ofs << "#Image generated by instance_generator - Mirko Bez" << endl;
	ofs << size << " " << size << endl;
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			ofs << (char) (img[i*size + j]+48); //48 = '0' in ASCII-code
		}
		ofs << endl;
	}
	ofs.close();
	delete[] img;
}

/**
 * parse the command line input
 * @param opt a char pointer in which the function store the option 
 * 'q' (square), 'r' (random - default), 'c' (circle)
 * @return N the number of points to generate
 */
int get_option(int argc, char * argv[], char * opt){
	stringstream ss;
	int N = 0;
	if(argc < 2){
		cerr << "Usage: " << argv[0] << " N <r|q|c>" << endl;
		exit(EXIT_FAILURE);
	}
	else if(argc < 3){
		ss << argv[1];
		ss >> N;
		*opt = 'r'; //Default is random
	} 
	else if(argc == 3){
		ss << argv[1] << " " << argv[2];
		ss >> N;
		ss >> *opt;
	} else {
		cerr << " Too much arguments given. Exit ... " << endl;
	}
	switch(*opt){
		case 'r':
		case 'q':
		case 'c': break;
		default:
			cerr << "Invalid option '" << * opt << "' given" << endl;
			cerr << "Usage: " << argv[0] << " N <r|q|c>" << endl;
			exit(EXIT_FAILURE);
	}
	return N;
}


/**
 * generate a random double between the given min and max
 * @param min
 * @param max
 * @return the random generated value
 */
double randomDouble(double min, double max){
	double d = (double) rand() / RAND_MAX;
	return min + d * (max - min);
}


/**
 * generate a random instance
 * @param N the number of points to generate
 * @param size the size of the grid
 * @return an array containing the points
 */
Point * generateRandomInstance(const int N, const int size){
	Point * arr = new Point[N];
	for(int i = 0; i < N; i++){
		arr[i].x = randomDouble(0.0, size);
		arr[i].y = randomDouble(0.0, size);
	}
	return arr;
}


/**
 * given N points it generate 4 circle containing ~N/4 points
 * @param N the number of points
 * @param size the size of the grid
 * @return an array containing the generated points
 */
Point * generateCircleInstance(const int N, const int size){
	Point * centers = new Point[4];
	centers[0] = {size/4.0, size/4.0};
	centers[1] = {size/4.0, 3.0*size/4.0};
	centers[2] = {3.0*size/4.0, size/4.0};
	centers[3] = {3.0*size/4.0, 3.0*size/4.0};
	Point * points = new Point[N];
	double radius = size/15;
	int pointsPerCircle[4] = {N/4, N/4, N/4, N/4};
	int mod = N%4;
	for(int i = 0; i < mod; i++){
		pointsPerCircle[i]++;
	}
	
	int k = 0;
	for(int i = 0; i < 4; i++){
		double angle = 0;
		double step = 2*M_PI / pointsPerCircle[i];
		for(int j = 0; j < pointsPerCircle[i]; j++){
			points[k].x = cos(angle)*radius + centers[i].x;
			points[k].y = sin(angle)*radius + centers[i].y;
			angle+=step;
			k++;
		}
	}
	
	
	delete [] centers;
	return points;
}


/**
 * generate an instance in which the points are grouped in 4 square very distant from each other
 * @param N the number of points to generate (N should be greater than 4)
 * @param size the size of the square. For simplicity it is a perfect square
 * @return an array containing the generated points
 */
Point * generateSquareInstance(const int N, const int size){
	Point * arr = new Point[N];
	int numOfSquare = 4;
	int numOfPointsPerSquare = N/numOfSquare;
	int extraPoints[4] = {0,0,0,0};
	int numOfExtraPoints = N % 16;
	int ind = 0;
	for(int i = 0; i < numOfExtraPoints; i++){
		extraPoints[ind++]++;
		ind = ind%4;
	}
	for(int i = 0; i < 4; i++){
		cout << "EXTRA" << extraPoints[i] << endl;
	}
	Point * centers = new Point[numOfSquare];
	centers[0].x = size/4;
	centers[0].y = size/4;
	centers[1].x = 3*size/4;
	centers[1].y = size/4;
	centers[2].x = size/4;
	centers[2].y = 3*size/4;
	centers[3].x = 3*size/4;
	centers[3].y = 3*size/4;
	int k = 0;
	for(int i = 0; i < numOfSquare && k < N; i++){
		
		double sizeOfSquare = size/10;
		/*
		 * + represents a point. With 16 we have 5 points per row. 
		 *  + - + - + - + - +
		 *  -               -
		 *  +               +
		 *  -               -
		 *  +               +
		 *  -               -
		 *  +               +
		 *  -               -
		 *  + - + - + - + - +
		 */
		
		int numOfInnerPointsPerEdge = (numOfPointsPerSquare-4)/4;
		cout << numOfExtraPoints << " " << numOfInnerPointsPerEdge << endl;
		int numOfPointsPerEdge = 2 + numOfInnerPointsPerEdge;
		double step = ((double)sizeOfSquare*2)/numOfPointsPerEdge;
		Point pos = centers[i];
		cout << "centers" << i;
		centers[i].print();
		
		cout << endl;
		cout << "STEP" << step << endl;
		//put cursor to top-left point of topmost leftmost square
		pos.x -= ((double) sizeOfSquare)/2;
		pos.y += ((double) sizeOfSquare)/2;
		
		for(int j = 0; j < numOfInnerPointsPerEdge+1 && k < N; j++){
			arr[k++] = pos;
			pos.x += step;
			
		} //TILL TOP-RIGHT Point
		for(int j = 0; j < numOfInnerPointsPerEdge+1 && k < N; j++){
			arr[k++] = pos;
			pos.y -= step;
		} //TILL BOTTOM-RIGHT Point
		for(int j = 0; j < numOfInnerPointsPerEdge+1 && k < N; j++){
			arr[k++] = pos;
			pos.x -= step;
		} //TILL BOTTOM-LEFT Point
		for(int j = 0; j < numOfInnerPointsPerEdge+1 && k < N; j++){
			arr[k++] = pos;
			pos.y += step;
		} //TILL TOP-LEFT (excluding the topmost-leftmost element that was already added.
		
		
		if(extraPoints[i] == 0)
			continue;
		else if(extraPoints[i] == 1){
			cout << "HERE" << endl;
			arr[k++] = centers[i];
		}
		else if(extraPoints[i] == 2){
			pos = centers[i];
			pos.x -= ((double)sizeOfSquare)/4;
			pos.y += ((double)sizeOfSquare)/4;
			arr[k++] = pos;
			pos =centers[i];
			pos.x += ((double)sizeOfSquare)/4;
			pos.y -= ((double) sizeOfSquare)/4;
			arr[k++] = pos;
		}
		
	}
	for(int i = 0; i < N; i+=1)
		arr[i].print();
	delete[] centers;
	return arr;
}


/**
 * Depending on opt it calls the right instance generator method
 * @param N the number of nodes
 * @param opt the option flag, one of 'q'|'r'|'c'
 */
void generateInstance(int N, char opt){
	const int size = 10*N;
	Point * arr;
	double * cost;
	char * tmpBuffer = new char[128];
	char * type = new char[32];
	switch(opt){
		case 'r': 
			arr = generateRandomInstance(N, size);
			strcpy(type, "random");
			break;
		case 'q': 
			arr = generateSquareInstance(N, size);
			strcpy(type, "square");			
			break;
		case 'c': 
			strcpy(type, "circle");			
			arr = generateCircleInstance(N, size);
			break;
		default: 
			
			exit(EXIT_FAILURE);
	}
	
	sprintf(tmpBuffer, "tsp_instance_%d_%s", N, type);
	printBinaryPBM(size, arr, N, tmpBuffer);
	printGerberFile(arr, N, tmpBuffer);
	cost = calculateDistance(N, arr);
	printDatFile(cost, N, tmpBuffer);
	delete[] arr;
	delete[] cost;
	delete[] tmpBuffer;
	delete[] type;
}

/**
 * the main function
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS on success
 */
int main(int argc, char * argv[]){
	srand(time(NULL));
	char opt;
	int N = get_option(argc, argv, &opt);
	cout << " N " << N << " OPT " << opt << endl;
	generateInstance(N, opt);
	
	return EXIT_SUCCESS;
}

