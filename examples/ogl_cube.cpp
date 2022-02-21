//
// This code was created by Jeff Molofee '99 (ported to Solaris/GLUT by Lakmal Gunasekara '99)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe 
// (email Richard Campbell at ulmont@bellsouth.net)
// (email Lakmal Gunasekara at lakmal@gunasekara.de)
//
// '15 edited in order to get dynamic texture update coming from a video stream.
//
#include <iostream>
#include <string.h>

#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <unistd.h>     // Header file for sleeping.
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.

#include "./include/avapplication.h"
#include "./include/avdecoder.h"
#include "./include/avimage.h"
#include "./include/videofiltergraph.h"

/* ascii codes for various special keys */
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

using namespace libavcpp;
CAVDecoder _avDecoder;


class AVDecoderEventsImp : public IAVDecoderEvents
{
public:
  const CAVImage*      getImage() const
  { 
    return &m_avImage;
  } 

  virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
   
    
    double d = 1/av_q2d(pAVCodecCtx->time_base);
     AVRational _avValue = av_d2q( 1/23.5, 30 );
    m_dFPSs = 1/d;
    m_dFPSw = 20;
    m_dFPSd = m_dFPSs -m_dFPSw;
    
    m_dDropTime =  1 / m_dFPSd;
  }
  
  virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
     //m_avImage.init( pAVFrame, pAVCodecCtx, 300, 300, PIX_FMT_BGR24 );
     //m_avImage.init( pAVFrame, pAVCodecCtx, -1, -1, PIX_FMT_RGBA );
     
     return true;
  }
  
  virtual bool   OnFilteredVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double pst )
  {
    /* Nothing to do */
    m_avImage.init( pAVFrame, pAVCodecCtx, -1, -1, AV_PIX_FMT_RGBA );
    
    return true;
  }
  
  virtual bool   OnAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
     return true;
  }
  
  virtual bool   OnFilteredAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pst )
  {
    /* Nothing to do */  
    return true;
  }

  
private:
  CAVImage   m_avImage;
  double     m_dFPSs;
  double     m_dFPSw;
  double     m_dFPSd;
  double     m_dDropTime;
};


const char* pFileName = NULL;

/* The number of our GLUT window */
int window; 

/* lighting on/off (1 = on, 0 = off) */
int light;

/* L pressed (1 = yes, 0 = no) */
int lp;

/* F pressed (1 = yes, 0 = no) */
int fp;


GLfloat xrot;   // x rotation 
GLfloat yrot;   // y rotation 
GLfloat xspeed; // x rotation speed
GLfloat yspeed; // y rotation speed

GLfloat z=-5.0f; // depth into the screen.

/* white ambient light at half intensity (rgba) */
GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };

/* super bright, full intensity diffuse light. */
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

/* position of light (x, y, z, (position of light)) */
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

GLuint	filter;			/* Which Filter To Use (nearest/linear/mipmapped) */
GLuint	texture[3];		/* Storage for 3 textures. */
GLuint  blend;                  /* Turn blending on/off */

/* Image type - contains height, width, and data */
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;


// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.
int ImageLoad(Image *image) 
{
    /////////////
    _avDecoder.read( AVD_EXIT_ON_NEXT_VIDEO_FRAME );
    ///////////////////////////
 
    IAVDecoderEvents* pEvents = NULL;


    _avDecoder.getDecoderEvents( pEvents );
    
    if ( pEvents == NULL )
      return 0;
    
    const CAVImage* pAvImage = ((AVDecoderEventsImp*)pEvents)->getImage();
    
    int iSize = 0;
    const unsigned char* pData = pAvImage->getData( 0, &iSize );

    
    if ( image->data == NULL )
    {
      image->data = (char*)malloc( iSize ); 
      image->sizeX = pAvImage->getWidth();
      image->sizeY = pAvImage->getHeight();
    }
    
    memcpy( image->data, pData, iSize );
  
    // we're done.
    return 1;
}

// Load Texture
Image *image1 = NULL;

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures() 
{
    
    // allocate space for texture
    if (image1 == NULL)
    {
        image1 = (Image *) malloc(sizeof(Image));
	image1->data = NULL;
	image1->sizeX = -1;
	image1->sizeY = -1;
    }
    
    if (image1 == NULL) 
    {
	printf("Error allocating space for image");
	exit(0);
    }
    
    if (!ImageLoad( image1)) 
    {
	exit(1);
    }        
    //_decoder.read();
    glDeleteTextures(3, &texture[0]);
    
    // Create Textures	
    glGenTextures(3, &texture[0]);

    // texture 1 (poor quality scaling)
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST); // cheap scaling when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST); // cheap scaling when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1->data);
    

    // texture 2 (linear scaling)
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1->data);

    // texture 3 (mipmapped scaling)
    glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale linearly + mipmap when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1->data);

    // 2d texture, 3 colors, width, height, RGB in that order, byte data, and the data.
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image1->sizeX, image1->sizeY, GL_RGBA, GL_UNSIGNED_BYTE, image1->data); 
    
};

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
GLvoid InitGL(GLsizei Width, GLsizei Height)	// We call this right after our OpenGL window is created.
{
     //////////////////////////////////
    //Initialize decoder
    _avDecoder.open( pFileName, 0.0 );
    _avDecoder.setDecoderEvents( new AVDecoderEventsImp, true );

    IAVFilterGraph* pAVFilterGraph = new CVideoFilterGraph( "frei0r=lenscorrection:0.5|0.5|0.38|0.5" );

    //_avDecoder.setFilterGraph( new CAVFilterGraph( "frei0r=lenscorrection:0.5:0.5:0.37:0.5,format=yuv420p" ) );
    _avDecoder.setFilterGraph( pAVFilterGraph );

    
    _avDecoder.read( AVD_EXIT_ON_NEXT_VIDEO_FRAME );
    ///////////////////////////
    
    LoadGLTextures();                           // load the textures.
    
    glEnable(GL_TEXTURE_2D);                    // Enable texture mapping.

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix
    
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);

    // set up light number 1.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
    glEnable(GL_LIGHT1);                             // turn light 1 on.
    
    /* setup blending */
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);			// Set The Blending Function For Translucency
    glColor4f(1.0f, 1.0f, 1.0f, 0.5);
   
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
    if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
	Height=1;

    glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
