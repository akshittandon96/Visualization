#include <stdio.h>
	// yes, I know stdio.h is not good C++, but I like the *printf()
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif


// You need to adjust the location of these header files according to your configuration

#include <windows.h>//windows.h is a Windows-specific header file for the C/C++ 
//programming language which contains declarations for all of the functions in the Windows API, 
//all the common macros used by Windows programmers, 
//and all the data types used by the various functions and subsystems.
//#include <freeglut.h>The OpenGL Utility Toolkit (GLUT) is a library of utilities for OpenGL programs, which primarily
//perform system-level I/O with the host operating system.
#include "GL\include\GL.H"
#include "GL\include\glu.h"
#include "GL\include\glut.h"
#include "GL\include\glui.h"

#include "Skeleton.h"//header file
#include <time.h>
//#include "glui.h"

//
//
//	This is a sample OpenGL / GLUT / GLUI program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with radio buttons
//
//	The left mouse button allows rotation
//	The middle mouse button allows scaling
//	The glui window allows:
//		1. The 3d object to be transformed
//		2. The projection to be changed
//		3. The color of the axes to be changed
//		4. The axes to be turned on and off
//		5. The transformations to be reset
//		6. The program to quit
//
//	Author: Joe Graphics
//


//
// constants:
//
// NOTE: There are a bunch of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch() statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch() statements.  Those are #defines.
//
//This is because #define is mostly used to define a name <string> in c++.


// This source code has been modified by Guoning Chen since its release


// title of these windows:

const char *WINDOWTITLE = { "Assignment 2 - Akshit Tandon" };//title that comes on the top of windows.
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };//size of the windos that opens on compilation


// size of the box:

const float BOXSIZE = { 2.f };//interface box size



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// able to use the left mouse for either rotation or scaling,
// in case have only a 2-button mouse:

enum LeftButton
{
	ROTATE,
	SCALE
};


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals//buttons on the user interface
{
	RESET,
	QUIT
};


// window background color (rgba):

const float BACKCOLOR[] = { 0., 0., 0., 0. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors//these colors are in the radio button on the user interface window
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};


// the object numbers:
// 
enum MODELS//these models are in the radio button on the user interface window
{
	//BUNNY,
	//FELINE,
	//DRAGON,
	//HAPPY,
	//SPHERE,
	//TORUS,
	//DIESEL_FIELD1,
	//DISTANCE_FIELD1,
	//DISTANCE_FIELD2,
	//ICELAND_CURRENT_FIELD,
	//TORUS_FIELD,
	TEMPERATURE1,
	TEMPERATURE2,
	

};
enum COLOR_MODELS//these models are in the radio button on the user interface window
{
	RAINBOW,
    BWR,
    HEATMAP,
    DISCRETE,
	NONLINEAR_RAINBOW,

};

// the color definitions:
// this order must match the radio button order

const GLfloat Colors[8][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };



//
// non-constant global variables:
//
int isWireframeOn = 0;
int displayXY = 1;
int displayYZ = 1;
int displayZX = 1;
int Xval = 32;
int Yval = 32;
int Zval = 32;
int TMIN = 0;
int TMAX = 100;
int GMIN = 0;
int GMAX = 0;
int set_opacity = 50;
int	ActiveButton;		// current button that is down
GLuint	AxesList;		// list to hold the axes
int	AxesOn;			// != 0 means to draw the axes
int	DebugOn;
int bilinear=0;// != 0 means to print debugging info
int vrendering = 0;
int	DepthCueOn;		// != 0 means to use intensity depth cueing
GLUI *	Glui;			// instance of glui window
int	GluiWindow;		// the glut id for the glui window
int	LeftButton;		// either ROTATE or SCALE
GLuint	BoxList;		// object display list
int	MainWindow;		// window id for main graphics window
GLfloat	RotMatrix[4][4];	// set by glui rotation widget
float	Scale, Scale2;		// scaling factors
int	WhichColor;		// index into Colors[]
int	WhichProjection;	// ORTHO or PERSP
int	Xmouse, Ymouse;		// mouse values
float	Xrot, Yrot;		// rotation angles in degrees
float	TransXYZ[3];		// set by glui translation widgets
float current_rgb[3];
int set_value;
int set_contours;
int set_s = 40;
float max = 0.;
float min = 0.;

//
// function prototypes:
//

void	Animate( void );
void	Buttons( int );
void	Display( void );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( void );
void	InitGlui( void );
void	InitGraphics( void );
void	InitLists( void );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( void );
void	Resize( int, int );
void	Visibility( int );

void	Arrow( float [3], float [3] );
void	Cross( float [3], float [3], float [3] );
float	Dot( float [3], float [3] );
float	Unit( float [3], float [3] );
void	Axes( float );
void	HsvRgb( float[3], float [3] );

void    Display_Model(void);
void    set_view(GLenum mode, Polyhedron *poly);
void    set_scene(GLenum mode, Polyhedron *poly);
void    display_shape(GLenum mode, Polyhedron *this_poly);
void    Choose_Object();
void rainbow_color(float s,
	float s_max,
	float s_min,
	float rgb[3]);
void calculate_max_min(Polyhedron *this_poly);
void bwr_color(float s,
	float s_max,
	float s_min);
void heat_color(float s,
	float s_max,
	float s_min);
void heat1_color(float s,
	float s_max,
	float s_min);
void discrete_color(float s,
	float s_max,
	float s_min);
void nonlrainbow_color(float s,
	float s_max,
	float s_min);

void Load_data_on_uniformGrids(const char *name);
void calculate_max_min_1();

void contour_calculationXY(int x, int y,int z);
void contour_calculationYZ(int x, int y, int z);
void contour_calculationZX(int x, int y, int z);
void set_s_value1(int x);
void coordinates_calculation();
void CompositeXY(void);
void DetermineVisibility();
void CompositeYZ(void);
void CompositeXZ(void);
void mapping();


double radius_factor = 0.9;
int display_mode = 0; 

int ObjectId = 0;
char object_name[128]= "temperature1";

int colorId = 0;
char color_name[128] = "Rainbow";

Polyhedron *poly = NULL;
//extern PlyFile *in_ply;
//extern FILE *this_file;

typedef struct node1
{
	float x, y, z, s;
};
typedef struct lineseg
{
	int n1, n2;
};
typedef struct quad
{
	int verts[4];
	int edges[4];
};
const int NX = 64, NY = 64, NZ = 64;
std::vector<node1> grid_pts;
std::vector<lineseg> edgeList;
std::vector<quad> quadList;

struct node
{
	float x, y, z; // location
	float T; // temperature
	float r, g, b; // the assigned color
	float rad; // radius
	float dTdx, dTdy, dTdz; // can store these if you want, or not
	float grad; // total gradient
};

node grid_3d[NX][NY][NZ];


struct sources
{
	double xc, yc, zc; //
	double a; // temperature value of the source
} Sources[] =
{
{ 1.00f, 0.00f, 0.00f, 90.00f },
{ -1.00f, -0.30f, 0.00f, 120.00f },
{ 0.10f, 1.00f, 0.00f, 120.00f },
{ 0.00f, 0.40f, 1.00f, 170.00f },
};

unsigned char TextureXY[NZ][NY][NX][4];
unsigned char TextureXZ[NY][NZ][NX][4];
unsigned char TextureYZ[NX][NZ][NY][4];
float TEMPMAX=100, TEMPMIN=0;
const int MINUS = { 0 };
const int PLUS = { 1 };

#define X 0
#define Y 1
#define Z 2

int Major;
int Xside, Yside, Zside; 

//
// main program:
//

int
main( int argc, char *argv[] )
{

	
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)
	//Load_data_on_uniformGrids("../models/temperature1.dat");
	glutInit( &argc, argv );

	 
	// Load the model and data here
	FILE *this_file = fopen("../models/bunny.ply", "r");
	poly = new Polyhedron (this_file);
	fclose(this_file);
	//mat_ident( rotmat );	

	poly->initialize(); // initialize everything

	poly->calc_bounding_sphere();
	poly->calc_face_normals_and_area();
	poly->average_normals();

	//calculate_max_min_1();
	coordinates_calculation();
	
	// setup all the graphics stuff:

	InitGraphics();
	


	// create the display structures that will not change:

	InitLists();


	// init all the global variables used by Display():
	// this will also post a redisplay
	// it is important to call this before InitGlui()
	// so that the variables that glui will control are correct
	// when each glui widget is created

	Reset();

	// setup all the user interface stuff:

	InitGlui();


	// draw the scene once and wait for some interaction:
	// (will never return)
	
	glutMainLoop();

	// finalize the object if loaded

	if (poly != NULL)
		poly->finalize();

	// this is here to make the compiler happy:

	return 0;
}



