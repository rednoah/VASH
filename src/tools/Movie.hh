#ifndef MOVIE_HH
#define MOVIE_HH

extern "C" {
#define __STDC_CONSTANT_MACROS	//UINT64_C is otherwise unknown
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "Bitmap.hh"

#include "../vash.hh"



using namespace std;

class Movie{
	public:
		Movie( char * f );
		~Movie();

		void seekNextSection( double percentage );
		bool loadNextFrame( cBitmap & b );
		bool isHealthy();
	private:
		void initialize();

		char filename[128];
		bool healthy;

		int width;
		int height;

		long lengthInFrames;
		long timestamp;

		AVFormatContext *pFormatCtx;
  		int             videoStream;
		AVStream * 		pVideoStream;
  		AVCodecContext  *pCodecCtx;
  		AVCodec         *pCodec;
  		AVFrame         *pFrame; 
  		AVFrame         *pFrameRGB;
  		AVPacket        packet;
  		int             frameFinished;
  		int             numBytes;
  		uint8_t         *buffer;
		
		SwsContext * pSWSContext;
		int nVidWidth;
		int nVidHeight;
};


#endif