GLvoid DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    
    LoadGLTextures();

    glLoadIdentity();				// Reset The View
  
    glTranslatef(0.0f,0.0f,z);                  // move z units out from the screen.
    
    glRotatef(xrot,1.0f,0.0f,0.0f);		// Rotate On The X Axis
    glRotatef(yrot,0.0f,1.0f,0.0f);		// Rotate On The Y Axis

    glBindTexture(GL_TEXTURE_2D, texture[filter]);   // choose the texture to use.
    
    glBegin(GL_QUADS);		                // begin drawing a cube
    
    // Front Face (note that the texture's corners have to match the quad's corners)
    glNormal3f( 0.0f, 0.0f, 1.0f);                              // front face points out of the screen on z.
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    
    // Back Face
    glNormal3f( 0.0f, 0.0f,-1.0f);                              // back face points into the screen on z.
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	
    // Top Face
    glNormal3f( 0.0f, 1.0f, 0.0f);                              // top face points up on y.
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    
    // Bottom Face       
    glNormal3f( 0.0f, -1.0f, 0.0f);                             // bottom face points down on y. 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    
    // Right face
    glNormal3f( 1.0f, 0.0f, 0.0f);                              // right face points right on x.
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    
    // Left Face
    glNormal3f(-1.0f, 0.0f, 0.0f);                              // left face points left on x.
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    
    glEnd();                                    // done with the polygon.

    xrot+=xspeed;		                // X Axis Rotation	
    yrot+=yspeed;		                // Y Axis Rotation

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}


/* The function called whenever a normal key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case ESCAPE: // kill everything.
	/* shut down our window */
	glutDestroyWindow(window); 
	
	/* exit the program...normal termination. */
	exit(1);                   	
	break; // redundant.

    case 76: 
    case 108: // switch the lighting.
	printf("L/l pressed; light is: %d\n", light);
	light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
	printf("Light is now: %d\n", light);
	if (!light) {
	    glDisable(GL_LIGHTING);
	} else {
	    glEnable(GL_LIGHTING);
	}
	break;

    case 70:
    case 102: // switch the filter.
	printf("F/f pressed; filter is: %d\n", filter);
	filter+=1;
	if (filter>2) {
	    filter=0;	
	}	
	printf("Filter is now: %d\n", filter);
	break;

    case 98: 
    case 130: // switch the blending.
	printf("B/b pressed; blending is: %d\n", blend);
	blend = blend ? 0 : 1;              // switch the current value of blend, between 0 and 1.
	printf("Blend is now: %d\n", blend);
	if (!blend) {
	  glDisable(GL_BLEND);              // Turn Blending Off
	  glEnable(GL_DEPTH_TEST);          // Turn Depth Testing On
	} else {
	  glEnable(GL_BLEND);		    // Turn Blending On
	  glDisable(GL_DEPTH_TEST);         // Turn Depth Testing Off
	}
	break;
	
    case 'q':
    break;
    case 'w':
    break;
    case 'e':
    break;

    case 'a':
    break;
    case 's':
    break;
    case 'd':
    break;
    
    default:
      printf ("Key %d pressed. No action there yet.\n", key);
      break;
    }	
}

/* The function called whenever a normal key is pressed. */
void specialKeyPressed(int key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case GLUT_KEY_PAGE_UP: // move the cube into the distance.
	z-=0.02f;
	break;
    
    case GLUT_KEY_PAGE_DOWN: // move the cube closer.
	z+=0.02f;
	break;

    case GLUT_KEY_UP: // decrease x rotation speed;
	xspeed-=0.01f;
	break;

    case GLUT_KEY_DOWN: // increase x rotation speed;
	xspeed+=0.01f;
	break;

    case GLUT_KEY_LEFT: // decrease y rotation speed;
	yspeed-=0.01f;
	break;
    
    case GLUT_KEY_RIGHT: // increase y rotation speed;
	yspeed+=0.01f;
	break;

    default:
	break;
    }	
}

int main(int argc, char **argv) 
{  
    CAVApplication::initLibAVCPP();
    
    /* Initialize GLUT state - glut will take any command line arguments that pertain to it or 
       X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */  
    glutInit(&argc, argv);  

    if ( argc < 2 )
    {
      printf( "Set default video input to ./video.avi\n" );
      pFileName = "./video.avi";
    }
    else
      pFileName = argv[1];
    
    /* Select type of Display mode:   
     Double buffer 
     RGBA color
     Depth buffer 
     Alpha blending */  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);  

    /* get a 800 x 600 window */
    glutInitWindowSize(800, 600);  

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);  

    /* Open a window */  
    window = glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99");  

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);  

    /* Go fullscreen.  This is as soon as possible. */
    //glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene); 

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    /* Register the function called when special keys (arrows, page down, etc) are pressed. */
    glutSpecialFunc(&specialKeyPressed);

    /* Initialize our window. */
    InitGL(800, 600);
  
    /* Start Event Processing Engine */  
    glutMainLoop();  

    CAVApplication::deinitLibAVCPP();

    return 1;
}

