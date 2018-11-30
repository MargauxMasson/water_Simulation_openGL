#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <jpeglib.h>
#include <jerror.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #include <GLUT/glut.h>

#include "bmptotexture.cpp"
#include "noise.cpp"
float Time;
// #include "sphere.cpp"
#include "noise.h"
#define		RESOLUTION 64
#define		MOD	0xff

// title of these windows:

const char *WINDOWTITLE = {"Final Project - Water Simulation"};
const char *GLUITITLE = {"Water Simulation"};

// what the glui package defines as true and false:

const int GLUITRUE = {true};
const int GLUIFALSE = {false};

// the escape key:

#define ESCAPE 0x1b

// initial window size:

const int INIT_WINDOW_SIZE = {600};

// size of the box:

const float BOXSIZE = {2.f};

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = {1.};
const float SCLFACT = {0.005f};

// minimum allowable scale factor:

const float MINSCALE = {0.05f};

// active mouse buttons (or them together):

const int LEFT = {4};
const int MIDDLE = {2};
const int RIGHT = {1};

// which projection:

enum Projections
{
    ORTHO,
    PERSP
};

// which button:

enum ButtonVals
{
    RESET,
    QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[] = {0., 0., 0., 1.};

// line width for the axes:

const GLfloat AXES_WIDTH = {3.};

// the color numbers:
// this order must match the radio button order

enum Colors
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

char *ColorNames[] =
    {
        "Red",
        "Yellow",
        "Green",
        "Cyan",
        "Blue",
        "Magenta",
        "White",
        "Black"};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
    {
        {1., 0., 0.}, // red
        {1., 1., 0.}, // yellow
        {0., 1., 0.}, // green
        {0., 1., 1.}, // cyan
        {0., 0., 1.}, // blue
        {1., 0., 1.}, // magenta
        {1., 1., 1.}, // white
        {0., 0., 0.}, // black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = {.0, .0, .0, 1.};
const GLenum FOGMODE = {GL_LINEAR};
const GLfloat FOGDENSITY = {0.30f};
const GLfloat FOGSTART = {1.5};
const GLfloat FOGEND = {4.};

// non-constant global variables:

int ActiveButton;    // current button that is down
GLuint AxesList;     // list to hold the axes
int AxesOn;          // != 0 means to draw the axes
int DebugOn;         // != 0 means to print debugging info
int DepthCueOn;      // != 0 means to use intensity depth cueing
int DepthBufferOn;   // != 0 means to use the z-buffer
int DepthFightingOn; // != 0 means to use the z-buffer
GLuint BoxList;      // object display list
int MainWindow;      // window id for main graphics window
float Scale;         // scaling factor
int WhichColor;      // index into Colors[ ]
int WhichProjection; // ORTHO or PERSP
int Xmouse, Ymouse;  // mouse values
float Xrot, Yrot;    // rotation angles in degrees

int width = 1024;
int height = 512;
bool isTexture = true;
int idTexture = 0;

unsigned char *Texture1 = BmpToTexture("mercury.bmp", &width, &height);
unsigned char *Texture2 = BmpToTexture("worldtex.bmp", &width, &height);
unsigned char *Texture3 = BmpToTexture("mars.bmp", &width, &height);
unsigned char *Texture = Texture1;

/////////// WATER ////////////
// static GLuint	texture;

// static int	left_click = GLUT_UP;
// static int	right_click = GLUT_UP;
// static int	wire_frame = 0;
// static int	normals = 0;
// static int	xold = 0;
// static int	yold = 0;
// static float	rotate_x = 30;
// static float	rotate_y = 15;
// static float	translate_z = 4;

// static float	surface[6 * RESOLUTION * (RESOLUTION + 1)];
// static float	normal[6 * RESOLUTION * (RESOLUTION + 1)];

static GLuint	texture;

static int	left_click = GLUT_UP;
static int	right_click = GLUT_UP;
static int	wire_frame = 0;
static int	normals = 0;
static int	xold = 0;
static int	yold = 0;
static float	rotate_x = 30;
static float	rotate_y = 15;
static float	translate_z = 4;

static float	surface[6 * RESOLUTION * (RESOLUTION + 1)];
static float	normal[6 * RESOLUTION * (RESOLUTION + 1)];



  const float t = glutGet (GLUT_ELAPSED_TIME) / 1000.;
  const float delta = 2. / RESOLUTION;
  const unsigned int length = 2 * (RESOLUTION + 1);
  const float xn = (RESOLUTION + 1) * delta + 1;
  unsigned int i;
  unsigned int j;
  float x;
  float y;
  unsigned int indice;
  unsigned int preindice;

  float v1x;
  float v1y;
  float v1z;

  float v2x;
  float v2y;
  float v2z;

  float v3x;
  float v3y;
  float v3z;

  float vax;
  float vay;
  float vaz;

  float vbx;
  float vby;
  float vbz;

  float nx;
  float ny;
  float nz;

  float l;


static float	z (const float x, const float y, const float t)
{
  const float x2 = x - 3;
  const float y2 = y + 1;
  const float xx = x2 * x2;
  const float yy = y2 * y2;
  return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
	   Noise (10 * x, 10 * y, t, 0)) / 200);
}


