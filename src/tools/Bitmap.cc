/* Author: Sebastian Agethen */
/* Email: s.agethen@gmail.com */
/* Simple Bitmap Loader for 24 bit and 32 bit Windows Bitmaps */

#include "Bitmap.hh"
#include <iostream>

cBitmap::cBitmap(){
 bmap = NULL;
 width = 0;
 height = 0;
 bpp = 0;
}

cBitmap::cBitmap( char * filename ){
 bmap = NULL;
 width = 0;
 height = 0;
 bpp = 0;

 loadBitmap( filename );
}

cBitmap::~cBitmap(){
 //if( bmap != NULL )
 // free( bmap );
}

int cBitmap::loadBitmap( char * filename ){

 fstream file;
 char buffer[128];

 int cBitmapoffset = 0;
 int dibsize = 0;

 int paddingbytes = 0;
 int flagNegHeight = 0;

 file.open( filename, fstream::in|fstream::binary|fstream::ate );

 if( file == NULL ){
  fprintf( stderr, "Could not open file %s\n", filename );
  return -1;
 }
 
 file.seekg( 0, file.beg );

 file.read( buffer, 14 ); //Read cBitmap header
 memcpy( &cBitmapoffset, buffer+10, 4 );

 file.read( buffer, 4 );
 memcpy( &dibsize, buffer, 4 );

 width = 0;
 height = 0;
 bpp = 0;

 file.read( buffer, 12 );
 memcpy( &width, buffer, 4 );
 memcpy( &height, buffer+4, 4 );
 memcpy( &bpp, buffer+10, 2 );

 if( height < 0 ){ height *= -1; flagNegHeight = 1; }

 switch( dibsize ){
	case 40:
	{
		assert( dibsize == 40 );	//BITMAPINFOHEADER

		file.read( buffer, dibsize-4-12 );	//We already read size, width, height

 		assert( bpp == 24 || bpp == 32 );
 		bpp /= 8;
 
 		paddingbytes = (4-(bpp*width)%4)%4;

 		int sizeimg = 0;
 		memcpy( &sizeimg, buffer+16, sizeof(int) );					//Size of row in bytes must be 0 modulo 4 -> padding if necessary
 		//assert( sizeimg-width*height*3 == paddingbytes*height );		//Apparently not set by all programs
		break;
	}
	case 108:
	{
		exit(1);
		/* BITMAPV4HEADER not really supported yet */
		file.read( buffer, dibsize-4-12 );

		break;
	}
	default:
		exit(1);
 } 

 file.seekg( cBitmapoffset, file.beg );

 bmap = (Pixel *) malloc( width*height*sizeof(struct Pixel) );
 memset( bmap, 0, width*height*sizeof(struct Pixel) );

 if( !bmap ){ fprintf( stderr, "Could not allocate memory\n" ); return -1; }

 int xpos = 0;
 int ypos = height-1;

 if( flagNegHeight )
  ypos = 0;

 while( !file.eof() ){

  if( ypos < 0 || ypos >= height )
   break;
 
  file.read( buffer, bpp );

  if( !file && !file.eof() ){ fprintf( stderr, "An error occured when reading from file\n" ); return -1; }

  bmap[ypos*width+xpos].r = buffer[2];
  bmap[ypos*width+xpos].g = buffer[1];
  bmap[ypos*width+xpos].b = buffer[0];

  if( bpp == 4 )
   bmap[ypos*width+xpos].a = buffer[3];
  else
   bmap[ypos*width+xpos].a = 0;

  xpos = (xpos+1)%width;

  if( xpos == 0 ){
   if( flagNegHeight )
    ypos++;
   else
    ypos--;

   if( paddingbytes )
    file.read( buffer, paddingbytes );
  }
 }

 file.close();
 return 0;
}

