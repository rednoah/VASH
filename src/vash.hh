#ifndef VASH_HH
#define VASH_HH

#include <vector>
#include <random>
#include <ctime>

#include <cstring>	//For memset etc..

#include <iostream>

#include "tools/Movie.hh"
#include "tools/Bitmap.hh"

#ifdef DEBUG
#include "tools/viewer.hh"
#endif

using namespace std;


#define DATASET_FILE "workset/dataset.db"
#define CENTROID_FILE "workset/centroids.db"

extern "C" {
#include "sift.h"
}


struct Noise{
 bool gaussian_heavy;
 bool gaussian_light;
 bool motion;
 bool radial;
 bool crop;
 bool logo;
 bool sharpen_heavy;
 bool sharpen_light;
 bool subtitles;
};

//Theoretically, there should be only one orientation. But practically, we might have ambiguous results
//SIFT computes up to four possible orientations.
//Important question: In http://www.vlfeat.org/api/sift.html#sift-intro-detector -> Orientation assignment, it sounds like the orientations are ordered.
//Should we just use best orientation?
//**** Addition: In case of multiple orientations, add those as separate keypoint.
//**** Wrapped to class
class SIFTFeature{
public:
	vl_sift_pix histogram[128];

	SIFTFeature operator+( SIFTFeature b ){
		SIFTFeature result;
		for( int i = 0; i < 128; i++ )
			result.histogram[i] = histogram[i] + b.histogram[i];

		return result;
	}

	SIFTFeature operator-( SIFTFeature b ){
		SIFTFeature result;
		for( int i = 0; i < 128; i++ )
			result.histogram[i] = histogram[i] - b.histogram[i];

		return result;
	}
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
	
	MovieFile(){}

	MovieFile( char * f, int i ){
		strcpy( filename, f );
		id = i;
	}
};


#include "tools/math.hh"
#include "k-means/kmeans.hh"


/* From main.cc */
void processSIFTPoints( vector<SIFTFeature> & storage, cBitmap & bitmap );

void saveCentroids( vector<SIFTFeature> & c, char * filename );
void loadCentroids( vector<SIFTFeature> & c, char * filename );

void saveDatabase( vector< pair<MovieFile, vector<VisualWord> > > & db, char * filename );
void loadDatabase( vector< pair<MovieFile, vector<VisualWord> > > & db, char * filename );

void addNoise( cBitmap & b, Noise noiseInfo );
#endif
