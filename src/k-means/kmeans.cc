#include "kmeans.hh"

using namespace std;

#ifdef STANDALONE
/* Main entry method for testing */
int main( int argc, char ** argv ){
	vector<SIFTFeature> dataset;
	vector<SIFTFeature> sift;

	/* Testing area */
	processSIFTPoints( sift );							//Call SIFT library, see above
	dataset = sift;										//Load keypoints

	KMeansClustering c(CLUSTER_K);						//Create Clustering object
	c.lloyds( dataset );								//Run Clustering algorithm

	cout << "Clustering done" << endl;

	vector<VisualWord> test;							//To save the result of lookup
	c.generateImageDescription( test, dataset );		//Quantize a feature vector by finding closest centroid (=keyword)

	#ifdef DEBUG
	for( vector<VisualWord>::iterator it = test.begin(); it != test.end(); it++ )
		cout << "Visual Word " << it->id << " occured " << it->occurences << " times." << endl;
	#endif

	return 0;
}
#endif

KMeansClustering::KMeansClustering( int k ){
	this->k = k;
	centroids = new SIFTFeature[k];
}

KMeansClustering::~KMeansClustering(){
	delete[] centroids;
	dataset.clear();
}

/* Here we will load the SIFT features into memory */
void KMeansClustering::loadDataset( vector<SIFTFeature> & db ){
	loadRandomDataset( db );
}


//For this test, we'll load some random data
//Clustering random data? Good idea...lol
void KMeansClustering::loadRandomDataset( vector<SIFTFeature> & db ){
	std::mt19937 generator( 0 );	//Or another seed. But this is just a test, so...
	std::normal_distribution<double> distribution( 0.0, 1.0 );

	int dim = 128;		//Test: Dimensionality of each keypoint
	int elem = 1000;	//Test: Number of keypoints

	for( int i = 0; i < elem; i++ ){
		SIFTFeature s;
		memset( &s, 0, sizeof( SIFTFeature ) );

		for( int j = 0; j < dim; j++ ){
  			s.orientations[0].histogram[j] = distribution(generator);
		}

		db.push_back( s );
	}
}

void KMeansClustering::generateImageDescription( vector<VisualWord> & description, vector<SIFTFeature> & features ){
	int * occurences = new int[features.size()];
	memset( occurences, 0, sizeof(int)*features.size() );

	/* First, generate the Visual Words by comparing to the clustering */
	for( vector<SIFTFeature>::iterator it = features.begin(); it != features.end(); it++ ){
		VisualWord w;
		convertToVisualWord( w, *it );
		occurences[w.id]++;	//We are interested in the number of occurences of this word in the image
	}

	/* Assemble <id,occurences> pair and save */
	for( unsigned int i = 0; i < features.size(); i++ ){
		if( occurences[i] == 0 ) continue;
		VisualWord w;
		w.id = i;
		w.occurences = occurences[i];
		description.push_back( w );
	}

	delete[] occurences;
}


/*  Convert the SIFTFeature given in @feature to a visual word in @result */
/* Can this be sped up with hashing? */
void KMeansClustering::convertToVisualWord( VisualWord & result, SIFTFeature & feature ){
	double min_distance = HUGE_VAL;
	int min_index = -1;

	for( int i = 0; i < k; i++ ){
		double d = sift_block_distance( feature, centroids[i] );

		if( d < min_distance ){
			min_distance = d;
			min_index = i;
		}
	}

	result.id = min_index;
}


//Clustering algorithm
void KMeansClustering::lloyds( vector<SIFTFeature> & db ){
	int * c = new int[k];
	int * assignment = new int[db.size()];
	memset( assignment, 0, db.size()*sizeof( int ) );

	#ifdef LSH
	LSHasher lsh( k );
	vector<SIFTFeature> buckets[k];

	for( unsigned int i = 0; i < db.size(); i++ ){
		double tmp[128];
		for( int j = 0; j < 128; j++ )	tmp[j] = static_cast<double>(db[i].orientations[0].histogram[j]);

		int index = lsh.hash( tmp, 128, 5 );
		buckets[index].push_back( db[i] );
	}

	int j = 0;
	while( j < k ){
		for( int i = 0; i < k; i++ ){
			if( buckets[i].empty() ) continue;

			centroids[j++] = buckets[i][0];
			if( j >= k ) break;
		}
	}
	#else
	//Should we try to speed up initialisation by hashing?
	//Possibly: Hash all features, choose k buckets as centroids
	//But will it be faster?	=> Test with code above: Not faster, possibly even slower..
	for( int i = 0; i < k; i++ )
		getUniqueUniformRandom( c, i, db.size() );	//Depending on k and db.size(), you may also assign w/o checking for uniqueness
	
	for( int i = 0; i < k; i++ )
		centroids[i] = db[c[i]];
	#endif

	int loop = 0;
	//k-means is proven to have an upper bound in the number of iterations
	//If this goes to infinite loop, there is a bug!
	while( doIteration( db, assignment ) ){ loop++; }	

	cout << "Required iterations were: " << loop << endl;

	//At this point, assignment holds our choosen centroids/words
	//centroids can be used to determine the appropriate word for a keypoint

	delete[] assignment;
	delete[] c;
}

