#ifndef KMEANS_HH
#define KMEANS_HH

#include <vector>
#include <random>
#include <ctime>

#include <cstring>	//For memset etc..

#include <iostream>

#define CLUSTER_K 50		//Number of clusters = visual words
#define MAX_ITERATIONS 50	//Limit on number of k-means iterations

using namespace std;

//Placeholder
//This keeps a descriptor for one orientation
//Therefore, the final vector will be a multiple of this
struct SIFTDescriptor{
 float histogram[128];
};

//Theoretically, there should be only one orientation. But practically, we might have ambiguous results
//SIFT computes up to four possible orientations.
//Important question: In http://www.vlfeat.org/api/sift.html#sift-intro-detector -> Orientation assignment, it sounds like the orientations are ordered.
//Should we just use best orientation?
struct SIFTFeature{
 SIFTDescriptor orientations[4];
};


//Placeholder
//Load all SIFT features into the vector structure
void loadDataset( vector<SIFTFeature> & db );
void loadRandomDataset( vector<SIFTFeature> & db );	//This one for testing purposes only!


//Used to initialize centroids
void getUniqueUniformRandom( int * data, int index, int limit );

//"Lloyds" k-Means clustering
void lloyds( vector<SIFTFeature> & db, SIFTFeature * centroids, int k );

//Do a single iteration of "Lloyds" algorithm.
bool doIteration( vector<SIFTFeature> & db, int * assignment, SIFTFeature * centroids, int k );
void divideSIFT( SIFTFeature & a, double b );
void addSIFT( SIFTFeature & a, SIFTFeature b );

//Block distance for SIFT feature
double sift_distance( SIFTFeature a, SIFTFeature b );

#endif