/*
** Function to load a Jpeg file.
*/
int		load_texture (const char * filename,
			      unsigned char * dest,
			      const int format,
			      const unsigned int size)
{
  FILE *fd;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  unsigned char * line;

  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);

  if (0 == (fd = fopen(filename, "rb")))
    return 1;

  jpeg_stdio_src (&cinfo, fd);
  jpeg_read_header (&cinfo, TRUE);
  if ((cinfo.image_width != size) || (cinfo.image_height != size))
    return 1;

  if (GL_RGB == format)
    {
      if (cinfo.out_color_space == JCS_GRAYSCALE)
	return 1;
    }
  else
    if (cinfo.out_color_space != JCS_GRAYSCALE)
      return 1;

  jpeg_start_decompress (&cinfo);

  while (cinfo.output_scanline < cinfo.output_height)
    {
      line = dest +
	(GL_RGB == format ? 3 * size : size) * cinfo.output_scanline;
      jpeg_read_scanlines (&cinfo, &line, 1);
    }
  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);
  return 0;
}

// static int		permut[256];
// static const char	gradient[32][4] =
//   {
//     { 1,  1,  1,  0}, { 1,  1,  0,  1}, { 1,  0,  1,  1}, { 0,  1,  1,  1},
//     { 1,  1, -1,  0}, { 1,  1,  0, -1}, { 1,  0,  1, -1}, { 0,  1,  1, -1},
//     { 1, -1,  1,  0}, { 1, -1,  0,  1}, { 1,  0, -1,  1}, { 0,  1, -1,  1},
//     { 1, -1, -1,  0}, { 1, -1,  0, -1}, { 1,  0, -1, -1}, { 0,  1, -1, -1},
//     {-1,  1,  1,  0}, {-1,  1,  0,  1}, {-1,  0,  1,  1}, { 0, -1,  1,  1},
//     {-1,  1, -1,  0}, {-1,  1,  0, -1}, {-1,  0,  1, -1}, { 0, -1,  1, -1},
//     {-1, -1,  1,  0}, {-1, -1,  0,  1}, {-1,  0, -1,  1}, { 0, -1, -1,  1},
//     {-1, -1, -1,  0}, {-1, -1,  0, -1}, {-1,  0, -1, -1}, { 0, -1, -1, -1},
// };


// void		InitNoise (void)
// {
//   unsigned int i = 0;
//   while (i < 256)
//     permut[i++] = rand () & MOD;
// }

// /*
// ** Function finding out the gradient corresponding to the coordinates
// */
// static int	Indice (const int i,
// 			const int j,
// 			const int k,
// 			const int l)
// {
//   return (permut[(l + permut[(k + permut[(j + permut[i & MOD])
// 					 & MOD])
// 			     & MOD])
// 		 & MOD]
// 	  & 0x1f);
// }

// /*
// ** Functions computing the dot product of the vector and the gradient
// */
// static float	Prod (const float a, const char b)
// {
//   if (b > 0)
//     return a;
//   if (b < 0)
//     return -a;
//   return 0;
// }

// static float	Dot_prod (const float x1, const char x2,
// 			  const float y1, const char y2,
// 			  const float z1, const char z2,
// 			  const float t1, const char t2)
// {
//   return (Prod (x1, x2) + Prod (y1, y2) + Prod (z1, z2) + Prod (t1, t2));
// }

// /*
// ** Functions computing interpolations
// */
// static float	Spline5 (const float state)
// {
//   /*
//   ** Enhanced spline :
//   ** (3x^2 + 2x^3) is not as good as (6x^5 - 15x^4 + 10x^3)
//   */
//   const float sqr = state * state;
//   return state * sqr * (6 * sqr - 15 * state + 10);
// }