//
// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display() from here -- let glutMainLoop() do it
//

void
Animate( void )
{
	// put animation stuff in here -- change some global variables
	// for Display() to find:



	// force a call to Display() next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay();
}




//
// glui buttons callback:
//

void
Buttons( int id )
{
	switch( id )
	{
		case RESET:
			Reset();
			Glui->sync_live();
			glutSetWindow( MainWindow );
			glutPostRedisplay();
			break;

		case QUIT:
			// gracefully close the glui window:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:

			Glui->close();
			glutSetWindow( MainWindow );
			glFinish();
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Button ID %d\n", id );
	}

}

void rainbow_color(float s,
	float s_max,
	float s_min,
	float rgb[3])
	
{
	
	float t = (s - s_min) / (s_max - s_min);
	// make sure t is between 0 and 1, if not, rgb should be black
	if (t < 0) {
		t = 0;
	}
	if (t > 100) {
		t = 100;
	}
	float hsv[3] = { 1. };
	// map the scalar value linearly to the hue channel of the HSV
	hsv[0] = (1. -  t) * 240;
	hsv[1] = hsv[2] = 1.; // set the saturation and value as 1
	// Call the HSV to RGB conversion function
    HsvRgb(hsv, current_rgb);
	
}
void bwr_color(float s,
	float s_max,
	float s_min)

{

	float t = (s - s_min) / (s_max - s_min);
	// make sure t is between 0 and 1, if not, rgb should be black
	if (t < 0 || t>1) {
		current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;
		return;
	}
	float hsv[3] = { 1. };
	// map the scalar value linearly to the hue channel of the HSV
	if (t >=0 && t<=(set_value*0.1))
	{
		hsv[0] = 240.;
		hsv[1] = (1 - 2*t);
		hsv[2] = 1.;
	}
	else if(t>= (set_value*0.1) && t<=1)
	{
		hsv[0] = 0.;
		hsv[1] = (2*t-1);
		hsv[2] = 1.;
	}
	// set the saturation and value as 1
	// Call the HSV to RGB conversion function
	HsvRgb(hsv, current_rgb);

}

void nonlrainbow_color(float s,
	float s_max,
	float s_min)

{
	float t = (s - s_min) / (s_max - s_min);
	float Nonlinear_t = (t*t);
	
	if (Nonlinear_t < 0 || Nonlinear_t>1) {
		current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;
		return;
	}
	float hsv[3] = { 1. };
	
	hsv[0] = (1. - Nonlinear_t) * 240;
	hsv[1] = hsv[2] = 1.; 
	
	HsvRgb(hsv, current_rgb);
	
}
void heat1_color(float s,
	float s_max,
	float s_min)

{//just using hsv for rainbow.
	// Not used this fucntion.

	float t = (s - s_min) / (s_max - s_min);
	// make sure t is between 0 and 1, if not, rgb should be black
	if (t < 0 || t>1) {
		current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;
		return;
	}
	float hsv[3] = { 1. };
	// map the scalar value linearly to the hue channel of the HSV
	if (t >= 0 && t <= 0.33)
	{
		hsv[0] = 0.;
		hsv[1] = 3*t;
		hsv[2] = 1.;
	}
	else if (t >= 0.33 && t <= 0.66)
	{
		hsv[0] = 60.;
		hsv[1] = 3*t-1;
		hsv[2] = 1.;
	}
	else {
		hsv[0] = 240.;
		hsv[1] = 3 * t - 2;
		hsv[2] = 1.;
	}
	// set the saturation and value as 1
	// Call the HSV to RGB conversion function
	HsvRgb(hsv, current_rgb);

}
void heat_color(float s,
	float s_max,
	float s_min)
	

{
	float r,g,b;

	float t = (s - s_min) / (s_max - s_min);
	// make sure t is between 0 and 1, if not, rgb should be black
	if (t < 0 || t>1) {
		current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;
		return;
	}

	// map the scalar value linearly to the hue channel of the HSV
	if (t>=0 && t <= 0.333)
	{
		r =3*t;
		g = 0;
		b = 0;
			
	}
	else if(t>0.333&& t<=0.666)
	{
		r = 1;
		g = (3*t-1);
		b= 0;
	}
	else {
		r = 1;
		g = 1;
		b = (3*t-2);
	}

	current_rgb[0] = r;
	current_rgb[1] = g;
	current_rgb[2] = b;
	

}

void discrete_color(float s,
	float s_max,
	float s_min)

{

	float r, g, b;

	float t = (s - s_min) / (s_max - s_min);
	// make sure t is between 0 and 1, if not, rgb should be black
	if (t < 0 || t>1) {
		current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;
		return;
	}

	// map the scalar value linearly to the hue channel of the HSV
	if (t >= 0 && t <= 0.1)
	{
		r = 0.50;
		g = 0;
		b = 0;

	}
	else if (t > 0.1 && t <= 0.2)
	{
		r = 0.66;
		g = 0.43;
		b = 0.15;
	}
	else if (t > 0.2 && t <= 0.3)
	{
		r = 0;
		g = 0;
		b = 0.50;
	}
	else if (t > 0.3 &&t <= 0.4)
	{
		r = 0;
		g = 0.50;
		b = 0;
	}
	else if (t > 0.4 &&t <= 0.5)
	{
		r = 1;
		g = 1;
		b = 0.09;
	}
	else if (t > 0.5 &&t <= 0.6)
	{
		r = 0.56;
		g = 0.11;
		b = 0.70;
	}
	else if (t > 0.6 &&t <= 0.7)
	{
		r = 0.94;
		g = 0.19;
		b = 0.90;
	}
	else if (t > 0.7 &&t <= 0.8)
	{
		r = 0.96;
		g = 0.50;
		b = 0.18;
	}
	else if (t > 0.8 &&t <= 0.9)
	{
		r = 0.27;
		g = 0.94;
		b = 0.94;
	}
	else {
		r = 0.66;
		g = 1;
		b = 0.76;
	}

	current_rgb[0] = r;
	current_rgb[1] = g;
	current_rgb[2] = b;

}
	// set the saturation and value as 1
	// Call the HSV to RGB conversion function
	



//
// draw the complete scene:
//

void
Display( void )
{
	GLsizei vx, vy, v;		// viewport dimensions
	GLint xl, yb;		// lower-left corner of viewport
	GLfloat scale2;		// real glui scale factor

	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_COLOR_MATERIAL);


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	vx = glutGet( GLUT_WINDOW_WIDTH );
	vy = glutGet( GLUT_WINDOW_HEIGHT );
	v = vx < vy ? vx : vy;			// minimum dimension
	xl = ( vx - v ) / 2;
	yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D() IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();


	// set the eye position, look-at position, and up-vector:
	// IF DOING 2D, REMOVE THIS -- OTHERWISE ALL YOUR 2D WILL DISAPPEAR !

	gluLookAt( 0., 0., 3.,     0., 0., 0.,     0., 1., 0. );


	// translate the objects in the scene:
	// note the minus sign on the z value
	// this is to make the appearance of the glui z translate
	// widget more intuitively match the translate behavior
	// DO NOT TRANSLATE IN Z IF YOU ARE DOING 2D !

	glTranslatef( (GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2] );


	// rotate the scene:
	// DO NOT ROTATE (EXCEPT ABOUT Z) IF YOU ARE DOING 2D !

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );
	glMultMatrixf( (const GLfloat *) RotMatrix );


	// uniformly scale the scene:

	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	scale2 = 1. + Scale2;		// because glui translation starts at 0.
	if( scale2 < MINSCALE )
		scale2 = MINSCALE;
	glScalef( (GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2 );


	// set the fog parameters:
	// DON'T NEED THIS IF DOING 2D !

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// Let us disable lighting right now
	glDisable(GL_LIGHTING);


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// set the color of the object:

	glColor3fv( Colors[WhichColor] );


	// Render the loaded object
	set_view(GL_RENDER, poly);
	
	glTranslatef(0.0, 0.0, -3.0);

	glTranslatef( (GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2] );
	
	glTranslatef(poly->center.entry[0], poly->center.entry[1], poly->center.entry[2]);

	glScalef( (GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2 );
	
	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );
	glMultMatrixf( (const GLfloat *) RotMatrix );
	

	glScalef(1.0/poly->radius, 1.0/poly->radius, 1.0/poly->radius);
	glTranslatef(-poly->center.entry[0], -poly->center.entry[1], -poly->center.entry[2]);

	//calculate_max_min(poly);

	display_shape(GL_RENDER, poly);


	// swap the double-buffered framebuffers:

	glutSwapBuffers();


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush() here, not glFinish() !

	glFlush();
	

}



