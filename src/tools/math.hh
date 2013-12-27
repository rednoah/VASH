#ifndef MATH_HH
#define MATH_HH

#include "../vash.hh"

using namespace std;

double sift_block_distance( SIFTFeature a, SIFTFeature b );
double vw_block_distance( vector<VisualWord> & a, vector<VisualWord> & b );

#endif
