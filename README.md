VASH
====

Perceptual Hash project for Videos (MMAI Term Project)

Students: 
* Sebastian Agethen (蔡格昇) (D01944015)
* Reinhard Pointner (瑞尼) (???)
* 蔡宗諭 (???)


Introduction
------------

With the advent of filesharing more and more people enjoy watching movies of tv shows on their computers, on top of that more and more people prefer buying digital copies over physical disks. The obvious advantage being that the media is always readily available on the harddrive for local consumption or streaming to other devices. Just as many people now store thousands of music files on their computer this may very well be the case for full-length movie files. And just as with music there will be a need organize and index this large number of videos in one way or another.

This trend is underlined by increasingly popular HTPC and Media Center software like XBMC, Plex, Servio, MythTV and myriad of similar software with the sole purpose of organizing, indexing and playing videos, music and images into beautiful easy-to-navigate libraries. For music files ID3 tags are generally available as part of the file itself so the HTPC software so music libraries can be organized automatically and easily navigated by artist, album, year, genere, or any other type of information. On top of that music identification based on Audio Fingerprint technology has become commonplace through services like Apples commercial iTunes MusicMatch or the open-source MusicBrainz AcoustID database. So even music that has not been tagged at all can easily be identified and associated with metadata online.

For video the situation is dire. Not only do the most popular video formats not even support any kind of tagging mechanism, even the newer formats like mp4 that would support tagging are generally never tagged. HTPC software can therefore only rely on the filepath to infere at least some pieces of information, and in many cases even that is impossible due to bad naming. The de-facto standard in HTPC software is that media already needs to be perfectly named and organized so that the HTPC software can look up additional metadata via webservices like TheMovieDB or TheTVDB. The process of manually organizing thousands of files is extremely time-consuming and even then there will be a risk of misidentified files if the names don't match the online database entries verbatim.

This project proposes a perceptual video hash algorithm to identify videos based by looking at the video content much like a human being would. Using this hash HTPC software would then be able to quickly and accurately identify video files and fetch additional metadata from online databases without any manual input greatly enhancing the HTPC experience.





Implementation Details
----------------------
_Dec. 22nd 2013_ 

**Tutorial: Video Decoding w/ FFMPEG**

When working with a collection of videos, it will be necessary to efficiently decode frames no matter what source format the data has. As a consequence, you can either write your own decoder for every possible format. Or you can be smart and use the help of a tool such as 'ffmpeg' :) The following describes briefly how to invoke ffmpeg decoding from within your C/C++ application:
* An introductory tutorial can be found at http://dranger.com/ffmpeg/tutorial01.html
* Unfortunately, the tutorial is somewhat outdated, many of the functions used are deprecated today.
* To save you a lot of Google'ing, we provide you with an overview of necessary changes to tutorial1.c:
  * Use 'avformat_open_input_file' instead of 'av_open_input_file'
  * Use 'avformat_find_stream_info' instead of 'av_find_stream_info'
  * Use 'av_dump_format' instead of 'dump_format'
  * 'AVMEDIA_TYPE_VIDEO' instead of 'CODEC_TYPE_VIDEO'
  * Use 'avcodec_open2' instead of 'avcodec_open'
  * Use 'avcodec_decode_video2' instead of 'avcodec_decode_video'
  * Use 'avformat_close_input' instead of 'av_close_input_file'
* The biggest change is necessary to replace 'img_convert'. Here we make use of libswscale:
  * First, use 'sws_getContext' before the main loop to get a SwsContext.
  * Assuming we have the current context in pSWSContext, and the old frame in pFrame.
  * The new frame shall be pFrameRGB. Then the command inside the main loop is:
  * 'sws_scale(pSWSContext, (const uint8_t **)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);'
  * This gives you a RGB bitmap (all channels are located in pFrameRGB->data[0]!)
* You can discard non-I-Frames by checking whether 'pFrame->pict_type == FF_I_TYPE' after decoding a packet.
* Required libraries are: libavformat, libavcodec, libswscale
* To see a full example, check our repository in src/tools/decodeVideo.c or src/tools/Movie.cc


Challenges
----------
_Dec. 18th 2013_

**Week 2**:
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


_Dec 9th 2013_ 