//
// use glut to display a string of characters using a raster font:
//

void
DoRasterString( float x, float y, float z, char *s )
{
	char c;			// one character to print

	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}



//
// use glut to display a string of characters using a stroke font:
//

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	char c;			// one character to print
	float sf;		// the scale factor

	glPushMatrix();
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		sf = ht / ( 119.05 + 33.33 );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix();
}



//
// return the number of seconds since the start of the program:
//

float
ElapsedSeconds( void )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.;
}



//
// initialize the glui window:
//

void
InitGlui( void )
{
	GLUI_Panel *panel;
	GLUI_RadioGroup *group;
	GLUI_Rotation *rot;
	GLUI_Translation *trans, *scale;
	GLUI_Panel *color_scheme; 
	GLUI_Spinner *myspinner;
	GLUI_Spinner *myspinner1;
	GLUI_Spinner *myspinner2;
	GLUI_Spinner *myspinner3;
	GLUI_Spinner *myspinner4;
	GLUI_Spinner *myspinner5;
	GLUI_Spinner *myspinner6;
	GLUI_Spinner *myspinner7;
	GLUI_Spinner *myspinner8;



	// setup the glui window:

	glutInitWindowPosition( INIT_WINDOW_SIZE + 50, 0 );
	Glui = GLUI_Master.create_glui( (char *) GLUITITLE );


	Glui->add_statictext( (char *) GLUITITLE );
	Glui->add_separator();

	Glui->add_checkbox( "XY", &displayXY );

	Glui->add_checkbox( "YZ", &displayYZ );

	Glui->add_checkbox( "ZX", &displayZX );

	Glui->add_checkbox("Wireframe", &isWireframeOn);

	Glui->add_checkbox("Bilinear", &bilinear);

	Glui->add_checkbox("Volume Rendering", &vrendering);

	// Add a rollout for the axes color
	GLUI_Rollout *rollout = Glui->add_rollout(" Axes Color ", 0);
	

	//panel = Glui->add_panel(  "Axes Color" );
	//GLUI_Rollout *rollout = Glui->add_rollout_to_panel(panel,  "Axes Color", 1 );
		//group = Glui->add_radiogroup_to_panel( panel, &WhichColor );

	    myspinner = Glui->add_spinner("X plane value",GLUI_SPINNER_INT,&Xval);
		myspinner->set_int_limits(0,NX);
		
		myspinner1 = Glui->add_spinner("Y plane value", GLUI_SPINNER_INT, &Yval);
		myspinner1->set_int_limits(0, NY);

		myspinner2 = Glui->add_spinner("Z plane value", GLUI_SPINNER_INT, &Zval);
		myspinner2->set_int_limits(0, NZ);

		myspinner3 = Glui->add_spinner("T min", GLUI_SPINNER_INT, &TMIN);
		myspinner3->set_int_limits(0, 100);

		myspinner4 = Glui->add_spinner("T max", GLUI_SPINNER_INT, &TMAX);
		myspinner4->set_int_limits(0, 100);

		myspinner5 = Glui->add_spinner("Min Grad", GLUI_SPINNER_INT, &GMIN);
		

		myspinner6 = Glui->add_spinner("Max Grad", GLUI_SPINNER_INT, &GMAX);
				
		myspinner7 = Glui->add_spinner("T value for wireframe", GLUI_SPINNER_INT, &set_s);
		myspinner7->set_int_limits(0, 100);

		myspinner8 = Glui->add_spinner("Opacity", GLUI_SPINNER_INT, &set_opacity);
		myspinner8->set_int_limits(0, 100);

		group = Glui->add_radiogroup_to_panel( rollout, &WhichColor );
			Glui->add_radiobutton_to_group( group, "Red" );
			Glui->add_radiobutton_to_group( group, "Yellow" );
			Glui->add_radiobutton_to_group( group, "Green" );
			Glui->add_radiobutton_to_group( group, "Cyan" );
			Glui->add_radiobutton_to_group( group, "Blue" );
			Glui->add_radiobutton_to_group( group, "Magenta" );
			Glui->add_radiobutton_to_group( group, "White" );
			Glui->add_radiobutton_to_group( group, "Black" );

			color_scheme = Glui->add_panel("Choose Color Scheme ");
			GLUI_Listbox *clr_list = Glui->add_listbox_to_panel(color_scheme, "Objects", &colorId, -1, (GLUI_Update_CB)Choose_Object);
			clr_list->add_item(0, "Rainbow");
			clr_list->add_item(1, "BWR");
			clr_list->add_item(2, "HeatMap");
			clr_list->add_item(3, "Discrete");
			clr_list->add_item(4, "Nonlinear_rainbow");

			

			


	// Add a list for the different models
	//rollout = Glui->add_rollout(" Models ", 0);
	/*panel = Glui->add_panel(  "Choose object to open " );
			GLUI_Listbox *obj_list = Glui->add_listbox_to_panel(panel, "Objects", &ObjectId, -1, ( GLUI_Update_CB) Choose_Object);
			//obj_list->add_item (0, "bunny");
			//obj_list->add_item (1, "feline");
			//obj_list->add_item (2, "dragon");
			//obj_list->add_item (3, "happy");
			//obj_list->add_item (4, "sphere");
			//obj_list->add_item (5, "torus");
			//obj_list->add_item(6, "diesel_field1");
			//obj_list->add_item(7, "distance_field1");
			//obj_list->add_item(8, "distance_field2");
			//obj_list->add_item(9, "iceland_current_field");
			//obj_list->add_item(10, "torus_field");
			obj_list->add_item(0, "temperature1");
			obj_list->add_item(1, "temperature2");*/

	panel = Glui->add_panel( "Object Transformation" );

		rot = Glui->add_rotation_to_panel( panel, "Rotation", (float *) RotMatrix );

		// allow the object to be spun via the glui rotation widget:

		rot->set_spin( 1.0 );


		Glui->add_column_to_panel( panel, GLUIFALSE );
		scale = Glui->add_translation_to_panel( panel, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.005f );

		Glui->add_column_to_panel( panel, GLUIFALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0] );
		trans->set_speed( 0.05f );

		Glui->add_column_to_panel( panel, GLUIFALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans Z",  GLUI_TRANSLATION_Z , &TransXYZ[2] );
		trans->set_speed( 0.05f );

	Glui->add_checkbox( "Debug", &DebugOn );
	


	panel = Glui->add_panel( "", GLUIFALSE );

	Glui->add_button_to_panel( panel, "Reset", RESET, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, GLUIFALSE );

	Glui->add_button_to_panel( panel, "Quit", QUIT, (GLUI_Update_CB) Buttons );


	// tell glui what graphics window it needs to post a redisplay to:

	Glui->set_main_gfx_window( MainWindow );


	// set the graphics window's idle function:

	GLUI_Master.set_glutIdleFunc( NULL );
}



//
// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
//

void
InitGraphics( void )
{
	// setup the display mode:
	// ( *must* be done before call to glutCreateWindow() )
	// ask for color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );


	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );


	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );


	// setup the clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );
	

	// setup the callback routines:


	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	//glutDisplayFunc( Display_Model );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( 0, NULL, 0 );

	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui()
}




//
// initialize the display lists that will not change:
//

