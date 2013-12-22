#ifndef VASH_HH
#define VASH_HH

#include <vector>
#include <random>
#include <ctime>

#include <cstring>	//For memset etc..

#include <iostream>

#include "tools/Bitmap.hh"
#include "tools/viewer.hh"

using namespace std;

extern "C" {
#include "sift.h"
}

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
 int num_orientations;
};

//Placeholer for our visual words
struct VisualWord{
	int id;
	int occurences;
};

class MovieFile{
public:
	char filename[128];
	int id;

	MovieFile( char * f, int i ){
		strcpy( filename, f );
		id = i;
	}
};


//TODO: Placeholder, convert decodeMovie to class
class Movie{
public:
	char filename[128];

	Movie( char * f ){
		strcpy( filename, f );
	}

	bool loadNextFrame( cBitmap & b ){
		
		return true;
	}
};



#include "k-means/kmeans.hh"




#endif
