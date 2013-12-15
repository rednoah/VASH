#include "viewer.hh"
#include <cstring>

unsigned char * buffer;
int width;
int height;

void glutViewer( unsigned char * b, int w, int h, int depth, int argc, char ** argv, int windowsize_x, int windowsize_y ){
// buffer = b;
 width = w;
 height = h;
 buffer = new unsigned char[w*h*4];

 if( depth == 1 ){	//Greyscale. Convert to RGBA
	 for( int i = 0; i < w*h; i++ ){
		buffer[i*4] = b[i];
		buffer[i*4+1] = b[i];
		buffer[i*4+2] = b[i];
		buffer[i*4+3] = 0;
 	}
 }
 
 if( depth == 3 ){	//Color w/o Alpha component -> Add Alpha Component
	 for( int i = 0; i < w*h; i++ ){		
		buffer[i*4] = b[i*3];
		buffer[i*4+1] = b[i*3+1];
		buffer[i*4+2] = b[i*3+2];
		buffer[i*4+3] = 0;
 	}
 }

 if( depth == 4 )	//Otherwise just copy
 	memcpy( buffer, b, w*h*4 );

 /* Viewer */
 glutInit( &argc, argv );
 glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
 glutInitWindowSize( windowsize_x,windowsize_y );
 glutInitWindowPosition( 400, 400 );
 glutCreateWindow("Raw Image Display");
 glutDisplayFunc(draw);
 glutKeyboardFunc(myKeyboard);
 glutMainLoop();

 
}



/* Viewer Related Stuff */
void myKeyboard(unsigned char key, int x, int y){
 switch(key){
   case 27:
    exit(0);
 };
}

void draw( ){

 
  glClear(GL_COLOR_BUFFER_BIT);
 
  glEnable( GL_TEXTURE_RECTANGLE_ARB );

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);

  glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height,
               0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
 
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER,	//Use GL_NEAREST here to avoid interpolation (instead of GL_LINEAR)
                  GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST);
 
  glBegin(GL_QUADS);
  glTexCoord2i(0, height);
  glVertex2f(-1, -1);
  glTexCoord2i(width, height);
  glVertex2f(1, -1);
  glTexCoord2i(width, 0);
  glVertex2f(1, 1);
  glTexCoord2i(0, 0);
  glVertex2f(-1, 1);
  glEnd();
 
  glDeleteTextures(1, &texture);
 
  glFlush();
  glutSwapBuffers();
}