// static float	Linear (const float start,
// 			const float end,
// 			const float state)
// {
//   return start + (end - start) * state;
// }

// /*
// ** Noise function, returning the Perlin Noise at a given point
// */
// float	Noise (const float x,
// 		       const float y,
// 		       const float z,
// 		       const float t)
// {
//   /* The unit hypercube containing the point */
//   const int x1 = (int) (x > 0 ? x : x - 1);
//   const int y1 = (int) (y > 0 ? y : y - 1);
//   const int z1 = (int) (z > 0 ? z : z - 1);
//   const int t1 = (int) (t > 0 ? t : t - 1);
//   const int x2 = x1 + 1;
//   const int y2 = y1 + 1;
//   const int z2 = z1 + 1;
//   const int t2 = t1 + 1;

//   /* The 16 corresponding gradients */
//   const char * g0000 = gradient[Indice (x1, y1, z1, t1)];
//   const char * g0001 = gradient[Indice (x1, y1, z1, t2)];
//   const char * g0010 = gradient[Indice (x1, y1, z2, t1)];
//   const char * g0011 = gradient[Indice (x1, y1, z2, t2)];
//   const char * g0100 = gradient[Indice (x1, y2, z1, t1)];
//   const char * g0101 = gradient[Indice (x1, y2, z1, t2)];
//   const char * g0110 = gradient[Indice (x1, y2, z2, t1)];
//   const char * g0111 = gradient[Indice (x1, y2, z2, t2)];
//   const char * g1000 = gradient[Indice (x2, y1, z1, t1)];
//   const char * g1001 = gradient[Indice (x2, y1, z1, t2)];
//   const char * g1010 = gradient[Indice (x2, y1, z2, t1)];
//   const char * g1011 = gradient[Indice (x2, y1, z2, t2)];
//   const char * g1100 = gradient[Indice (x2, y2, z1, t1)];
//   const char * g1101 = gradient[Indice (x2, y2, z1, t2)];
//   const char * g1110 = gradient[Indice (x2, y2, z2, t1)];
//   const char * g1111 = gradient[Indice (x2, y2, z2, t2)];

//   /* The 16 vectors */
//   const float dx1 = x - x1;
//   const float dx2 = x - x2;
//   const float dy1 = y - y1;
//   const float dy2 = y - y2;
//   const float dz1 = z - z1;
//   const float dz2 = z - z2;
//   const float dt1 = t - t1;
//   const float dt2 = t - t2;

//   /* The 16 dot products */
//   const float b0000 = Dot_prod(dx1, g0000[0], dy1, g0000[1],
// 			       dz1, g0000[2], dt1, g0000[3]);
//   const float b0001 = Dot_prod(dx1, g0001[0], dy1, g0001[1],
// 			       dz1, g0001[2], dt2, g0001[3]);
//   const float b0010 = Dot_prod(dx1, g0010[0], dy1, g0010[1],
// 			       dz2, g0010[2], dt1, g0010[3]);
//   const float b0011 = Dot_prod(dx1, g0011[0], dy1, g0011[1],
// 			       dz2, g0011[2], dt2, g0011[3]);
//   const float b0100 = Dot_prod(dx1, g0100[0], dy2, g0100[1],
// 			       dz1, g0100[2], dt1, g0100[3]);
//   const float b0101 = Dot_prod(dx1, g0101[0], dy2, g0101[1],
// 			       dz1, g0101[2], dt2, g0101[3]);
//   const float b0110 = Dot_prod(dx1, g0110[0], dy2, g0110[1],
// 			       dz2, g0110[2], dt1, g0110[3]);
//   const float b0111 = Dot_prod(dx1, g0111[0], dy2, g0111[1],
// 			       dz2, g0111[2], dt2, g0111[3]);
//   const float b1000 = Dot_prod(dx2, g1000[0], dy1, g1000[1],
// 			       dz1, g1000[2], dt1, g1000[3]);
//   const float b1001 = Dot_prod(dx2, g1001[0], dy1, g1001[1],
// 			       dz1, g1001[2], dt2, g1001[3]);
//   const float b1010 = Dot_prod(dx2, g1010[0], dy1, g1010[1],
// 			       dz2, g1010[2], dt1, g1010[3]);
//   const float b1011 = Dot_prod(dx2, g1011[0], dy1, g1011[1],
// 			       dz2, g1011[2], dt2, g1011[3]);
//   const float b1100 = Dot_prod(dx2, g1100[0], dy2, g1100[1],
// 			       dz1, g1100[2], dt1, g1100[3]);
//   const float b1101 = Dot_prod(dx2, g1101[0], dy2, g1101[1],
// 			       dz1, g1101[2], dt2, g1101[3]);
//   const float b1110 = Dot_prod(dx2, g1110[0], dy2, g1110[1],
// 			       dz2, g1110[2], dt1, g1110[3]);
//   const float b1111 = Dot_prod(dx2, g1111[0], dy2, g1111[1],
// 			       dz2, g1111[2], dt2, g1111[3]);

