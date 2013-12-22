/* Dec 22nd: Modified by Sebastian Agethen from Bohmes original (tutorial 1).
   ********  -replaced deprecated features,
   ********  -removed some of the comments
   ********  -created Class
*/
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

#include "Movie.hh"

using namespace std;

Movie::Movie( char * f ){
 memset( filename, 0, 128*sizeof(char) );
 strcpy( filename, f );
 healthy = true;

 pFormatCtx = NULL;
 pCodecCtx = NULL;
 pCodec = NULL;
 pFrame = NULL; 
 pFrameRGB = NULL;

 initialize();
}

Movie::~Movie(){
	// Free all the stuff
	av_free(buffer);
	av_free(pFrameRGB);
	av_free( pSWSContext );  
	av_free(pFrame);
  
	avcodec_close(pCodecCtx);
  
	avformat_close_input(&pFormatCtx);
}

bool Movie::isHealthy( ){
 return healthy;
}

void Movie::initialize(){
	av_register_all();			  					//Register all formats and codecs

	if( avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0 ){
		healthy = false;
		return; // Couldn't open file
	}
  
  	if( avformat_find_stream_info(pFormatCtx,NULL) < 0 ){	//Retrieve stream information
		healthy = false;
		return;
	}

	#ifdef DEBUG
	av_dump_format(pFormatCtx, 0, filename, 0);				//Dump information about file onto standard error
	#endif

	videoStream=-1;
	for( unsigned int i = 0; i < pFormatCtx->nb_streams; i++ )
		if( pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO ){
			videoStream=i;
			break;
		}

	if(videoStream==-1){
		healthy = false;
		return;
	}
  
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;	// Get a pointer to the codec context for the video stream
  
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);		// Find the decoder for the video stream

	if(pCodec==NULL) {
		fprintf(stderr, "Movie: Unsupported codec!\n");
		healthy = false;
		return;
	}
  
	if( avcodec_open2(pCodecCtx, pCodec, NULL) < 0 ){		// Open codec
		healthy = false;
		return;
	}
  
	pFrame = avcodec_alloc_frame();							// Allocate video frame
  
	pFrameRGB = avcodec_alloc_frame();						// Allocate an AVFrame structure

	if( pFrameRGB == NULL ){
		healthy = false;
		return;
	}
  
	// Determine required buffer size and allocate buffer
	numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
 	buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	//Dec 22nd: Added this to replace img_convert(..) with sws_scale(...)
	nVidWidth = pCodecCtx->width;		//We can also resize the image here if necessary
	nVidHeight = pCodecCtx->height;
	width = pCodecCtx->width;
	height = pCodecCtx->height;
	pSWSContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, nVidWidth, nVidHeight, PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0);
}

bool Movie::loadNextFrame( cBitmap & b ){
	while(av_read_frame(pFormatCtx, &packet)>=0) {
		if(packet.stream_index==videoStream) {													//Is this a packet from the video stream?
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet );					//Decode video frame

			if(pFrame->pict_type != FF_I_TYPE )													//Added: If this is not an I-Frame, skip it
				continue;
      
			//If we got a frame, convert it to RGB and save it.
			if(frameFinished) {
				sws_scale(pSWSContext, (const uint8_t **)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

				//Create Bitmap from frame
				char * tmp = new char[width*height*3];

				//Image is stored linewise. Note that pFrameRGB->linesize[0] not necessarily == width*3 (4 byte alignment)
				for( int i = 0; i < height; i++ )
					memcpy( tmp+3*i*width, pFrameRGB->data[0]+i*pFrameRGB->linesize[0], width*3 );

				b.setBitmap( reinterpret_cast<unsigned char*>(tmp), width, height, 3 ); 
				delete[] tmp;
				break;
			}
		}    
		av_free_packet(&packet);
	}
	return true;
}
