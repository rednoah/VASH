#include "vash.hh"

using namespace std;

int main( int argc, char ** argv ){
	bool train = false;
	bool test = false;
	bool clustering = false;
	bool combine = false;

	char fileset[128];
	char testfile[128];
	char clusterfile[128];
	char trainfile[128];

	int cluster_k = CLUSTER_K;	//The number of visual words we use


	/* Parse parameters */
	if( argc < 3 || argc > 5  ) return -1;	//Not enough parameters passed

	if( strcmp( argv[1], "train" ) == 0 ){
		train = true;
		strcpy( fileset, argv[2] );		//A textfile specifying all videos to train
		strcpy( clusterfile, argv[3] );
	}

	if( strcmp( argv[1], "test" ) == 0 ){
		test = true;
		strcpy( testfile, argv[2] );	//A video to test
		if( argc == 5 ){
			strcpy( clusterfile, argv[3] );
			strcpy( trainfile, argv[4] );
		}
	}

	if( strcmp( argv[1], "cluster" ) == 0 ){
		clustering = true;
		sscanf( argv[2], "%d", &cluster_k );
		strcpy( fileset, argv[3] );
	}

	if( strcmp( argv[1], "combine" ) == 0 ){	//This does both Clustering AND training.
		combine = true;
		sscanf( argv[2], "%d", &cluster_k );
		strcpy( fileset, argv[3] );		
	}


	/* Used in clustering & training */
	vector<pair<MovieFile,vector<SIFTFeature> > > dataset;	//The database to be trained
	vector<SIFTFeature> centroids;							//The centroids


	/* Standalone Clustering */
	if( clustering || combine ){
		vector<SIFTFeature> clustering;										//Copy of features for clustering alone

		readWorkset( fileset, clustering, dataset, true, combine, NO_FRAMES_SKIP );	//Change the last parameter to not train all files

		//Run k-means on ALL SIFT Features to generate words
		KMeansClustering c(cluster_k);
		c.lloyds( clustering );

		clustering.clear();

		c.getCentroids( centroids );
		
		//Save centroids (Visual Words) to File
		char centroidFile[128];
		strcpy( centroidFile, "workset/centroids_k" );
		strcat( centroidFile, argv[2] ); 				//(The number of centroids)
		strcat( centroidFile, ".db" );

		saveCentroids( centroids, centroidFile );
	}

	/* Standalone Training phase */
	/* Requires clustering to be completed! */
	if( train || combine ){

		if( !combine ){				//If this is not clustering + training combined, we need to load workset and centroids
			char centroidFile[128];
			if( argc == 4 )
				strcpy( centroidFile, clusterfile );
			else
				strcpy( centroidFile, CENTROID_FILE );

			loadCentroids( centroids, centroidFile );

			vector<SIFTFeature> dummy;	//Since we dont do clustering
			readWorkset( fileset, dummy, dataset, false, true, NO_FRAMES_SKIP );
			cout << "Finished reading workset" << endl;
		}

		//Save visual word representation to a database
		char datasetFile[128];
		strcpy( datasetFile, DATASET_FILE );

		vector<pair<MovieFile, vector<VisualWord> > > db;

		for( vector<pair<MovieFile,vector<SIFTFeature> > >::iterator it = dataset.begin(); it != dataset.end(); it++ ){
			//For each movie
			vector<VisualWord> v;

			//Generate a histogram of Visual Words: <id, #occurences>
			generateImageDescription( v, it->second, centroids );	//Actually more like MovieDescription, see TODO above.

			db.push_back( make_pair( it->first, v ) );
		}

		saveDatabase( db, datasetFile );
	}

	/* Test phase */
	if( test ){
		//Decode test video, train SIFT Features
		Movie m( testfile );
		cBitmap b;
		vector<SIFTFeature> sift;

		//What attacks do we want to use?
		Noise noiseInfo;
		memset( &noiseInfo, 0, sizeof(Noise) );
		noiseInfo.gaussian_heavy = true;


		m.seekNextSection( NO_FRAMES_SKIP );
		int frames = 0;
		double percentage = NO_FRAMES_SKIP;

		while( m.loadNextFrame( b ) ){
			//Do noise attacks here in evaluation
			
			addNoise( b, noiseInfo );

			//Parse SIFT keypoints in Bitmap b
			processSIFTPoints( sift, b );

			//Select 10 Frames at each percentage (0.25, 0.5, 0.75)
			//For simplicity, need to keep track of percentage here, not in Movie!
			if( ++frames == NUM_FRAMES_PER_SEC ){
				frames = 0;
				percentage += NO_FRAMES_SKIP;
				if( percentage >= 1 )
					break;
				else
					m.seekNextSection( NO_FRAMES_SKIP );
			}
		}

		//Load VW, translate features
		char centroidFile[128];
		char datasetFile[128];

		if( argc != 5 ){
			strcpy( datasetFile, DATASET_FILE );
			strcpy( centroidFile, CENTROID_FILE );
		}else{
			strcpy( datasetFile, trainfile );
			strcpy( centroidFile, clusterfile );
		}

		loadCentroids( centroids, centroidFile );

		vector<VisualWord> v;

		generateImageDescription( v, sift, centroids );

		sift.clear();
		centroids.clear();

		//Compare representation to others in database

		vector<pair<MovieFile, vector<VisualWord> > > db;
		loadDatabase( db, datasetFile );

		double min_dist = HUGE_VAL;
		vector< pair<MovieFile, vector<VisualWord> > >::iterator closest_match;

		for( vector< pair<MovieFile, vector<VisualWord> > >::iterator it = db.begin(); it != db.end(); it++ ){
			double d = vw_block_distance( it->second, v );

			if( d < min_dist ){
				min_dist = d;
				closest_match = it;
			}
		}

		//Output result
		cout << "Best match is: " << closest_match->first.filename << " (ID: " << closest_match->first.id << ")" << endl;
		cout << "Distance to best match was: " << min_dist << endl;
	}

	return 0;
}


