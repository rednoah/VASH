#ifndef KMEANS_HH
#define KMEANS_HH

#include "../vash.hh"
#include "../lsh/lsh.hh"


#define CLUSTER_K 100		//Number of clusters = visual words
#define MAX_ITERATIONS 50	//Limit on number of k-means iterations

using namespace std;


class KMeansClustering{

public:
	KMeansClustering( int k );
	~KMeansClustering();

	//Placeholder
	//Load all SIFT features into the vector structure
	void loadDataset( vector<SIFTFeature> & db );
	void loadRandomDataset( vector<SIFTFeature> & db );	//This one for testing purposes only!

	//Generate a description for a whole image represented by a vector of features generated previously
	void generateImageDescription( vector<VisualWord> & description, vector<SIFTFeature> & features );
	
	//Find the Visual word for a single feature
	void convertToVisualWord( VisualWord & result, SIFTFeature & feature );

	//Used to initialize centroids
	void getUniqueUniformRandom( int * data, int index, int limit );

	//"Lloyds" k-Means clustering
	void lloyds( vector<SIFTFeature> & db );

	//Do a single iteration of "Lloyds" algorithm.
	bool doIteration( vector<SIFTFeature> & db, int * assignment );
	void divideSIFT( SIFTFeature & a, double b );		//Divide SIFT vector a by the scalar b
	void addSIFT( SIFTFeature & a, SIFTFeature b );		//Add the SIFT feature vector of b onto a

	//Block distance for SIFT feature
	double sift_block_distance( SIFTFeature a, SIFTFeature b );

	void getCentroids( vector<SIFTFeature> & sift );
private:
	vector<SIFTFeature> dataset;
	SIFTFeature * centroids;
	int k;

};
#endif