void
InitLists( void )
{
	float dx = BOXSIZE / 2.;
	float dy = BOXSIZE / 2.;
	float dz = BOXSIZE / 2.;

	// create the object:

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		glBegin( GL_QUADS );

			glColor3f( 0., 0., 1. );
			glNormal3f( 0., 0.,  1. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glColor3f( 1., 0., 0. );
			glNormal3f(  1., 0., 0. );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

			glColor3f( 0., 1., 0. );
			glNormal3f( 0.,  1., 0. );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

		glEnd();

	glEndList();


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList();
}



//
// the keyboard callback:
//

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			Buttons( QUIT );	// will not return here
			break;			// happy compiler

		case 'r':
		case 'R':
			LeftButton = ROTATE;
			break;

		case 's':
		case 'S':
			LeftButton = SCALE;
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}


	// synchronize the GLUI display with the variables:

	Glui->sync_live();


	// force a call to Display():

	glutSetWindow( MainWindow );
	glutPostRedisplay();
}



//
// called when the mouse button transitions down or up:
//

void
MouseButton( int button, int state, int x, int y )
{
	int b;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}



//
// called when the mouse moves while a button is down:
//

void
MouseMotion( int x, int y )
{
	int dx, dy;		// change in mouse coordinates

	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	dx = x - Xmouse;		// change in mouse coords
	dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		switch( LeftButton )
		{
			case ROTATE:
				Xrot += ( ANGFACT*dy );
				Yrot += ( ANGFACT*dx );
				break;

			case SCALE:
				Scale += SCLFACT * (float) ( dx - dy );
				if( Scale < MINSCALE )
					Scale = MINSCALE;
				break;
		}
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay();
}



//
// reset the transformations and the colors:
//
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
//

void
Reset( void )
{
	ActiveButton = 0;
	AxesOn = GLUITRUE;
	DebugOn = GLUIFALSE;
	DepthCueOn = GLUIFALSE;
	LeftButton = ROTATE;
	Scale  = 1.0;
	Scale2 = 0.0;		// because we add 1. to it in Display()
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;
	current_rgb[0] = current_rgb[1] = current_rgb[2] = 0.;

	                  RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;
}



//
// called when user resizes the window:
//

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display():

	glutSetWindow( MainWindow );
	glutPostRedisplay();
}


//
// handle a change to the window's visibility:
//

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay();
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}




//////////////////////////////////////////  EXTRA HANDY UTILITIES:  /////////////////////////////

// size of wings as fraction of length:

#define WINGS	0.10


// axes:

#define X	1
#define Y	2
#define Z	3


// x, y, z, axes:

static float axx[3] = { 1., 0., 0. };
static float ayy[3] = { 0., 1., 0. };
static float azz[3] = { 0., 0., 1. };


void
Arrow( float tail[3], float head[3] )
{
	float u[3], v[3], w[3];		// arrow coordinate system
	float d;			// wing distance
	float x, y, z;			// point to plot
	float mag;			// magnitude of major direction
	float f;			// fabs of magnitude
	int axis;			// which axis is the major


	// set w direction in u-v-w coordinate system:

	w[0] = head[0] - tail[0];
	w[1] = head[1] - tail[1];
	w[2] = head[2] - tail[2];


	// determine major direction:

	axis = X;
	mag = fabs( w[0] );
	if( (f=fabs(w[1]))  > mag )
	{
		axis = Y;
		mag = f;
	}
	if( (f=fabs(w[2]))  > mag )
	{
		axis = Z;
		mag = f;
	}


	// set size of wings and turn w into a Unit vector:

	d = WINGS * Unit( w, w );


	// draw the shaft of the arrow:

	glBegin( GL_LINE_STRIP );
		glVertex3fv( tail );
		glVertex3fv( head );
	glEnd();

	// draw two sets of wings in the non-major directions:

	if( axis != X )
	{
		Cross( w, axx, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
	}


	if( axis != Y )
	{
		Cross( w, ayy, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
	}



	if( axis != Z )
	{
		Cross( w, azz, v );
		(void) Unit( v, v );
		Cross( v, w, u  );
		x = head[0] + d * ( u[0] - w[0] );
		y = head[1] + d * ( u[1] - w[1] );
		z = head[2] + d * ( u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
		x = head[0] + d * ( -u[0] - w[0] );
		y = head[1] + d * ( -u[1] - w[1] );
		z = head[2] + d * ( -u[2] - w[2] );
		glBegin( GL_LINE_STRIP );
			glVertex3fv( head );
			glVertex3f( x, y, z );
		glEnd();
	}
}



float
Dot( float v1[3], float v2[3] )
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}



void
Cross( float v1[3], float v2[3], float vout[3] )
{
	float tmp[3];

	tmp[0] = v1[1]*v2[2] - v2[1]*v1[2];
	tmp[1] = v2[0]*v1[2] - v1[0]*v2[2];
	tmp[2] = v1[0]*v2[1] - v2[0]*v1[1];

	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}



float
Unit( float vin[3], float vout[3] )
{
	float dist, f ;

	dist = vin[0]*vin[0] + vin[1]*vin[1] + vin[2]*vin[2];

	if( dist > 0.0 )
	{
		dist = sqrt( dist );
		f = 1. / dist;
		vout[0] = f * vin[0];
		vout[1] = f * vin[1];
		vout[2] = f * vin[2];
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}

	return dist;
}



// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[] = {
		1, 2, -3, 4
		};


static float yx[] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[] = {
		1, 2, 3, -2, 4
		};


static float zx[] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[] = {
		1, 2, 3, 4, -5, 6
		};


// fraction of the length to use as height of the characters:

const float LENFRAC = 0.10f;


// fraction of length to use as start location of the characters:

const float BASEFRAC = 1.10f;


//
//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)
//

void
Axes( float length )
{
	int i, j;			// counters
	float fact;			// character scale factor
	float base;			// character start location


	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd();
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd();

	fact = LENFRAC * length;
	base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 4; i++ )
		{
			j = xorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd();

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 5; i++ )
		{
			j = yorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd();

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 6; i++ )
		{
			j = zorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd();

}




//
// routine to convert HSV to RGB
//
// Reference:  Foley, van Dam, Feiner, Hughes,
//		"Computer Graphics Principles and Practices,"
//		Additon-Wesley, 1990, pp592-593.


void
HsvRgb( float hsv[3], float rgb[3] )//converting hsv to rgb
{
	float h, s, v;			// hue, sat, value
	float r, g, b;			// red, green, blue
	float i, f, p, q, t;		// interim values


	// guarantee valid input:

	h = hsv[0] / 60.;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;


	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		current_rgb[0] = current_rgb[1] = current_rgb[2] = v;
		return;
	}


	// get an rgb from the hue itself:
	
	i = floor( h );
	f = h - i;
	p = v * ( 1. - s );
	q = v * ( 1. - s*f );
	t = v * ( 1. - ( s * (1.-f) ) );

	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	current_rgb[0] = r;
	current_rgb[1] = g;
	current_rgb[2] = b;
}

// Set the view and the lighting properties
//