//   /* Then the interpolations, down to the result */
//   const float idx1 = Spline5 (dx1);
//   const float idy1 = Spline5 (dy1);
//   const float idz1 = Spline5 (dz1);
//   const float idt1 = Spline5 (dt1);

//   const float b111 = Linear (b1110, b1111, idt1);
//   const float b110 = Linear (b1100, b1101, idt1);
//   const float b101 = Linear (b1010, b1011, idt1);
//   const float b100 = Linear (b1000, b1001, idt1);
//   const float b011 = Linear (b0110, b0111, idt1);
//   const float b010 = Linear (b0100, b0101, idt1);
//   const float b001 = Linear (b0010, b0011, idt1);
//   const float b000 = Linear (b0000, b0001, idt1);

//   const float b11 = Linear (b110, b111, idz1);
//   const float b10 = Linear (b100, b101, idz1);
//   const float b01 = Linear (b010, b011, idz1);
//   const float b00 = Linear (b000, b001, idz1);

//   const float b1 = Linear (b10, b11, idy1);
//   const float b0 = Linear (b00, b01, idy1);

//   return Linear (b0, b1, idx1);
// }


/*
** Function to load a Jpeg file.
*/
// int load_texture (const char * filename,
// 			      unsigned char * dest,
// 			      const int format,
// 			      const unsigned int size)
// {
//   FILE *fd;
//   struct jpeg_decompress_struct cinfo;
//   struct jpeg_error_mgr jerr;
//   unsigned char * line;

//   cinfo.err = jpeg_std_error (&jerr);
//   jpeg_create_decompress (&cinfo);

//   if (0 == (fd = fopen(filename, "rb")))
//     return 1;

//   jpeg_stdio_src (&cinfo, fd);
//   jpeg_read_header (&cinfo, TRUE);
//   if ((cinfo.image_width != size) || (cinfo.image_height != size))
//     return 1;

//   if (GL_RGB == format)
//     {
//       if (cinfo.out_color_space == JCS_GRAYSCALE)
// 	return 1;
//     }
//   else
//     if (cinfo.out_color_space != JCS_GRAYSCALE)
//       return 1;

//   jpeg_start_decompress (&cinfo);

//   while (cinfo.output_scanline < cinfo.output_height)
//     {
//       line = dest +
// 	(GL_RGB == format ? 3 * size : size) * cinfo.output_scanline;
//       jpeg_read_scanlines (&cinfo, &line, 1);
//     }
//   jpeg_finish_decompress (&cinfo);
//   jpeg_destroy_decompress (&cinfo);
//   return 0;
// }

// static float	z (const float x, const float y, const float t)
// {
//   const float x2 = x - 3;
//   const float y2 = y + 1;
//   const float xx = x2 * x2;
//   const float yy = y2 * y2;
//   return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
// 	   Noise (10 * x, 10 * y, t, 0)) / 200);
// }



#define MS_PER_CYCLE 20000
// function prototypes:

void Animate();
void Display();
void DoAxesMenu(int);
void DoColorMenu(int);
void DoDepthBufferMenu(int);
void DoDepthFightingMenu(int);
void DoDepthMenu(int);
// void DoDistort(int);
void DoTexture(int);
void DoDebugMenu(int);
void DoMainMenu(int);
void DoProjectMenu(int);
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Resize(int, int);
void Visibility(int);

void Axes(float);
void HsvRgb(float[3], float[3]);

// main program:

int main(int argc, char *argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)

    glutInit(&argc, argv);

    // setup all the graphics stuff:

    InitGraphics();


    /* Loop */
    // glutMainLoop ();
    
    // create the display structures that will not change:

    // InitLists();
    
    // init all the global variables used by Display( ):
    // this will also post a redisplay

    Reset();

    // setup all the user interface stuff:

    InitMenus();

    // draw the scene once and wait for some interaction:
    // (this will never return)

    glutSetWindow(MainWindow);

    glutMainLoop();

    // this is here to make the compiler happy:

    return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void Animate()
{
    // put animation stuff in here -- change some global variables
    // for Display( ) to find:
    // int ms = glutGet(GLUT_ELAPSED_TIME);
    // ms %= MS_PER_CYCLE;
    // Time = (float)ms / (float)MS_PER_CYCLE; // [0.,1.)

  /* Vertices */
  for (j = 0; j < RESOLUTION; j++)
    {
      y = (j + 1) * delta - 1;
      for (i = 0; i <= RESOLUTION; i++)
	{
	  indice = 6 * (i + j * (RESOLUTION + 1));

	  x = i * delta - 1;
	  surface[indice + 3] = x;
	  surface[indice + 4] = z (x, y, t);
	  surface[indice + 5] = y;
	  if (j != 0)
	    {
	      /* Values were computed during the previous loop */
	      preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
	      surface[indice] = surface[preindice + 3];
	      surface[indice + 1] = surface[preindice + 4];
	      surface[indice + 2] = surface[preindice + 5];
	    }
	  else
	    {
	      surface[indice] = x;
	      surface[indice + 1] = z (x, -1, t);
	      surface[indice + 2] = -1;
	    }
	}
    }

  /* Normals */
  for (j = 0; j < RESOLUTION; j++)
    for (i = 0; i <= RESOLUTION; i++)
      {
	indice = 6 * (i + j * (RESOLUTION + 1));

	v1x = surface[indice + 3];
	v1y = surface[indice + 4];
	v1z = surface[indice + 5];

	v2x = v1x;
	v2y = surface[indice + 1];
	v2z = surface[indice + 2];

	if (i < RESOLUTION)
	  {
	    v3x = surface[indice + 9];
	    v3y = surface[indice + 10];
	    v3z = v1z;
	  }
	else
	  {
	    v3x = xn;
	    v3y = z (xn, v1z, t);
	    v3z = v1z;
	  }

	vax =  v2x - v1x;
	vay =  v2y - v1y;
	vaz =  v2z - v1z;

	vbx = v3x - v1x;
	vby = v3y - v1y;
	vbz = v3z - v1z;

	nx = (vby * vaz) - (vbz * vay);
	ny = (vbz * vax) - (vbx * vaz);
	nz = (vbx * vay) - (vby * vax);

	l = sqrtf (nx * nx + ny * ny + nz * nz);
	if (l != 0)
	  {
	    l = 1 / l;
	    normal[indice + 3] = nx * l;
	    normal[indice + 4] = ny * l;
	    normal[indice + 5] = nz * l;
	  }
	else
	  {
	    normal[indice + 3] = 0;
	    normal[indice + 4] = 1;
	    normal[indice + 5] = 0;
	  }


	if (j != 0)
	  {
	    /* Values were computed during the previous loop */
	    preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
	    normal[indice] = normal[preindice + 3];
	    normal[indice + 1] = normal[preindice + 4];
	    normal[indice + 2] = normal[preindice + 5];
	  }
	else
	  {
/* 	    v1x = v1x; */
	    v1y = z (v1x, (j - 1) * delta - 1, t);
	    v1z = (j - 1) * delta - 1;

/* 	    v3x = v3x; */
	    v3y = z (v3x, v2z, t);
	    v3z = v2z;

	    vax = v1x - v2x;
	    vay = v1y - v2y;
	    vaz = v1z - v2z;

	    vbx = v3x - v2x;
	    vby = v3y - v2y;
	    vbz = v3z - v2z;

	    nx = (vby * vaz) - (vbz * vay);
	    ny = (vbz * vax) - (vbx * vaz);
	    nz = (vbx * vay) - (vby * vax);

	    l = sqrtf (nx * nx + ny * ny + nz * nz);
	    if (l != 0)
	      {
		l = 1 / l;
		normal[indice] = nx * l;
		normal[indice + 1] = ny * l;
		normal[indice + 2] = nz * l;
	      }
	    else
	      {
		normal[indice] = 0;
		normal[indice + 1] = 1;
		normal[indice + 2] = 0;
	      }
	  }
      }
    // force a call to Display( ) next time it is convenient:

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// draw the complete scene:

void Display()
{
    InitNoise ();
    unsigned char total_texture[4 * 256 * 256];
    unsigned char alpha_texture[256 * 256];
    unsigned char caustic_texture[3 * 256 * 256];
    // unsigned int i;

    /* Texture loading  */
    glGenTextures (1, &texture);
    load_texture ("alpha.jpg", alpha_texture, GL_ALPHA, 256);
    load_texture ("reflection.jpg", caustic_texture, GL_RGB, 256);

    for (int i = 0; i < 256 * 256; i++)
        {
        total_texture[4 * i] = caustic_texture[3 * i];
        total_texture[4 * i + 1] = caustic_texture[3 * i + 1];
        total_texture[4 * i + 2] = caustic_texture[3 * i + 2];
        total_texture[4 * i + 3] = alpha_texture[i];
        }
    glBindTexture (GL_TEXTURE_2D, texture);
    gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, 256, 256, GL_RGBA,
                GL_UNSIGNED_BYTE,  total_texture);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glEnable (GL_TEXTURE_GEN_S);
    glEnable (GL_TEXTURE_GEN_T);
    glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity ();
    glTranslatef (0, 0, -translate_z);
    glRotatef (rotate_y, 1, 0, 0);
    glRotatef (rotate_x, 0, 1, 0);

    if (DebugOn != 0)
    {
        fprintf(stderr, "Display\n");
    }

    // set which window we want to do the graphics into:

    glutSetWindow(MainWindow);

    // erase the background:

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (DepthBufferOn != 0)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    // specify shading to be flat:

    glShadeModel(GL_FLAT);

    // set the viewport to a square centered in the window:

    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy; // minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);

    // set the viewing volume:
    // remember that the Z clipping  values are actually
    // given as DISTANCES IN FRONT OF THE EYE
    // USE gluOrtho2D( ) IF YOU ARE DOING 2D !
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO)
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    else
        gluPerspective(90., 1., 0.1, 1000.);

    // place the objects into the scene:

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set the eye position, look-at position, and up-vector:
    gluLookAt(2, 2, 2, 0., 0., 0., 0., 1., 0.);

    // rotate the scene:

    glRotatef((GLfloat)Yrot, 0., 1., 0.);
    glRotatef((GLfloat)Xrot, 1., 0., 0.);

    // uniformly scale the scene:

    if (Scale < MINSCALE)
        Scale = MINSCALE;
    glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

    // set the fog parameters:

    if (DepthCueOn != 0)
    {
        glFogi(GL_FOG_MODE, FOGMODE);
        glFogfv(GL_FOG_COLOR, FOGCOLOR);
        glFogf(GL_FOG_DENSITY, FOGDENSITY);
        glFogf(GL_FOG_START, FOGSTART);
        glFogf(GL_FOG_END, FOGEND);
        glEnable(GL_FOG);
    }
    else
    {
        glDisable(GL_FOG);
    }

    // if we are using a texture on the sphere
    // if (isTexture)
    // {
    //     glEnable(GL_TEXTURE_2D);
    //     glBindTexture(GL_TEXTURE_2D, tex);
    // }
    // else
    // {
    //     // the blob-ish object
    //     glDisable(GL_TEXTURE_2D);
    //     glColor3f(0.5, 0.5, 0.5);
    // }
    // if (Distort)
    // {
    //     glRotatef(360*Time, 0., 1., 0.);
    // }
    // MjbSphere(2, 50, 50);



    glTranslatef (0, 0.2, 0);

    //   if (wire_frame != 0)
    //     glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    /* The water */
    glEnable (GL_TEXTURE_2D);
    glColor3f (1, 1, 1);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_VERTEX_ARRAY);
    glNormalPointer (GL_FLOAT, 0, normal);
    glVertexPointer (3, GL_FLOAT, 0, surface);
    for (int i = 0; i < 64; i++)
        glDrawArrays (GL_TRIANGLE_STRIP, i * length, length);

    glEnd();

    // possibly draw the axes:
    // if (AxesOn != 0)
    // {
    //     glColor3fv(&Colors[WhichColor][0]);
    //     glCallList(AxesList);
    // }

    // since we are using glScalef( ), be sure normals get unitized:

    glEnable(GL_NORMALIZE);

    // draw the current object:
    glCallList(BoxList);

    if (DepthFightingOn != 0)
    {
        glPushMatrix();
        glRotatef(90., 0., 1., 0.);
        glCallList(BoxList);
        glPopMatrix();
    }

    // draw some gratuitous text that just rotates on top of the scene:

    glDisable(GL_DEPTH_TEST);
    glColor3f(1., 1., 1.);
    DoRasterString(0., 1., 0., "");

    // draw some gratuitous text that is fixed on the screen:
    //
    // the projection matrix is reset to define a scene whose
    // world coordinate system goes from 0-100 in each axis
    //
    // this is called "percent units", and is just a convenience
    //
    // the modelview matrix is reset to identity as we don't
    // want to transform these coordinates

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 100., 0., 100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1., 1., 1.);
    DoRasterString(5., 5., 0., "Margaux Masson - Texturing");

    // swap the double-buffered framebuffers:

    glutSwapBuffers();

    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !

    glFlush();
}