int cBitmap::saveBitmap( char * filename ){ 

 char header[14];
 header[0] = 'B';
 header[1] = 'M';

 int filesize = width*height*3;
 memcpy( header+2, &filesize, sizeof( int ) );

 header[6] = 0;
 header[7] = 0;
 header[8] = 0;
 header[9] = 0;

 int start = 54;
 memcpy( header+10, &start, sizeof( int ) );

 char dibheader[40];
 int dibsize = 40;
 memcpy( dibheader, &dibsize, sizeof( int ) );
 memcpy( dibheader+sizeof(int), &width, sizeof(int) );
 memcpy( dibheader+2*sizeof(int), &height, sizeof(int) );
 short planes = 1;
 memcpy( dibheader+3*sizeof(int), &planes, sizeof(short) );
 short bbp = 3*8; //Store in 24bbp format
 memcpy( dibheader+3*sizeof(int)+sizeof(short), &bbp, sizeof(short) );
 int compression = 0;
 memcpy( dibheader+3*sizeof(int)+2*sizeof(short), &compression, sizeof(int) );
 int rawsize = width*height*3;
 memcpy( dibheader+4*sizeof(int)+2*sizeof(short), &rawsize, sizeof(int) );
 int resolution = 2835; // lol
 memcpy( dibheader+5*sizeof(int)+2*sizeof(short), &resolution, sizeof(int) );
 memcpy( dibheader+6*sizeof(int)+2*sizeof(short), &resolution, sizeof(int) );
 int palette = 0;
 memcpy( dibheader+7*sizeof(int)+2*sizeof(short), &palette, sizeof(int) );
 int important = 0;
 memcpy( dibheader+8*sizeof(int)+2*sizeof(short), &important, sizeof(int) );

 int paddingbytes = (4-((width*3)%4))%4;
 const char padding[3] = { 0, 0, 0 };

 fstream output;
 output.open( filename, fstream::out|fstream::binary );

 if( output == NULL ){
   fprintf( stderr, "Could not open file (writeToFile)!\n" );
   exit(1);
 }

 //Write headers
 output.write( header, 14 );
 output.write( dibheader, 40 );

 for( int i = height-1; i >= 0; i-- ){
  for( int j = 0; j < width; j++ ){
   char buffer[4];

   memcpy( buffer, &(bmap[i*width+j]), sizeof(struct Pixel) );

   output.write( buffer+2, 1 );
   output.write( buffer+1, 1 );
   output.write( buffer, 1 );  
  }
  if( paddingbytes ){
   if( !output ) fprintf( stderr, "Some error" );
   output.write( padding, paddingbytes );
  }
 }
 output.close();

return 0; 
}
 
 
int cBitmap::getPixel( int x, int y, Pixel & p ){
 if( x < 0 || x >= width || y < 0 || y >= height ) return -1;

 p.r = (bmap[y*width+x]).r;
 p.g = (bmap[y*width+x]).g;
 p.b = (bmap[y*width+x]).b;
 p.a = (bmap[y*width+x]).a;

 return 0;
}

int cBitmap::setPixel( int x, int y, Pixel p ){
 if( x < 0 || x >= width || y < 0 || y >= height ) return -1;
 bmap[y*width+x] = p;
 return 0;
}

void cBitmap::getBitmap( unsigned char * buffer, int max ){
 int size = width*height*sizeof(struct Pixel);

 if( size > max )
  return;
 else 
  memcpy( buffer, bmap, size );
}

/* Assuming the assumptions hold, we hold a 24 or 32 bit bmap */
void cBitmap::getGreyscaleBitmap( unsigned char * buffer, int max ){
 int size = width*height*sizeof(unsigned char);

 if( size > max )
  return;

 for( int i = 0; i < width*height; i++ ){
  buffer[i] = static_cast<unsigned char>( floor( (1.0f/3.0f)*(bmap[i].r+bmap[i].g+bmap[i].b) ) );
 }
}


void cBitmap::getRangeOfBitmap( unsigned char * buffer, int maxsize, int min_x, int max_x, int min_y, int max_y ){
 int poff = 0;

 for( int i = min_y; i <= max_y; i++ ){
  memcpy( buffer+poff, bmap+i*width+min_x, sizeof( struct Pixel )*(max_x-min_x+1) );	//Copy 4*(max_x - min_x) bytes from each row
  poff += sizeof( struct Pixel )*(max_x-min_x+1);
  if( poff > maxsize ) return;
 }
}

void cBitmap::setBitmap( unsigned char * bmp, int w, int h, int b ){
 width = w;
 height = h;
 bpp = b;

 bmap = (Pixel *) realloc( bmap, width*height*sizeof( struct Pixel ) );

 if( bmap == NULL ){ fprintf( stderr, "Could not allocate memory!\n" ); exit(1); }

 /* This is not really optimal, but we still keep data as RGBA internally. Time for Templates maybe? */
 if( b == 3 ){
  for( int j = 0; j < h; j++ ){
	for( int i = 0; i < w; i++ ){
		memcpy( &(bmap[j*width+i]), &(bmp[j*width*3+i*3]), 3*sizeof(unsigned char) );
		}
	}
 }
	
 if( b == 4 )
  memcpy( bmap, bmp, width*height*sizeof( struct Pixel ) );
}


int cBitmap::getWidth(){ return width; }
int cBitmap::getHeight(){ return height; }
int cBitmap::getBPP(){ return bpp; }


void cBitmap::setWidth( int w ){ width = w; }
void cBitmap::setHeight( int h ){ height = h; }
void cBitmap::setBPP( int d ){ bpp = d; }

void cBitmap::allocateMemory(){
 if( width > 0 && height > 0 && bpp > 0 ){
  bmap = (Pixel *) realloc( bmap, width*height*sizeof( struct Pixel ) );
  if( bmap == NULL ){ fprintf( stderr, "Could not allocate memory!\n" ); exit(1); }
 }
};