**Week 1**:
* Evaluate SIFT features for frame similarity detection using the VLFeat implementation (http://www.vlfeat.org/api/sift.html)
* Procure (a small set of) real-world sample data (different resolution, noise, and similar attack types)
* Investigate Bag-of-Words supported by local-sensitive-hashing

**Testdata:**
* 4-times the same 50min video (900-1100 i-frames per video) with different time offsets, encodings, video overlay (logo)
* random selection of videos files as negative samples (and training data for visual word vocabulary)

Related Work
------------

Video duplicate detection and, more general, Video Retrieval, have been hot research topics in recent years. The surveys in [1] and [2] summarize the state of the art in the field of video duplicate detection. In [1], Cai et al. describe components of a detection system in detail and perform an evaluation of different techniques. Their result shows that performance still suffers for large datasets.

There are two feature groups in video duplicate detection: Global features, such as color histograms, and local features. A state-of-the-art local feature is SIFT [3], which we are using in our project. In his work, Lowe finds keypoints as local extrema in the scale space of the DoG (Difference of Gaussian). Keypoints have scales and orientations, eventually leading to 128-bin histograms, a format often used with other detectors.
As matching (possibly bipartite matching) of these keypoints between frames is expensive, a quantization method is necessary. In [4], Shang et al. propose the so-called Bag-of-Words concept, simplifying feature vectors to Visual words. The intention is to reduce to problem to a text duplicate detection, for which well-established algorithms exist.
We implement this concept by clustering SIFT keypoints with k-means.

We also read a number of other papers, which we did not implement, partially due to time reasons. Park et al. propose in [5] a local feature and a framework to identify duplicate web videos. Shot detection is performed and a signature generated, which describes the occurences of a feature in the shot. In [6] a new strategy to select frames of a video is introduced and performance is increased by elimination of rare features in such frames. Local features and BoW are used in [7] together with a new algorithm that detects temporal patterns in those features. Sarkar et al. propose a new distance measure in [8]. Furthermore, search algorithms are developed supported by a pruning technique for fast runtime. Liu et al. propose Video Histograms in [9], where a bin of the histogram represents the number of frames that are similar to a predefined set of seed vectors. [10] uses chromacity histograms as features and uses Dynamic Programming to match video shots based on the features.

In our implementation, we are using the popular VLFeat Library [11] to implement a SIFT Keypoint detector. BoW quantizer and matching are implemented by ourselves however.

_Surveys:_
* [1] "Y. Cai, L. Yang: Large-Scale Near-Duplicate Web Video Retrieval: Challenges and Approaches, 2013"
* [2] "Liu et al.: Near-Duplicate Video Retrieval: Current Research and Future Trends, 2013"

_Main papers:_
* [3] "David G. Lowe. 2004. Distinctive Image Features from Scale-Invariant Keypoints. Int. J. Comput. Vision 60, 2 (November 2004)"
* [4] "Lifeng Shang, Linjun Yang, Fei Wang, Kwok-Ping Chan, and Xian-Sheng Hua. 2010. Real-time large scale near-duplicate web video retrieval. In Proceedings of the international conference on Multimedia (MM '10)"
 
_Others:_
* [5] "Kyung-Wook Park, Jee-Uk Heu, Bo-kyeong Kim, and Dong-Ho Lee. 2013. Real-time near-duplicate web video identification by tracking and matching of spatial features. In Proceedings of the 7th International Conference on Ubiquitous Information Management and Communication (ICUIMC '13)"
* [6] "Xiangmin Zhou, Xiaofang Zhou, Lei Chen, Athman Bouguettaya, Nong Xiao, and John A. Taylor. 2009. An efficient near-duplicate video shot detection method using shot-based interest points. Trans. Multi. 11, 5 (August 2009), 879-891."
* [7] "Wan-Lei Zhao and Chong-Wah Ngo. 2009. Scale-rotation invariant pattern entropy for keypoint-based near-duplicate detection. Trans. Img. Proc. 18, 2 (February 2009), 412-423"
* [8] "Efficient and Robust Detection of Duplicate Videos in a Large Database. Anindya Sarkar, Vishwakarma Singh, Pratim Ghosh, Bangalore S. Manjunath, and Ambuj K. Singh. IEEE Trans. Circuits Syst. Video Techn. 20(6):870-885 (2010)"
* [9] "Lu Liu, Wei Lai, Xian-Sheng Hua, and Shi-Qiang Yang. 2007. Video histogram: a novel video signature for efficient web video duplicate detection. In Proceedings of the 13th International conference on Multimedia Modeling - Volume Part II (MMM'07)"
* [10] "Jian Zhou and Xiao-Ping Zhang. 2005. Automatic identification of digital video based on shot-level sequence matching. In Proceedings of the 13th annual ACM international conference on Multimedia (MULTIMEDIA '05)"

_Resources_:
* [11] VLFeat Library (http://www.vlfeat.org/index.html)