void DoAxesMenu(int id)
{
    AxesOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoColorMenu(int id)
{
    WhichColor = id - RED;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDebugMenu(int id)
{
    DebugOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthBufferMenu(int id)
{
    DepthBufferOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthFightingMenu(int id)
{
    DepthFightingOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthMenu(int id)
{
    DepthCueOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}
// void DoDistort(int id)
// {
//     Distort = (bool)id;
// }
void DoTexture(int id)
{
    idTexture = id;
    switch (id)
    {
    case 0:
        Texture = Texture1;
        isTexture = true;
        break;
    case 1:
        Texture = Texture2;
        isTexture = true;
        break;
    case 2:
        Texture = Texture3;
        isTexture = true;
        break;
    case 3:
        isTexture = false;
        break;
    }
}

// main menu callback:

void DoMainMenu(int id)
{
    switch (id)
    {
    case RESET:
        Reset();
        break;

    case QUIT:
        // gracefully close out the graphics:
        // gracefully close the graphics window:
        // gracefully exit the program:
        glutSetWindow(MainWindow);
        glFinish();
        glutDestroyWindow(MainWindow);
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoProjectMenu(int id)
{
    WhichProjection = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void DoRasterString(float x, float y, float z, char *s)
{
    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

// use glut to display a string of characters using a stroke font:

void DoStrokeString(float x, float y, float z, float ht, char *s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

// return the number of seconds since the start of the program:

float ElapsedSeconds()
{
    // get # of milliseconds since the start of the program:

    int ms = glutGet(GLUT_ELAPSED_TIME);

    // convert it to seconds:

    return (float)ms / 1000.f;
}

// initialize the glui window:

void InitMenus()
{
    glutSetWindow(MainWindow);

    int numColors = sizeof(Colors) / (3 * sizeof(int));
    int colormenu = glutCreateMenu(DoColorMenu);
    for (int i = 0; i < numColors; i++)
    {
        glutAddMenuEntry(ColorNames[i], i);
    }

    int axesmenu = glutCreateMenu(DoAxesMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthcuemenu = glutCreateMenu(DoDepthMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int debugmenu = glutCreateMenu(DoDebugMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int projmenu = glutCreateMenu(DoProjectMenu);
    glutAddMenuEntry("Orthographic", ORTHO);
    glutAddMenuEntry("Perspective", PERSP);

    // int distortmenu = glutCreateMenu(DoDistort);
    // glutAddMenuEntry("Off", 0);
    // glutAddMenuEntry("On", 1);

    int texturemenu = glutCreateMenu(DoTexture);
    glutAddMenuEntry("Mercury", 0);
    glutAddMenuEntry("Earth", 1);
    glutAddMenuEntry("Mars", 2);
    glutAddMenuEntry("No texture", 3);

    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Texture", texturemenu);
    // glutAddSubMenu("Distortion", distortmenu);
    glutAddSubMenu("Axes", axesmenu);
    glutAddSubMenu("Colors", colormenu);
    glutAddSubMenu("Depth Buffer", depthbuffermenu);
    glutAddSubMenu("Depth Fighting", depthfightingmenu);
    glutAddSubMenu("Depth Cue", depthcuemenu);
    glutAddSubMenu("Projection", projmenu);
    glutAddMenuEntry("Reset", RESET);
    glutAddSubMenu("Debug", debugmenu);
    glutAddMenuEntry("Quit", QUIT);

    // attach the pop-up menu to the right mouse button:

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void InitGraphics()
{
    // request the display modes:
    // ask for red-green-blue-alpha color, double-buffering, and z-buffering:

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // set the initial window configuration:

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

    // open the window and set its title:

    MainWindow = glutCreateWindow(WINDOWTITLE);
    glutSetWindowTitle(WINDOWTITLE);

    // set the framebuffer clear values:

    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

    // setup the callback functions:
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

    glutSetWindow(MainWindow);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(NULL);
    glutVisibilityFunc(Visibility);
    glutEntryFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpaceballMotionFunc(NULL);
    glutSpaceballRotateFunc(NULL);
    glutSpaceballButtonFunc(NULL);
    glutButtonBoxFunc(NULL);
    glutDialsFunc(NULL);
    glutTabletMotionFunc(NULL);
    glutTabletButtonFunc(NULL);
    glutMenuStateFunc(NULL);
    glutTimerFunc(-1, NULL, 0);
    glutIdleFunc(Animate);

    // init glew (a window must be open to do this):

#ifdef WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "glewInit Error\n");
    }
    else
        fprintf(stderr, "GLEW initialized OK\n");
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void InitLists()
{
    // create the axes:
    // AxesList = glGenLists(1);
    // glNewList(AxesList, GL_COMPILE);
    // glLineWidth(AXES_WIDTH);
    // Axes(1.5);
    // glLineWidth(1.);
    // glEndList();
}

// the keyboard callback:

void Keyboard(unsigned char c, int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

    switch (c)
    {
    case 'o':
    case 'O':
        WhichProjection = ORTHO;
        break;

    case 'p':
    case 'P':
        WhichProjection = PERSP;
        break;
    case 't':
    case 'T':
        if (idTexture < 3)
        {
            DoTexture(idTexture + 1);
        }
        else
        {
            DoTexture(0);
            idTexture = 0;
        }
        break;
    // case 'd':
    // case 'D':
    //     Distort = !Distort;
    //     break;

    case 'q':
    case 'Q':
    case ESCAPE:
        DoMainMenu(QUIT); // will not return here
        break;            // happy compiler

    default:
        fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
    }

    // force a call to Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y)
{
    int b = 0; // LEFT, MIDDLE, or RIGHT

    if (DebugOn != 0)
        fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

    // get the proper button bit mask:

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        b = LEFT;
        break;

    case GLUT_MIDDLE_BUTTON:
        b = MIDDLE;
        break;

    case GLUT_RIGHT_BUTTON:
        b = RIGHT;
        break;

    default:
        b = 0;
        fprintf(stderr, "Unknown mouse button: %d\n", button);
    }

    // button down sets the bit, up clears the bit:

    if (state == GLUT_DOWN)
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b; // set the proper bit
    }
    else
    {
        ActiveButton &= ~b; // clear the proper bit
    }
}

// called when the mouse moves while a button is down:

void MouseMotion(int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "MouseMotion: %d, %d\n", x, y);

    int dx = x - Xmouse; // change in mouse coords
    int dy = y - Ymouse;

    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
    }

    if ((ActiveButton & MIDDLE) != 0)
    {
        Scale += SCLFACT * (float)(dx - dy);

        // keep object from turning inside-out or disappearing:

        if (Scale < MINSCALE)
            Scale = MINSCALE;
    }

    Xmouse = x; // new current position
    Ymouse = y;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void Reset()
{
    ActiveButton = 0;
    AxesOn = 1;
    DebugOn = 0;
    DepthBufferOn = 1;
    DepthFightingOn = 0;
    DepthCueOn = 0;
    Scale = 1.0;
    WhichColor = WHITE;
    WhichProjection = PERSP;
    Xrot = Yrot = 0.;
}

// called when user resizes the window:

void Resize(int width, int height)
{
    if (DebugOn != 0)
        fprintf(stderr, "ReSize: %d, %d\n", width, height);

    // don't really need to do anything since window size is
    // checked each time in Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// handle a change to the window's visibility:

void Visibility(int state)
{
    if (DebugOn != 0)
        fprintf(stderr, "Visibility: %d\n", state);

    if (state == GLUT_VISIBLE)
    {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
    0.f, 1.f, 0.f, 1.f};

static float xy[] = {
    -.5f, .5f, .5f, -.5f};

static int xorder[] = {
    1, 2, -3, 4};

static float yx[] = {
    0.f, 0.f, -.5f, .5f};

static float yy[] = {
    0.f, .6f, 1.f, 1.f};

static int yorder[] = {
    1, 2, 3, -2, 4};

static float zx[] = {
    1.f, 0.f, 1.f, 0.f, .25f, .75f};

static float zy[] = {
    .5f, .5f, -.5f, -.5f, 0.f, 0.f};

static int zorder[] = {
    1, 2, 3, 4, -5, 6};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void Axes(float length)
{
    glBegin(GL_LINE_STRIP);
    glVertex3f(length, 0., 0.);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., length, 0.);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., length);
    glEnd();

    float fact = LENFRAC * length;
    float base = BASEFRAC * length;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++)
    {
        int j = xorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 5; i++)
    {
        int j = yorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++)
    {
        int j = zorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
    }
    glEnd();
}