/* Read a workset file. Used when clustering or training */
void readWorkset( char * fileset, vector<SIFTFeature> & clustering, vector<pair<MovieFile,vector<SIFTFeature> > > & db, bool cluster, bool train, double percentage ){

	//Decode all Videos, train SIFT Features
	ifstream queries;
	queries.open(fileset, ifstream::in|ifstream::binary );  //Contains location of all movies

	if( !queries.is_open() ){	
		cerr << "Could not open workset" << endl;
		exit(1);		//Could not open file
	}

	queries.seekg( queries.beg );

	int i = 0;
	int frames = 0;
	double p = 0;

	vector<SIFTFeature> sift;
	
	if( percentage >= 1.0 )
		percentage = 0.0;

	while( true ){
		char filename[128];
		queries.getline( filename, 128 );

		if( !queries ) break;

		cout << "Reading file" << filename << endl;

		cBitmap b;
		Movie m( filename );
		MovieFile f( filename, i++ );

		if( percentage > 0.0 ){						//If _not_ all data is to be trained (i.e., choose only 10 frames at 25% steps)
			m.seekNextSection( percentage );
			frames = 0;
			p = percentage;
		}

		int loop = 0;

		//Load a frame from the movie
		while( m.loadNextFrame( b ) ){
			if( ++loop%100 == 0 )
				cout << "Processing frame " << loop << endl;

			vector<SIFTFeature> tmp;

			//Process its SIFT keypoints
			processSIFTPoints( tmp, b );

			//If we want to do clustering, save in 'clustering'
			if( cluster )
				clustering.insert( clustering.end(), tmp.begin(), tmp.end() );	

			//Same with training
			if( train )
				sift.insert( sift.end(), tmp.begin(), tmp.end() );

			tmp.clear();

			if( percentage > 0 ){
				//Select 10 Frames at each percentage (e.g., at 0.25, 0.5, 0.75, NOT at 0.0 or 1.0)
				//For simplicity, need to keep track of percentage here, not in Movie!
				if( ++frames == NUM_FRAMES_PER_SEC ){
					frames = 0;
					p += percentage;
					if( p >= 1 )
						break;
					else
				m.seekNextSection( percentage );
				}
			}
		}

		cout << "Processed " << loop << " frames" << endl;

		if( train ){
			db.push_back( make_pair( f, sift ) );		//Save a copy of MovieFile struct (which saves information like title) and the features
			sift.clear();
		}

		if( cluster )
			cout << "Clustering size is: " << clustering.size() << endl;
		
	}

	queries.close();
}

