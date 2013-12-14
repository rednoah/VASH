/* Author: Sebastian Agethen */
/* Email: s.agethen@gmail.com */
/* Simple Bitmap Loader for 24 bit and 32 bit Windows Bitmaps */


#ifndef __BITMAP_HH__
#define __BITMAP_HH__

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cassert>
#include <cmath>

using namespace std;

/* Pixel format: RGBA in that order*/
struct Pixel{
 unsigned char r;
 unsigned char g;
 unsigned char b;
 unsigned char a;
};



class cBitmap{
 private:
  struct Pixel * bmap;
  int width;
  int height;
  int bpp;

 public:
  /* Construct an empty bitmap */
  cBitmap();

  /* Construct & Load a bitmap from <filename> */
  cBitmap( char * filename );

  /* Destructor */
  ~cBitmap();

  /* load a bitmap from <filename>. Supported formats are 24 and 32bpp. */
  /* Return 0 on success, -1 otherwise */
  int loadBitmap( char * filename );

  /* Save bitmap to <filename>. Output format is currently fixed to 24bpp */
  /* Return 0 on success, -1 otherwise */
  int saveBitmap( char * filename );
  
  /* Get pixel at (x,y) and store it in provided Pixel p */
  /* Return 0 on success, -1 otherwise (e.g., x,y out-of-bounds) */
  int getPixel( int x, int y, Pixel & p );

  /* Set Pixel at (x,y) */
  /* Return 0 on success, -1 otherwise (e.g., x,y out-of-bounds) */
  int setPixel( int x, int y, Pixel p );

  /* Copies bitmap into the provided parameter buffer. maxsize is the maximum space in buffer in sizeof(char) */
  /* Output format is RGBA in that order */
  void getBitmap( unsigned char * buffer, int maxsize );

  /* Same as above, but output format is greyscale, 1 Byte per pixel */
  void getGreyscaleBitmap( unsigned char * buffer, int maxsize );

  /* Loads bitmap from bmp */
  /* bpp are bytes per pixel. Possible values are bpp=3 or bpp=4 */
  /* Assumed input format is RGBA in that order */
  void setBitmap( unsigned char * bmp, int w, int h, int b );

  /* Like getBitmap(2), but only copy pixels from given range */
  void getRangeOfBitmap( unsigned char * buffer, int maxsize, int min_x, int max_x, int min_y, int max_y );

  /* Get parameters of bitmap */
  int getWidth();
  int getHeight();
  int getBPP(); //Size in Bytes, not Bits!

  void setWidth( int w );
  void setHeight( int h );
  void setBPP( int d );
  void allocateMemory();
};
#endif
