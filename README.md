VASH
====

Perceptual Hash project for Videos (MMAI Term Project)


Tutorial: So I have a video, but how can I get all the (I-) Frames out of it?
Well, you could either write your own decoder. Or you could be smart and use the help of 'ffmpeg' :)
* A simple tutorial can be found at http://dranger.com/ffmpeg/tutorial01.html
* Unfortunately, the tutorial is somewhat outdated, many of the functions used are deprecated today.
* To save you a lot of Google'ing, here is an overview fitted to tutorial1.c:
  * Use 'avformat_open_input_file' instead of 'av_open_input_file'
  * Use 'avformat_find_stream_info' instead of 'av_find_stream_info'
  * Use 'av_dump_format' instead of 'dump_format'
  * 'AVMEDIA_TYPE_VIDEO' instead of 'CODEC_TYPE_VIDEO'
  * Use 'avcodec_open2' instead of 'avcodec_open'
  * Use 'avcodec_decode_video2' instead of 'avcodec_decode_video'
  * Use 'avformat_close_input' instead of 'av_close_input_file'
* The biggest change is necessary to replace 'img_convert'. Here we change to libswscale:
  * First, use 'sws_getContext' before the main loop to get a SwsContext.
  * Assuming we have the current context in pSWSContext, and the old frame in pFrame.
  * The new frame shall be pFrameRGB. Then the command inside the main loop is:
  * 'sws_scale(pSWSContext, (const uint8_t **)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);'
* You can get I-Frames by checking whether 'pFrame->pict_type == FF_I_TYPE' after decoding a packet.
* To see an example, check our repository in src/tools/decodeVideo.c


Week 2:
* Read I-frames directly into memory from video file in C and/or Java
  * Resize all frame to standard resolution? e.g. VGA 640x480, or go with a more modern 16:9 ratio?
  * What if Aspect Ratio doesn't match? Fit into 640x480 without cutting of the image and fill the remainder with black (single-color background won't have any SIFT features) 
* How to calculate distance between sets of SIFT features? 
  * How to cluster? k-means? Bag-of-Words?
  * How to deal with SIFT features with multiple orientations? What does it even mean?
* How to deal video?
  * How to deal with the temporal offset?
    * Scene detection? Then use one frame per scene? First frame or representative frame?
    * Calibrate by comparing the first sequence of frames and verify with another sequence later on?
* Evaluate Manhattan-Distance (D1) for frame similarity
* Noise function (blur, drop random images, logo overlay, crop, black bars, subtitles, etc) for testing (as set of images)


Week 1:
* Evaluate SIFT features for frame similarity detection using the VLFeat implementation (http://www.vlfeat.org/api/sift.html)
* Procure (a small set of) real-world sample data (different resolution, noise, and similar attack types)
* Investigate Bag-of-Words supported by local-sensitive-hashing

Testdata:
* 4-times the same 50min video (900-1100 i-frames per video) with different time offsets, encodings, video overlay (logo)
* random selection of videos files as negative samples (and training data for visual word vocabulary)

Papers:
* "Y. Cai, L. Yang: Large-Scale Near-Duplicate Web Video Retrieval: Challenges and Approaches, 2013"
* "Liu et al.: Near-Duplicate Video Retrieval: Current Research and Future Trends, 2013"
