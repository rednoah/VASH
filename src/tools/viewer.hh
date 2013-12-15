#ifndef __VIEWER_HH__
#define __VIEWER_HH__
#include <GL/glut.h>

/* Call this function to display an image */
/* @b: image buffer. Expected size is w*h*4 */
/* @w: Width of image */
/* @h: Height of image */
/* @depth: Depth of image in Bytes (accepted: 1,3 or 4) */
/* @argc: Pass argc of main( .. ) here. Needed for glut stuff */
/* @argv: Pass argv of main( .. ) here. Needed for glut stuff */
/* @windowsize_x: Viewer Window size in x dimension (width) */
/* @windowsize_y: Viewer Window size in y dimension (height) */
/* Notes: Expected format of buffer is RGBA. */
void glutViewer( unsigned char * b, int w, int h, int depth, int argc, char ** argv, int windowsize_x, int windowsize_y );

/* Keyboard function. Used by glutViewer(..) */
void myKeyboard(unsigned char key, int x, int y);

/* Draw function. Used by glutViewer(..) */
void draw( );
#endif