void generateImageDescription( vector<VisualWord> & description, vector<SIFTFeature> & features, vector<SIFTFeature> & centroids ){
	int * occurences = new int[centroids.size()];
	memset( occurences, 0, centroids.size()*sizeof(int) );

	for( vector<SIFTFeature>::iterator it = features.begin(); it != features.end(); it++ ){
		double min_distance = HUGE_VAL;
		int min_index = -1;

		for( unsigned int i = 0; i < centroids.size(); i++ ){
			double d = sift_block_distance( *it, centroids[i] );

			if( d < min_distance ){
				min_distance = d;
				min_index = i;
			}
		}
		occurences[min_index]++;
	}

	
	for( unsigned int i = 0; i < centroids.size(); i++ ){
		if( occurences[i] == 0 ) continue;
		VisualWord w;
		w.id = i;
		w.occurences = occurences[i];
		description.push_back( w );
	}

	delete[] occurences;
}

//For evaluation, we try to distort our images
void addNoise( cBitmap & b, Noise noiseInfo ){
    char tmpfile[64];
	char command[128];

	strcpy( tmpfile, "/tmp/vashImage.bmp3" );	//mogrify will output v4 Header if ending is not .bmp3, which my Bitmap class does not parse correctly atm ;)
	strcpy( command, "rm " );
	strcat( command, tmpfile );

	if( system( command ) != 0 ){
		cerr << "Could not delete tmp file!" << endl;
	}
	
	b.saveBitmap( tmpfile );

	if( noiseInfo.gaussian_
heavy ){
		strcpy( command, "../util/mogrifiers/blur-heavy.sh " );
		strcat( command, tmpfile );

		int r = system( command );

		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;
	}

	if( noiseInfo.gaussian_light ){
		strcpy( command, "../util/mogrifiers/blur-light.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.motion ){
		strcpy( command, "../util/mogrifiers/blur-light-motion.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.radial ){
		strcpy( command, "../util/mogrifiers/blur-light-radial.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.crop ){
		strcpy( command, "../util/mogrifiers/crop-black.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.logo ){
		strcpy( command, "../util/mogrifiers/logo.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.sharpen_heavy ){
		strcpy( command, "../util/mogrifiers/sharpen-heavy.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.sharpen_light ){
		strcpy( command, "../util/mogrifiers/sharpen-light.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	if( noiseInfo.subtitles ){
		strcpy( command, "../util/mogrifiers/subs.sh " );
		strcat( command, tmpfile );

		int r = system( command );
		if( r != 0 ) cerr << "Encountered an error while executing mogrify script!" << endl;

	}

	b.loadBitmap( tmpfile );
}


//TODO: This should also save the VisualWords (->Merge centroids into VisualWord struct)
void saveCentroids( vector<SIFTFeature> & c, char * filename ){
	std::ofstream data;
	data.open(filename, ifstream::out|ifstream::binary );

	/* Only saving one orientation for now */
	for( vector<SIFTFeature>::iterator it = c.begin(); it != c.end(); it++ ){
		data.write( reinterpret_cast<char *>(&(*it)), sizeof(SIFTFeature) );
	}
	data.close();
}

void loadCentroids( vector<SIFTFeature> & c, char * filename ){
	std::ifstream data;
	data.open( filename, ifstream::in|ifstream::binary );

	if( !data ) exit(1);	//Could not open file

	/* Only loading one orientation for each SIFTFeature */
	while( true ){
		SIFTFeature f;
		memset( &f, 0, sizeof( SIFTFeature ) );

		data.read( reinterpret_cast<char *>(&f), sizeof(SIFTFeature) );

		if( !data ) break;	//EOF bit is only set after eof was reached DURING a read. If the previous loop ended on the last byte, no eof is set.

		c.push_back( f );
	}
	data.close();

	cout << c.size() << " Centroid were loaded!" << endl;

}

void loadDatabase( vector< pair<MovieFile, vector<VisualWord> > > & db, char * filename ){
	std::ifstream data;
	data.open( filename, ifstream::in|ifstream::binary );

	if( !data ) exit(1);

	while( true ){
		MovieFile mf;
		vector<VisualWord> vv;

		int objects = 0;

		data.read( reinterpret_cast<char *>(&mf), sizeof(MovieFile) );

		if( !data ) break;	//EOF bit is only set after eof was reached DURING a read. If the previous loop ended on the last byte, no eof is set.

		data.read( reinterpret_cast<char *>(&objects), sizeof(int) );

		for( int i = 0; i < objects; i++ ){
			VisualWord v;
			data.read( reinterpret_cast<char *>(&v), sizeof(VisualWord) );
			vv.push_back( v );
		}
		db.push_back( make_pair( mf, vv ) );
	}
}

void saveDatabase( vector< pair<MovieFile, vector<VisualWord> > > & db, char * filename ){
	std::ofstream data;
	data.open( filename, ifstream::out|ifstream::binary );

	for( vector< pair<MovieFile, vector<VisualWord> > >::iterator it = db.begin(); it != db.end(); it++ ){
		int num = it->second.size();
		data.write( reinterpret_cast<char *>(&(it->first)), sizeof(MovieFile) );
		data.write( reinterpret_cast<char *>(&num), sizeof(int) );
		for( vector<VisualWord>::iterator fit = it->second.begin(); fit != it->second.end(); fit++ ){
			data.write( reinterpret_cast<char *>(&(*fit)), sizeof(VisualWord) );
		}
	}

	data.close();
}


/* For testing purposes, let's generate a few SIFT keypoints here. */
void processSIFTPoints( vector<SIFTFeature> & storage, cBitmap & bitmap ){

	int number_pixels = bitmap.getWidth()*bitmap.getHeight();

	unsigned char * buffer = new unsigned char[number_pixels];
	bitmap.getGreyscaleBitmap( buffer, number_pixels );		//SIFT accepts greyscale only

	/* Convert to correct format (float/vl_sift_pix) */
	vl_sift_pix * im = new vl_sift_pix[number_pixels];

	for( int i = 0; i < bitmap.getWidth()*bitmap.getHeight(); i++ )
		im[i] = static_cast<vl_sift_pix>( buffer[i]/256.0f );		//From testing: Both scaling to [0,1] or leaving at [0,255] are acceptable. vl_sift_pix = float.
	
	delete[] buffer;

	#ifdef DEBUG
	unsigned char * debug_im = new unsigned char[number_pixels*sizeof(Pixel)];
	bitmap.getBitmap( debug_im, number_pixels*sizeof(Pixel) );
	int iteration = 0;
	#endif

	/* Create SIFT Filter object */
	VlSiftFilt * s = vl_sift_new( bitmap.getWidth(), bitmap.getHeight(), 3, 3, 0 );
	vl_sift_process_first_octave( s, im );

	do{	
		/* Detect and retrieve keypoints in this octave */
		vl_sift_detect( s );
		const VlSiftKeypoint * keypoint = vl_sift_get_keypoints( s );
		int num_keys = vl_sift_get_nkeypoints( s );

		#ifdef DEBUG
		cout << num_keys << " keypoints were detected!" << endl;
		#endif

		for( int i = 0; i < num_keys; i++ ){
			#ifdef DEBUG
			cout << "Keypoint at x=" << ceil(keypoint[i].x) << ", y=" << ceil(keypoint[i].y) << ":" << endl;
			int coord = ceil(keypoint[i].x)+(ceil(keypoint[i].y)*bitmap.getWidth());				//For visualization of keypoints, you can set an appropriate color here
			debug_im[coord*4] = 255; debug_im[coord*4+1] = 255;
			debug_im[coord*4+2] = 0; debug_im[coord*4+3] = 0;
			#endif

			double angles[4];
			int num_angles = vl_sift_calc_keypoint_orientations( s, angles, &(keypoint[i]) );

			#ifdef DEBUG
			cout << "\t" << num_angles << " orientations found!" << endl;
			#endif

			for( int j = 0; j < num_angles; j++ ){
				SIFTFeature f;

				/* Get 128 bin histogram */
				vl_sift_pix * output = new vl_sift_pix[128];
				memset( output, 0, 128*sizeof( vl_sift_pix ) );

				vl_sift_calc_keypoint_descriptor( s, output, &(keypoint[i]), angles[j] );

				//Save feature here!
				memcpy( f.histogram, output, 128*sizeof( vl_sift_pix ) );
				storage.push_back( f );

				delete[] output;
			}

		}
	
		#ifdef DEBUG
		cout << "Iteration " << ++iteration << endl;
		#endif
	}while( vl_sift_process_next_octave( s ) != VL_ERR_EOF );

	#ifdef DEBUG
	glutViewer( debug_im, bitmap.getWidth(), bitmap.getHeight(), 4, 0, NULL, 600, 600 );	//Displays a bitmap image. See viewer.cc/hh
	//glutViewer is a blocking call. Program will stop here until the window is closed
	delete[] debug_im;
	#endif

	vl_sift_delete( s );
}