/* Finds a random number in [0,limit] which has not been drawn before, i.e., is unique */
/* The point here is that we want k clusters without duplicates */
void KMeansClustering::getUniqueUniformRandom( int * data, int index, int limit ){
	int seed = (index+1)*time( NULL );

	std::default_random_engine generator( seed );
	std::uniform_int_distribution<int> distribution( 0, limit );

	while( true ){
		int r = distribution(generator);
		bool fresh = true;	//Fixed a bug here

		for( int j = 0; j < index; j++ )
			if( data[j] == r ){
				fresh = false; 
				break;
			}

		if( fresh ){
			data[index] = r;
			break;
		}
	}
}

//One iteration of k-means (lloyds)
bool KMeansClustering::doIteration( vector<SIFTFeature> & db, int * assignment ){
	int * old_assignment = new int[db.size()];
	memcpy( old_assignment, assignment, db.size()*sizeof( int ) );

	vector<SIFTFeature> * inverted_list = new vector<SIFTFeature>[k];		//This makes updating the centroids somewhat easier

	for( vector<SIFTFeature>::iterator it = db.begin(); it != db.end(); ++it ){
		
		double min_dist = HUGE_VAL;
		int best_index = 0;

		//Compute distances to all current centroids
		for( int i = 0; i < k; i++ ){
			double d = sift_block_distance( *it, centroids[i] );
			if( d < min_dist ){
				min_dist = d;
				best_index = i;
			}
		}

		assignment[it-db.begin()] = best_index;
		inverted_list[best_index].push_back( *it );
	}
	
	/* Update centroids */
	//For each of the k clusters ...
	for( int i = 0; i < k; i++ ){
		memset( &centroids[i], 0, sizeof( SIFTFeature ) );	//Set the mean to zero

		//Sum up vectors
		for( vector<SIFTFeature>::iterator it = inverted_list[i].begin(); it != inverted_list[i].end(); ++it )
			addSIFT( centroids[i], *it );

		//Scale them by number of vectors (-> arithmetic mean)
		divideSIFT( centroids[i], inverted_list[i].size() );
	}

	/* Check if this iteration changed anything */
	bool changes = false;

	for( unsigned int i = 0; i < db.size(); i++ ){
		if( assignment[i] != old_assignment[i] ){
			changes = true;
			break;
		}
	}


	for( int i = 0; i < k; i++ )
		inverted_list[i].clear();

	delete[] inverted_list;
	delete[] old_assignment;
	return changes;
}

//Add the SIFT feature vector of b onto a
void KMeansClustering::addSIFT( SIFTFeature & a, SIFTFeature b ){
	for( int i = 0; i < 128; i++ )
		a.orientations[0].histogram[i] += b.orientations[0].histogram[i];
	
}

//Divide SIFT vector a by the scalar b
void KMeansClustering::divideSIFT( SIFTFeature & a, double b ){
	for( int i = 0; i < 128; i++ ){
		a.orientations[0].histogram[i] /= b;
	}
}

//Choosing block-distance for this test
//In the Google Video paper they use Mahalanobis distance. Maybe implement that too?
double KMeansClustering::sift_block_distance( SIFTFeature a, SIFTFeature b ){
	double d = 0;
	for( int i = 0; i < 128; i++ )
		d += std::abs(a.orientations[0].histogram[i] - b.orientations[0].histogram[i]);
	return d;
}

void KMeansClustering::getCentroids( vector<SIFTFeature> & sift ){
	for( int i = 0; i < k; i++ ){
		sift.push_back( centroids[i] );
	}
}
