#ifndef LSH_HH
#define LSH_HH

#include <random>
#include <ctime>
#include <cmath>

using namespace std;

class LSHasher{
public:
	LSHasher( int scale );
	LSHasher( int scale, double sigma, double mu );

	int hash( double * feature, int dim, int num_hashes );
	void seed( int seed );
private:
	void generateRandomNormal( double * w, int dim );
	double dotProduct( double * w, double * feature, int dim );

	mt19937 generator;
	normal_distribution<double> distribution;
	int scale;
};

#endif
