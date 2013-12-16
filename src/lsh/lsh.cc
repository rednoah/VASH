#include "lsh.hh"
#include <iostream>
using namespace std;


LSHasher::LSHasher( int scale ){
	this->scale = scale;
	generator.seed( time(NULL) );
	distribution = normal_distribution<double>( 0.0, 1.0 );
}

LSHasher::LSHasher( int scale, double mu, double sigma ){
	this->scale = scale;
	generator.seed( time(NULL) );
	distribution = normal_distribution<double>( mu, sigma );
}

void LSHasher::generateRandomNormal( double * w, int dim ){
	for( int i = 0; i < dim; i++ )
		w[i] = distribution(generator);
}

/* Evaluates the dot-product */
double LSHasher::dotProduct( double * w, double * feature, int dim ){
	double r = 0;

	for( int i = 0; i < dim; i++ )
		r += (w[i]*feature[i]);

	return r;
}

int LSHasher::hash( double * feature, int dim, int num_hashes ){
	double * w = new double[dim];	//Random vector
	double result = 0;

	for( int i = 0; i < num_hashes; i++ ){
		generateRandomNormal( w, dim );
		result += dotProduct( w, feature, dim );
	}

	#ifdef DEBUG
	cout << "Hashed bucket is: " << x << endl;
	#endif

	delete[] w;

	//In HW#2 we formed binary strings and compared them with hamming, but here I want k buckets, i.e., integers in [0,k-1].
	//Is this a plausible way?
	int x = static_cast<int>(result*(scale/num_hashes))%scale;
	if( x < 0 ) x = scale+x;			//Behavior of modulus for negative numbers is not defined and depends on implementation -.-

	return x;
}

void LSHasher::seed( int seed ){
	generator.seed( seed );
}
