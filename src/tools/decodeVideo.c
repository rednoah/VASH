/* Dec 22nd: Modified by Sebastian Agethen from Bohmes original (tutorial 1).
   ********  -replaced deprecated features,
   ********  -removed some of the comments
*/
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  fclose(pFile);
}

int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx;
  AVCodec         *pCodec;
  AVFrame         *pFrame; 
  AVFrame         *pFrameRGB;
  AVPacket        packet;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer;
  
  if(argc < 2) {
    printf("Please provide a movie file\n");
    return -1;
  }

  av_register_all();			  					//Register all formats and codecs
  
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file
  
  
  if(avformat_find_stream_info(pFormatCtx,NULL)<0)	//Retrieve stream information
    return -1;
  
  av_dump_format(pFormatCtx, 0, argv[1], 0);		//Dump information about file onto standard error

  
  //Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }

  if(videoStream==-1)
    return -1;
  
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;	  // Get a pointer to the codec context for the video stream
  
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);		  // Find the decoder for the video stream
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; 
  }
  
  if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)			  // Open codec
    return -1;
  
  
  pFrame=avcodec_alloc_frame();							  // Allocate video frame
  
  pFrameRGB=avcodec_alloc_frame();						  // Allocate an AVFrame structure

  if(pFrameRGB==NULL)
    return -1;
  
  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
  
  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);


  //Dec 22nd: Added this to replace img_convert(..) with sws_scale(...)
  int nVidWidth = pCodecCtx->width;
  int nVidHeight = pCodecCtx->height;
  struct SwsContext * pSWSContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, nVidWidth, nVidHeight, PIX_FMT_RGB24, SWS_BILINEAR, 0, 0, 0); 


  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    if(packet.stream_index==videoStream) {													//Is this a packet from the video stream?
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet );					//Decode video frame

	  if(pFrame->pict_type != FF_I_TYPE )													//Added: If this is not an I-Frame, skip it
         continue;
      
      //If we got a frame, convert it to RGB and save it.
      if(frameFinished) {
    	sws_scale(pSWSContext, (const uint8_t **)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
	
		if(++i > 5)
			break;																			//TODO: This oughta be changed obviously

	    SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);						//TODO: And here we dont need to write to disk. Copy to memory instead
      }
    }
    
    av_free_packet(&packet);
  }
  
  // Free all the stuff
  av_free(buffer);
  av_free(pFrameRGB);
  av_free( pSWSContext );  
  av_free(pFrame);
  
  avcodec_close(pCodecCtx);
  
  avformat_close_input(&pFormatCtx);
  return 0;
}



/* Some Changes I had to make on the original tutorial:
- img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, (AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
became sws_scale,
- av_close_input_file(pFormatCtx);
became avformat_close_input,
- avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
became avcodec_decode_video2,
- avcodec_open(pCodecCtx, pCodec)
became avcodec_open2
- CODEC_TYPE_VIDEO is now AVMEDIA_TYPE_VIDEO
- dump_format(pFormatCtx, 0, argv[1], 0);
became av_dump_format,
- av_find_stream_info(pFormatCtx)
became avformat_find_stream_info,
- av_open_input_file(&pFormatCtx, argv[1], NULL, 0, NULL)
became avformat_open_input_file.
*/