void set_view(GLenum mode, Polyhedron *poly)
{
	icVector3 up, ray, view;
	GLfloat light_ambient0[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_diffuse0[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat light_specular0[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_ambient1[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse1[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular1[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_ambient2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);


    glMatrixMode(GL_PROJECTION);
	if (mode == GL_RENDER)
		glLoadIdentity();

	if (WhichProjection == ORTHO)
		glOrtho(-radius_factor, radius_factor, -radius_factor, radius_factor, 0.0, 40.0);
	else
		gluPerspective(45.0, 1.0, 0.1, 40.0);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	light_position[0] = 5.5;
	light_position[1] = 0.0;
	light_position[2] = 0.0;
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	light_position[0] = -0.1;
	light_position[1] = 0.0;
	light_position[2] = 0.0;
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
}

//
// Set the scene for the object based on the center of the object

void set_scene(GLenum mode, Polyhedron *poly)
{
	glTranslatef(0.0, 0.0, -3.0);

	glScalef(1.0/poly->radius, 1.0/poly->radius, 1.0/poly->radius);
	glTranslatef(-poly->center.entry[0], -poly->center.entry[1], -poly->center.entry[2]);
}

//
// OpenGL rendering of the object

void display_shape(GLenum mode, Polyhedron *this_poly)
{
	
 	unsigned int i, j;
	GLfloat mat_diffuse[4];

	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (1., 1.);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Set the material property (esp. color) of the object
	mat_diffuse[0] = 0.6;
	mat_diffuse[1] = 0.8;
	mat_diffuse[2] = 0.7;
	mat_diffuse[3] = 1.0;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// vertex_m
	//levi
	
	for (int i = 0; i < 62; i++)
	{
		for (int j = 0; j < 62; j++)
		{
			for (int k = 0; k < 62; k++)
			{
				if (isWireframeOn == 1) {
					contour_calculationXY(i, j, k);
					contour_calculationYZ(i, j, k);
					contour_calculationZX(i, j, k);
				}
				else {
					if (grid_3d[i][j][k].T >= TMIN && grid_3d[i][j][k].T <= TMAX && grid_3d[i][j][k].grad <= (GMAX + 1) && grid_3d[i][j][k].grad >= GMIN) {

						if (displayXY == 1) {
							glBegin(GL_QUADS);
							//k is constant
							glColor3f(grid_3d[i][j][Xval].r, grid_3d[i][j][Xval].g, grid_3d[i][j][Xval].b);
							glVertex3f(grid_3d[i][j][Xval].x, grid_3d[i][j][Xval].y, grid_3d[i][j][Xval].z);
							glColor3f(grid_3d[i + 1][j][Xval].r, grid_3d[i + 1][j][Xval].g, grid_3d[i + 1][j][Xval].b);
							glVertex3f(grid_3d[i + 1][j][Xval].x, grid_3d[i + 1][j][Xval].y, grid_3d[i + 1][j][Xval].z);
							glColor3f(grid_3d[i + 1][j + 1][Xval].r, grid_3d[i + 1][j + 1][Xval].g, grid_3d[i + 1][j + 1][Xval].b);
							glVertex3f(grid_3d[i + 1][j + 1][Xval].x, grid_3d[i + 1][j + 1][Xval].y, grid_3d[i + 1][j + 1][Xval].z);
							glColor3f(grid_3d[i][j + 1][Xval].r, grid_3d[i][j + 1][Xval].g, grid_3d[i][j + 1][Xval].b);
							glVertex3f(grid_3d[i][j + 1][Xval].x, grid_3d[i][j + 1][Xval].y, grid_3d[i][j + 1][Xval].z);
							glEnd();
						}

						if (displayYZ == 1) {
							glBegin(GL_QUADS);
							//j is constant
							glColor3f(grid_3d[i][Yval][k].r, grid_3d[i][Yval][k].g, grid_3d[i][Yval][k].b);
							glVertex3f(grid_3d[i][Yval][k].x, grid_3d[i][Yval][k].y, grid_3d[i][Yval][k].z);
							glColor3f(grid_3d[i + 1][Yval][k].r, grid_3d[i + 1][Yval][k].g, grid_3d[i + 1][Yval][k].b);
							glVertex3f(grid_3d[i + 1][Yval][k].x, grid_3d[i + 1][Yval][k].y, grid_3d[i + 1][Yval][k].z);
							glColor3f(grid_3d[i + 1][Yval][k + 1].r, grid_3d[i + 1][Yval][k + 1].g, grid_3d[i + 1][Yval][k + 1].b);
							glVertex3f(grid_3d[i + 1][Yval][k + 1].x, grid_3d[i + 1][Yval][k + 1].y, grid_3d[i + 1][Yval][k + 1].z);
							glColor3f(grid_3d[i][Yval][k + 1].r, grid_3d[i][Yval][k + 1].g, grid_3d[i][Yval][k + 1].b);
							glVertex3f(grid_3d[i][Yval][k + 1].x, grid_3d[i][Yval][k + 1].y, grid_3d[i][Yval][k + 1].z);
							glEnd();
						}

						if (displayZX == 1) {
							glBegin(GL_QUADS);
							//i is constant
							glColor3f(grid_3d[Zval][j][k].r, grid_3d[Zval][j][k].g, grid_3d[Zval][j][k].b);
							glVertex3f(grid_3d[Zval][j][k].x, grid_3d[Zval][j][k].y, grid_3d[Zval][j][k].z);
							glColor3f(grid_3d[Zval][j + 1][k].r, grid_3d[Zval][j + 1][k].g, grid_3d[Zval][j + 1][k].b);
							glVertex3f(grid_3d[Zval][j + 1][k].x, grid_3d[Zval][j + 1][k].y, grid_3d[Zval][j + 1][k].z);
							glColor3f(grid_3d[Zval][j + 1][k + 1].r, grid_3d[Zval][j + 1][k + 1].g, grid_3d[Zval][j + 1][k + 1].b);
							glVertex3f(grid_3d[Zval][j + 1][k + 1].x, grid_3d[Zval][j + 1][k + 1].y, grid_3d[Zval][j + 1][k + 1].z);
							glColor3f(grid_3d[Zval][j][k + 1].r, grid_3d[Zval][j][k + 1].g, grid_3d[Zval][j][k + 1].b);
							glVertex3f(grid_3d[Zval][j][k + 1].x, grid_3d[Zval][j][k + 1].y, grid_3d[Zval][j][k + 1].z);
							glEnd();
						}

					}
				}
			}
		}
	}
	if (vrendering == 1)
	{
		DetermineVisibility();
		CompositeXY();
		CompositeYZ();
		CompositeXZ();
		mapping();
	}
	/*

	for (i = 0; i < NY-2; i++){
		for (j = 0; j < NX - 2; j++)


		{
			if (mode == GL_SELECT)
				glLoadName(i + 1);

			glBegin(GL_QUADS);
			
			rainbow_color(grid_pts[j+(i*NY)].s, max, min, current_rgb);
			glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
		    glVertex3f(grid_pts[(i*NX)+j].x, grid_pts[(i*NX)+j].y, grid_pts[(i*NX)+j].z);

			rainbow_color(grid_pts[(j+1) + (i*NX)].s, max, min, current_rgb);
			glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
			glVertex3f(grid_pts[(i*NX)+j+1].x, grid_pts[(i*NX)+j+1].y, grid_pts[(i*NX)+j+1].z);

			rainbow_color(grid_pts[(j+NX+1) + (i*NX)].s, max, min, current_rgb);
			glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
			glVertex3f(grid_pts[(i*NX)+(j+NX+1)].x, grid_pts[(i*NX )+ (j+NX+1)].y, grid_pts[(i*NX)+(j+NX+1)].z);

			rainbow_color(grid_pts[(j+NX) + (i*NX)].s, max, min, current_rgb);
			glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
			glVertex3f(grid_pts[(i*NX) + (j+NX)].x, grid_pts[(i*NX) + (j+NX)].y, grid_pts[(i*NX)+(j+NX)].z);
			glEnd();
			
			
			
			
			for (int a = 0; a < set_contours; a++)
			{
				if (set_contours != 1)
				{
					set_s_value1(a);
				}
				contour_calculation(i, j);
			}
			
		}


	}*/
	/*
	for (i=0; i<this_poly->ntris; i++) {
		if (mode == GL_SELECT)
			glLoadName(i+1);

		Triangle *temp_t=this_poly->tlist[i];

			

				switch (colorId) {
				case RAINBOW:
					strcpy(color_name, "Rainbow");
					glBegin(GL_POLYGON);
					for (j = 0; j < 3; j++) {
						Vertex *temp_v = temp_t->verts[j];
						rainbow_color(temp_v->s, max, min, current_rgb);
						glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
						glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
						glVertex3d(temp_v->x, temp_v->y, temp_v->z);
					}glEnd();
					break;

				case BWR:
					strcpy(color_name, "BWR");
					glBegin(GL_POLYGON);
					for (j = 0; j < 3; j++) {
						Vertex *temp_v = temp_t->verts[j];
						bwr_color(temp_v->s, max, min);
						glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
						glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
						glVertex3d(temp_v->x, temp_v->y, temp_v->z);
					}glEnd();
					break;

				case HEATMAP:
					strcpy(color_name, "HeatMap");
					glBegin(GL_POLYGON);
					for (j = 0; j < 3; j++) {
						Vertex *temp_v = temp_t->verts[j];

						//heat_color(temp_v->s, 1, 0);
						heat_color(temp_v->s, max, min);
						glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
						glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
						glVertex3d(temp_v->x, temp_v->y, temp_v->z);
					}glEnd();
					break;

				case DISCRETE:
					strcpy(color_name, "Discrete");
					glBegin(GL_POLYGON);
					for (j = 0; j < 3; j++) {
						Vertex *temp_v = temp_t->verts[j];
						discrete_color(temp_v->s, max, min);
						glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
						glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
						glVertex3d(temp_v->x, temp_v->y, temp_v->z);
					}glEnd();
				    break;

				case NONLINEAR_RAINBOW:
					strcpy(color_name, "Nonlinear_rainbow");
					glBegin(GL_POLYGON);
					for (j = 0; j < 3; j++) {
						Vertex *temp_v = temp_t->verts[j];
						nonlrainbow_color(temp_v->s, max, min);
						glColor3f(current_rgb[0], current_rgb[1], current_rgb[2]);
						glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
						glVertex3d(temp_v->x, temp_v->y, temp_v->z);
					}glEnd();
					break;


				}


				
				/*Vertex *temp_v = temp_t->verts[j];
				//rainbow_color(temp_v->s, 1, 0,current_rgb);
				//bwr_color(temp_v->s, 1, 0);
				//heat_color(temp_v->s, 1, 0);
				heat1_color(temp_v->s, 1, 0);*/

				/*glColor3f(current_rgb[0],current_rgb[1],current_rgb[2]);
				glNormal3d(temp_v->normal.entry[0], temp_v->normal.entry[1], temp_v->normal.entry[2]);
				glVertex3d(temp_v->x, temp_v->y, temp_v->z);
			

	}*/
}


void Display_Model(void)
{
	GLint viewport[4];
	int jitter;

	glClearColor (1.0, 1.0, 1.0, 1.0);  // background for rendering color coding and lighting
	glGetIntegerv (GL_VIEWPORT, viewport);
 
	set_view(GL_RENDER, poly);
	set_scene(GL_RENDER, poly);
	display_shape(GL_RENDER, poly);
	
	glFlush();
	glutSwapBuffers();
	glFinish();
}


void Choose_Object()
{
	/*
	int w, h;
	switch(ObjectId){
		case BUNNY:
			strcpy(object_name, "bunny");
			break;

		case FELINE:
			strcpy(object_name, "feline");
			break;

		case DRAGON:
			strcpy(object_name, "dragon");
			break;

		case HAPPY:
			strcpy(object_name, "happy");
			break;

		case TORUS:
			strcpy(object_name, "torus");
			break;

		case SPHERE:
			strcpy(object_name, "sphere");
			break;

		case DIESEL_FIELD1:
			strcpy(object_name, "diesel_field1");
			break;

		case DISTANCE_FIELD1:
			strcpy(object_name, "distance_field1");
			break;

		case DISTANCE_FIELD2:
			strcpy(object_name, "distance_field2");
			break;

		case ICELAND_CURRENT_FIELD:
			strcpy(object_name, "iceland_current_field");
			break;

		case TORUS_FIELD:
			strcpy(object_name, "torus_field");
			break;

		case TEMPERATURE1:
			strcpy(object_name, "temperature1");
			break;

		case TEMPERATURE2:
			strcpy(object_name, "temperature2");
			break;

			*/
	coordinates_calculation();

//	}



	//poly->finalize();

    //Reset();

//	char tmp_str[512];

//	sprintf (tmp_str, "../models/%s.dat", object_name);
//	Load_data_on_uniformGrids(tmp_str);
	/*

	FILE *this_file = fopen(tmp_str, "r");
	poly = new Polyhedron (this_file);
	fclose(this_file);

    ////Following codes build the edge information
	clock_t start, finish; //Used to show the time assumed
	start = clock(); //Get the first time

	poly->initialize(); // initialize everything

	poly->calc_bounding_sphere();
	poly->calc_face_normals_and_area();
	poly->average_normals();
	
	finish = clock(); //Get the current time after finished
	double t = (double)(finish - start)/CLOCKS_PER_SEC;

	printf("\n");
	printf("The number of the edges of the object %s is %d \n",object_name, poly->nedges);
	printf("The Euler Characteristics of the object %s is %d \n",object_name, (poly->nverts - poly->nedges + poly->ntris));

	printf("Time to building the edge link is %f seconds\n", t);
			*/	
	//Glui->sync_live();
	//glutSetWindow( MainWindow );
	//glutPostRedisplay();
	
	
}

void calculate_max_min(Polyhedron *this_poly)
{
	max = 0.;
	min = 0.;
	for (int i = 0; i < this_poly->nverts; i++)
	{
		if (min>this_poly->vlist[i]->s)
		{
			min = this_poly->vlist[i]->s;
			
		}
		if (max < this_poly->vlist[i]->s)
		{
			max = this_poly->vlist[i]->s;
		}
		
	}

}

void calculate_max_min_1() //this is for quad.
{
	
	for (int i = 0; i < grid_pts.size(); i++)
	{
		if (min > grid_pts[i].s)
		{
			min = grid_pts[i].s;

		}
		if (max < grid_pts[i].s)
		{
			max = grid_pts[i].s;
		}

	}

}

void Load_data_on_uniformGrids(const char *name)
{
	int i;
	FILE *fp = fopen(name, "r");
	if (fp == NULL) return;
	fscanf(fp, "%d %d\n", &NX, &NY);
	grid_pts.clear();
	
		for (i = 0; i < NX*NY; i++)
		{
			node tmp;
			fscanf(fp, "%f, %f, %f, %f \n", &tmp.x, &tmp.y, &tmp.z, &tmp.x);//change1
			//grid_pts.push_back(tmp);change2
		}
	fclose(fp);
}


void contour_calculationXY(int i, int j, int k)
{
	
	int intersection_counter = 0;
	float s1, s2, s3, s4;
	float t_new=0, s_new=set_s;
	
	s1= grid_3d[i][j][k].T;
	s2= grid_3d[i + 1][j][k].T; 
	s3= grid_3d[i + 1][j + 1][k].T; 
	s4 = grid_3d[i][j + 1][k].T ; 

	
	std::vector<node> points;
	node temp;

	//edge connecting s1 & s2
	if (s1 > s2)
	{
		t_new = (s_new - s2) / (s1 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i + 1][j][k].x + t_new * grid_3d[i][j][k].x;
			temp.y = grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}
		

	else if(s1 < s2) {

			t_new = (s_new - s1) / (s2 - s1);

			if (t_new > 0 && t_new < 1)
			{
				temp = grid_3d[i][j][k];
				temp.x = (1 - t_new) * grid_3d[i][j][k].x + t_new * grid_3d[i + 1][j][k].x;
				temp.y = grid_3d[i][j][k].y;
				intersection_counter++;
				points.push_back(temp);
			}

		}


	//edge connecting s2 and s3
	if (s2 > s3)
	{
		t_new = (s_new - s3) / (s2 - s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x= grid_3d[i + 1][j][k].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j + 1][k].y + t_new * grid_3d[i + 1][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if(s2 < s3) {

		t_new = (s_new - s2) /( s3 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i + 1][j][k].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j][k].y + t_new * grid_3d[i + 1][j + 1][k].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s3 and s4
	if (s3 > s4)
	{
		t_new = (s_new - s4) / (s3 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j + 1][k].x + t_new * grid_3d[i + 1][j + 1][k].x;
			temp.y= grid_3d[i][j + 1][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if(s3 < s4) {

		t_new = (s_new - s3) / (s4- s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) *  grid_3d[i + 1][j + 1][k].x + t_new * grid_3d[i][j + 1][k].x;
			temp.y = grid_3d[i][j + 1][k].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s4 and s1
	if (s1 > s4)
	{
		t_new = (s_new - s4) / (s1 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j + 1][k].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j + 1][k].y + t_new * grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s1 < s4) 
	{

		t_new = (s_new - s1) / (s4 - s1);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j + 1][k].x;
			temp.y = (1 - t_new) * grid_3d[i][j][k].y + t_new * grid_3d[i + 1][j + 1][k].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}

	switch (intersection_counter)
	{
	case 2:
		glBegin(GL_LINES);
		glColor3f(points[0].r, points[0].g, points[0].b);
		glVertex3f(points[0].x, points[0].y, points[0].z);
		glColor3f(points[1].r, points[1].g, points[1].b);
		glVertex3f(points[1].x, points[1].y, points[1].z);
		glEnd();
		break;

	case 4:
		break;


    }
}

void contour_calculationYZ(int i, int j, int k)
{

	int intersection_counter = 0;
	float s1, s2, s3, s4;
	float t_new = 0, s_new = set_s;

	s1 = grid_3d[i][j][k].T; 
	s2 = grid_3d[i][j+1][k].T; 
	s3 = grid_3d[i][j + 1][k+1].T; 
	s4 = grid_3d[i][j][k+1].T; 

	std::vector<node> points;
	node temp;

	//edge connecting s1 & s2
	if (s1 > s2)
	{
		t_new = (s_new - s2) / (s1 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j+1][k].x + t_new * grid_3d[i][j][k].x;
			temp.y = grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s1 < s2) {

		t_new = (s_new - s1) / (s2 - s1);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j][k].x + t_new * grid_3d[i][j+1][k].x;
			temp.y = grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s2 and s3
	if (s2 > s3)
	{
		t_new = (s_new - s3) / (s2 - s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j+1][k].x;
			temp.y = (1 - t_new) * grid_3d[i][j + 1][k+1].y + t_new * grid_3d[i][j+1][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s2 < s3) {

		t_new = (s_new - s2) / (s3 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j+1][k].x;
			temp.y = (1 - t_new) * grid_3d[i][j+1][k].y + t_new * grid_3d[i][j + 1][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s3 and s4
	if (s3 > s4)
	{
		t_new = (s_new - s4) / (s3 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j][k+1].x + t_new * grid_3d[i][j + 1][k+1].x;
			temp.y = grid_3d[i][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s3 < s4) {

		t_new = (s_new - s3) / (s4 - s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) *  grid_3d[i][j + 1][k+1].x + t_new * grid_3d[i][j][k+1].x;
			temp.y = grid_3d[i][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s4 and s1
	if (s1 > s4)
	{
		t_new = (s_new - s4) / (s1 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j][k+1].x;
			temp.y = (1 - t_new) * grid_3d[i][j + 1][k+1].y + t_new * grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s1 < s4)
	{

		t_new = (s_new - s1) / (s4 - s1);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j][k+1].x;
			temp.y = (1 - t_new) * grid_3d[i][j][k].y + t_new * grid_3d[i][j + 1][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}

	switch (intersection_counter)
	{
	case 2:
		glBegin(GL_LINES);
		glColor3f(points[0].r, points[0].g, points[0].b);
		glVertex3f(points[0].x, points[0].y, points[0].z);
		glColor3f(points[1].r, points[1].g, points[1].b);
		glVertex3f(points[1].x, points[1].y, points[1].z);
		glEnd();
		break;

	case 4:
		break;


	}
}

void contour_calculationZX(int i, int j, int k)
{

	int intersection_counter = 0;
	float s1, s2, s3, s4;
	float t_new = 0, s_new = set_s;

	s1 = grid_3d[i][j][k].T; 
	s2 = grid_3d[i + 1][j][k].T; 
	s3 = grid_3d[i + 1][j][k+1].T; 
	s4 = grid_3d[i][j][k+1].T; 

	

	std::vector<node> points;
	node temp;

	//edge connecting s1 & s2
	if (s1 > s2)
	{
		t_new = (s_new - s2) / (s1 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i + 1][j][k].x + t_new * grid_3d[i][j][k].x;
			temp.y = grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s1 < s2) {

		t_new = (s_new - s1) / (s2 - s1);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j][k].x + t_new * grid_3d[i + 1][j][k].x;
			temp.y = grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s2 and s3
	if (s2 > s3)
	{
		t_new = (s_new - s3) / (s2 - s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i + 1][j][k].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j][k+1].y + t_new * grid_3d[i + 1][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s2 < s3) {

		t_new = (s_new - s2) / (s3 - s2);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i + 1][j][k].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j][k].y + t_new * grid_3d[i + 1][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s3 and s4
	if (s3 > s4)
	{
		t_new = (s_new - s4) / (s3 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) * grid_3d[i][j][k+1].x + t_new * grid_3d[i + 1][j][k+1].x;
			temp.y = grid_3d[i][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s3 < s4) {

		t_new = (s_new - s3) / (s4 - s3);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = (1 - t_new) *  grid_3d[i + 1][j][k+1].x + t_new * grid_3d[i][j][k+1].x;
			temp.y = grid_3d[i][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}


	//edge connecting s4 and s1
	if (s1 > s4)
	{
		t_new = (s_new - s4) / (s1 - s4);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j][k+1].x;
			temp.y = (1 - t_new) * grid_3d[i + 1][j][k+1].y + t_new * grid_3d[i][j][k].y;
			intersection_counter++;
			points.push_back(temp);

		}
	}


	else if (s1 < s4)
	{

		t_new = (s_new - s1) / (s4 - s1);

		if (t_new > 0 && t_new < 1)
		{
			temp = grid_3d[i][j][k];
			temp.x = grid_3d[i][j][k+1].x;
			temp.y = (1 - t_new) * grid_3d[i][j][k].y + t_new * grid_3d[i + 1][j][k+1].y;
			intersection_counter++;
			points.push_back(temp);
		}

	}

	switch (intersection_counter)
	{
	case 2:
		glBegin(GL_LINES);
		glColor3f(points[0].r, points[0].g, points[0].b);
		glVertex3f(points[0].x, points[0].y, points[0].z);
		glColor3f(points[1].r, points[1].g, points[1].b);
		glVertex3f(points[1].x, points[1].y, points[1].z);
		glEnd();
		break;

	case 4:
		break;


	}
}

void set_s_value1(int a)
{
	float range = (max - min);
	set_s = (range/(set_contours+1))*(a+1);

}

double
Temperature(double x, double y, double z)
{
	double t = 0.0;
	for (int i = 0; i < 4; i++)
	{
		double dx = x - Sources[i].xc;
		double dy = y - Sources[i].yc;
		double dz = z - Sources[i].zc;
		double rsqd = dx * dx + dy * dy + dz * dz;
		t += Sources[i].a * exp(-5.*rsqd);
	}
	if (t > TEMPMAX)
		
		t = TEMPMAX;
	return t;
}

void coordinates_calculation()
{
	node temp1;
	for (int i = 0; i < NX; i++)
	{
		for (int j = 0; j < NY; j++)
		{
			for (int k = 0; k < NZ; k++)
			{
				temp1.x = ((2.0 / NX)*i) - 1;
				temp1.y = ((2.0 / NY)*j) - 1;
				temp1.z = ((2.0 / NZ)*k) - 1;
				temp1.T = Temperature(temp1.x, temp1.y, temp1.z);
				if (temp1.T > 100) {
					temp1.T = 100;
				}
				if (temp1.T < 0) {
					temp1.T = 0;
				}
				
				switch (colorId) {
				case RAINBOW:
						rainbow_color(temp1.T, TEMPMAX, TEMPMIN, current_rgb);
						temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
					break;

				case BWR:
						bwr_color(temp1.T, TEMPMAX, TEMPMIN);
						temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
					break;

				case HEATMAP:
						heat_color(temp1.T, TEMPMAX, TEMPMIN);
						temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
					break;

				case DISCRETE:
						discrete_color(temp1.T, TEMPMAX, TEMPMIN);
						temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
				
				    break;

				case NONLINEAR_RAINBOW:
						nonlrainbow_color(temp1.T, TEMPMAX, TEMPMIN);
						temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
				}
				








				/*
				rainbow_color(temp1.T, TEMPMAX, TEMPMIN, current_rgb);
				temp1.r = current_rgb[0];
				temp1.g = current_rgb[1];
				temp1.b = current_rgb[2];
				grid_3d[i][j][k] = temp1;
				
				*/
				float dTdx;
				float dTdy;
				float dTdz;

				if (i > 1 && j > 1 && k > 1) {


					// calculate grad
					if (i != 0 && i != 63) {
						dTdx = (grid_3d[i][j - 1][k - 1].T - grid_3d[i - 2][j - 1][k - 1].T) / (grid_3d[i][j - 1][k - 1].x - grid_3d[i - 2][j - 1][k - 1].x);
					}
					else {
						dTdx = 0;
					}
					if (j != 0 && j != 63) {
						dTdy = (grid_3d[i - 1][j][k - 1].T - grid_3d[i - 1][j - 2][k - 1].T) / (grid_3d[i - 1][j][k - 1].y - grid_3d[i - 1][j - 2][k - 1].y);
					}
					else {
						dTdy = 0;
					}
					if (k != 0 && k != 63) {
						dTdz = (grid_3d[i - 1][j - 1][k].T - grid_3d[i - 1][j - 1][k - 2].T) / (grid_3d[i - 1][j - 1][k].z - grid_3d[i - 1][j - 1][k - 2].z);
					}
					else {
						dTdz = 0;
					}
					grid_3d[i - 1][j - 1][k - 1].grad = sqrt(pow(dTdx, 2) + pow(dTdy, 2) + pow(dTdz, 2));
					if (grid_3d[i - 1][j - 1][k - 1].grad > GMAX) {
						GMAX = grid_3d[i - 1][j - 1][k - 1].grad;
					}
				}
				else {
					grid_3d[i][j][k].grad = 0;
				}
			}
		}
	}
}

void
CompositeXY(void)
{
	int x, y, z, zz;
	float alpha; /* opacity at this voxel */
	float r, g, b; /* running color composite */
	for (x = 0; x < NX; x++)
	{
		for (y = 0; y < NY; y++)
		{
			r = g = b = 0.;
			for (zz = 0; zz < NZ; zz++)
			{
				/* which direction to composite: */
				if (Zside == PLUS)
					z = zz;
				else
					z = (NZ - 1) - zz;
				if (grid_3d[x][y][z].T >= TMIN && grid_3d[x][y][z].T <= TMAX && grid_3d[x][y][z].grad <= (GMAX + 1) && grid_3d[x][y][z].grad >= GMIN)  // determine whether the value is out of the
				// range set by the range slider
				{
					r = grid_3d[x][y][z].r;
					g = grid_3d[x][y][z].g;
					b = grid_3d[x][y][z].b;
					alpha = set_opacity/100.;
					
				}
				else
				{
					r = g = b = 0.;
					alpha = 0.;
				}
				TextureXY[zz][y][x][0] = (unsigned char)(255.*r + .5);
				TextureXY[zz][y][x][1] = (unsigned char)(255.*g + .5);
				TextureXY[zz][y][x][2] = (unsigned char)(255.*b + .5);
				TextureXY[zz][y][x][3] = (unsigned char)(255.*alpha + .5);
			}
		}
	}
}
void
CompositeYZ(void)
{
	int x, y, z, xx;
	float alpha; /* opacity at this voxel */
	float r, g, b; /* running color composite */
	for (y = 0; y< NY; y++)
	{
		for (z = 0; z < NZ; z++)
		{
			r = g = b = 0.;
			for (xx = 0; xx < NX; xx++)
			{
				
				if (Xside == PLUS)
					x = xx;
				else
					x = (NX - 1) - xx;
				if (grid_3d[x][y][z].T >= TMIN && grid_3d[x][y][z].T <= TMAX && grid_3d[x][y][z].grad <= (GMAX + 1) && grid_3d[x][y][z].grad >= GMIN) 
				
				{
					r = grid_3d[x][y][z].r;
					g = grid_3d[x][y][z].g;
					b = grid_3d[x][y][z].b;
					alpha = set_opacity/100.;
				}
				else
				{
		
					r = g = b = 0.;
					alpha = 0.;
				}
				TextureYZ[xx][z][y][0] = (unsigned char)(255.*r + .5);
				TextureYZ[xx][z][y][1] = (unsigned char)(255.*g + .5);
				TextureYZ[xx][z][y][2] = (unsigned char)(255.*b + .5);
				TextureYZ[xx][z][y][3] = (unsigned char)(255.*alpha + .5);
			}
		}
	}
}

void
CompositeXZ(void)
{
	int x, y, z, yy;
	float alpha; /* opacity at this voxel */
	float r, g, b; /* running color composite */
	for (x = 0; x < NX; x++)
	{
		for (z = 0; z < NZ; z++)
		{
			r = g = b = 0.;
			for (yy = 0; yy < NY; yy++)
			{
				/* which direction to composite: */
				if (Yside == PLUS)
					y = yy;
				else
					y = (NY - 1) - yy;
				if (grid_3d[x][y][z].T >= TMIN && grid_3d[x][y][z].T <= TMAX && grid_3d[x][y][z].grad <= (GMAX + 1) && grid_3d[x][y][z].grad >= GMIN) // determine whether the value is out of the
				// range set by the range slider
				{
					
					r = grid_3d[x][y][z].r;
					g = grid_3d[x][y][z].g;
					b = grid_3d[x][y][z].b;
					alpha = set_opacity/100.;
				}
				else
				{
					r = g = b = 0.;
					alpha = 0.;
				}
				TextureXZ[yy][z][x][0] = (unsigned char)(255.*r + .5);
				TextureXZ[yy][z][x][1] = (unsigned char)(255.*g + .5);
				TextureXZ[yy][z][x][2] = (unsigned char)(255.*b + .5);
				TextureXZ[yy][z][x][3] = (unsigned char)(255.*alpha + .5);
			}
		}
	}
}

void DetermineVisibility()

{
	float xr, yr;
	float cx, sx;
	float cy, sy;
	float nzx, nzy, nzz; /* z component of normal for x side, y side,
	and z side */
	xr = Xrot * (M_PI / 180.);
	yr = Yrot * (M_PI / 180.);
	cx = cos(xr);
	sx = sin(xr);
	cy = cos(yr);
	sy = sin(yr);
	nzx = -sy;
	nzy = sx * cy;
	nzz = cx * cy;
	/* which sides of the cube are showing:
	*/
	/* the Xside being shown to the user is MINUS or PLUS */
	Xside = (nzx > 0. ? PLUS : MINUS);
	Yside = (nzy > 0. ? PLUS : MINUS);
	Zside = (nzz > 0. ? PLUS : MINUS);
	/* which direction needs to be composited: */
	if (fabs(nzx) > fabs(nzy) && fabs(nzx) > fabs(nzz))
		Major = X;
	else if (fabs(nzy) > fabs(nzx) && fabs(nzy) > fabs(nzz))
		Major = Y;
	else
		Major = Z;
}

void mapping()
{
	int x, y, z;
	float z0, x0, y0;
	float dz, dx, dy;
	float xcoord, ycoord, zcoord;
	GLfloat filter;
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	filter = GL_NEAREST;
	if (bilinear == 1) 
		filter = GL_LINEAR;
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if (Major==Z)
	{
		if (Zside == PLUS)
		{
			z0 = -1.;
			dz = 2. / (float)(NZ - 2);
		}
		else
		{
			z0 = 1.;
			dz = -2. / (float)(NZ - 2);
		}
		for (z = 0, zcoord = z; z < NZ; z++, zcoord += dz)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 4, NX, NY, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, &TextureXY[z][0][0][0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0., 0.);
			glVertex3f(-1., -1., zcoord);
			glTexCoord2f(1., 0.);
			glVertex3f(1., -1., zcoord);
			glTexCoord2f(1., 1.);
			glVertex3f(1., 1., zcoord);
			glTexCoord2f(0., 1.);
			glVertex3f(-1., 1., zcoord);
			glEnd();
		}
	}
	if (Major==X)
	{
		if (Xside == PLUS)
		{
			x0 = -1.;
			dx = 2. / (float)(NX - 1);
		}
		else
		{
			x0 = 1.;
			dx = -2. / (float)(NX - 1);
		}
		for (x = 0, xcoord = x0; x < NX; x++, xcoord += dx)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 4, NZ, NY, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, &TextureYZ[x][0][0][0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0., 0.);
			glVertex3f(xcoord ,-1., -1.);
			glTexCoord2f(1., 0.);
			glVertex3f(xcoord,1., -1.);
			glTexCoord2f(1., 1.);
			glVertex3f(xcoord,1., 1.);
			glTexCoord2f(0., 1.);
			glVertex3f(xcoord ,-1., 1.);
			glEnd();
		}
	}
	if (Major==Y)
	{
		if (Yside == PLUS)
		{
			y0 = -1;
			dy = 2. / (float)(NY - 1);
		}
		else
		{
			y0 = 1.;
			dy = -2. / (float)(NY - 1);
		}
		for (y = 0, ycoord = y0; y < NY; y++, ycoord += dy)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 4, NX, NZ, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, &TextureXZ[y][0][0][0]);
			glBegin(GL_QUADS);

			glTexCoord2f(0., 0.);
			glVertex3f(-1., ycoord ,-1. );
			glTexCoord2f(1., 0.);
			glVertex3f(1., ycoord, -1. );
			glTexCoord2f(1., 1.);
			glVertex3f(1., ycoord, 1.);
			glTexCoord2f(0., 1.);
			glVertex3f(-1., ycoord,1.);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}
