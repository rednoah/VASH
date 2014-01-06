#include "math.hh"

double sift_block_distance( SIFTFeature a, SIFTFeature b ){
	double d = 0;
	for( unsigned int i = 0; i < 128; i++ )
		d += std::abs(a.histogram[i] - b.histogram[i]);
	return d;
}

double vw_block_distance( vector<VisualWord> & a, vector<VisualWord> & b ){
	double d = 0;

	if( a.size() != b.size() ){
		cerr << "VisWord vectors are not of equal length! (->Not enough keypoints)" << endl;	
		cout << a.size() << "/" << b.size() << endl;
	}

	for( unsigned int i = 0; i < a.size(); i++ ){
		d += std::abs(a[i].occurences - b[i].occurences);
	}

	return d;
}
