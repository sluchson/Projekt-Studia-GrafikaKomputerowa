﻿// Gl_template.c
//Wyłšczanie błędów przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS

// Ładowanie bibliotek:
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif

// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif


#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include <math.h>				// Define for sqrt
#include <stdio.h>

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

// Color Palette handle
HPALETTE hPalette = NULL;

// Application name and instance storeage
static LPCTSTR lpszAppName = "GL Template";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


static GLsizei lastHeight;
static GLsizei lastWidth;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nagłówek obrazu
unsigned char*		bitmapData;			// dane tekstury
unsigned int		texture[5];			// obiekt tekstury



double rot1, rot2, rot3, rot4, rot5, rot6, rot7, rot8, rot9, rot10, rot11, rot12, rot13, rot14, rot15, mkulaX ,mkulaY, mkulaZ, anglekula=0.0f, moveX=0, moveZ=0, observerX, observerY, observerZ;
int licznik = 0, licznikpom = 0;






										// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);


// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 600.0f;
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	// Establish perspective: 
	/*
	gluPerspective(60.0f,fAspect,1.0,400);
	*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
		glEnable(GL_COLOR_MATERIAL);

		// Set Material properties to follow glColor values
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		// All materials hereafter have full specular reflectivity
		// with a high shine
		glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
		glMateriali(GL_FRONT,GL_SHININESS,128);


								// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}

void skrzynka(void)
{
	glColor3d(0.8, 0.7, 0.3);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, 30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(-30, 30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, -30, 30);
	glTexCoord2d(1.0, 0.0); glVertex3d(30, -30, 30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, 30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, -30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(30, -30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(30, 30, -30);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, 30, -30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, -30, -30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, -30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, 30, -30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, -30, -30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, 30, -30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, 30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, -30, -30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-30, 30, -30);
	glTexCoord2d(0.0, 1.0); glVertex3d(-30, -30, -30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, -30, 30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, 30, 30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-30, -30, -30);
	glTexCoord2d(0.0, 1.0); glVertex3d(-30, 30, -30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, 30, 30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, -30, 30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, -30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(-30, -30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(-30, -30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(30, -30, -30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-30, -30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, -30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(30, -30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, -30, -30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(30, -30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, 30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(30, 30, -30);
	glTexCoord2d(1.0, 0.0); glVertex3d(30, -30, -30);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glTexCoord2d(1.0, 1.0); glVertex3d(-30, 30, 30);
	glTexCoord2d(0.0, 1.0); glVertex3d(30, 30, 30);
	glTexCoord2d(0.0, 0.0); glVertex3d(30, -30, 30);
	glTexCoord2d(1.0, 0.0); glVertex3d(-30, -30, 30);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


void walec01(void)
{
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricNormals(obj, GLU_SMOOTH);
	glColor3d(1, 0, 0);
	glPushMatrix();
	gluCylinder(obj, 20, 20, 30, 15, 7);
	gluCylinder(obj, 0, 20, 0, 15, 7);
	glTranslated(0, 0, 60);
	glRotated(180.0, 0, 1, 0);
	gluCylinder(obj, 0, 20, 30, 15, 7);
	glPopMatrix();
}

void kula(double kulaX, double kulaY, double kulaZ)
{

	glTranslated(0+kulaX,18+kulaY,-200+kulaZ);
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricTexture(obj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3d(1.0, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	gluSphere(obj, 15, 15, 7);
	glDisable(GL_TEXTURE_2D);
}

void kula_tex(double a)
{

	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluQuadricTexture(obj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3d(1.0, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	gluSphere(obj, a, 15, 7);
	glDisable(GL_TEXTURE_2D);
}



// LoadBitmapFile
// opis: ładuje mapę bitową z pliku i zwraca jej adres.
//       Wypełnia strukturę nagłówka.
//	 Nie obsługuje map 8-bitowych.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							// wskaźnik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nagłówek pliku
	unsigned char		*bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany składowych

												// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nagłówek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nagłówek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wskaźnik pozycji pliku na początku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pamięć buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy udało się przydzielić pamięć
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zostały wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami składowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wskaźnik bufora zawierającego wczytany obraz
	fclose(filePtr);
	return bitmapImage;
}


void kula_robot(double a) {
	GLUquadricObj* obj;
	obj = gluNewQuadric();
	glColor3d(1, 0, 0);
	gluSphere(obj, a, 15, 7);
}



void ramie(double r1, double r2, double h, double d)
{
	//mała 1/2 walca
	double angle, x, y;
	glColor3d(1, 0, 0);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glVertex3d(0.0f, 0.0f, h);
	for (angle = 0.0f; angle >= -(GL_PI); angle -= (GL_PI / 8.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glVertex3d(x, y, h);
	}
	glEnd();

	//duża 1/2 walca
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glVertex3d(d, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= GL_PI; angle += (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glVertex3d(x+d, y, 0.0);
	}
	glEnd();



	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= GL_PI; angle += (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0);
		glVertex3d(x+d, y, 0);
		glVertex3d(x+d, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glVertex3d(d, 0.0f, h);
	for (angle = -(GL_PI); angle >= -(2.0f * GL_PI); angle -= (GL_PI / 8.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glVertex3d(x+d, y, h);
	}
	glEnd();




	//glEnable(GL_TEXTURE_2D); // Włącz teksturowanie

	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(d, -r2, h);
	glTexCoord2d(0.0, 1.0); glVertex3d(d, r2, h);
	glTexCoord2d(0.0, 0.0); glVertex3d(0, r1, h);
	glTexCoord2d(1.0, 0.0); glVertex3d(0, -r1, h);
	glEnd();
	//glDisable(GL_TEXTURE_2D); // Wyłącz teksturowanie
	

	//Przednia ściana
	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(0, r1, 0);
	glVertex3d(d, r2, 0);
	glVertex3d(d, -r2, 0);
	glVertex3d(0, -r1, 0);

	glEnd();


	//Gorna ściana
	glBegin(GL_QUADS);
	{
		float v[3][3] = {
			{d, r2, h},
			{d, r2, 0},
			{0, r1, 0}
		};
		float norm[3];
		calcNormal(v, norm);
		glNormal3d(norm[0], norm[1], norm[2]);

		glVertex3d(d, r2, h);
		glVertex3d(d, r2, 0);
		glVertex3d(0, r1, 0);
		glVertex3d(0, r1, h);
	}
	glEnd();
	
	//Dolna ściana
	glBegin(GL_QUADS);
	{
		float v[3][3] = {
			{d, -r2, h},
			{0, -r1, h},
			{0, -r1, 0}
		};
		float norm[3];
		calcNormal(v, norm);
		glNormal3d(norm[0], norm[1], norm[2]);

		glVertex3d(d, -r2, h);
		glVertex3d(0, -r1, h);
		glVertex3d(0, -r1, 0);
		glVertex3d(d, -r2, 0);

	}
	glEnd();
	
}

void walec(double r, double h)
{
	double angle, x, y;
	
	glColor3d(0.5, 1, 0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glColor3d(0, 1, 0);
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();
	glColor3d(0.5, 1, 1);
	glBegin(GL_TRIANGLE_FAN);
	//glVertex3d(0.0f, 0.0f, 0.0f);
	glVertex3d(0.0f, 0.0f, h);
	//for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	for (angle = 0.0f; angle >= -(2.0f * GL_PI); angle -= (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		//glVertex3d(x, y, 0.0);
		glVertex3d(x, y, h);
	}
	glEnd();
}

void walec_light(double r, double h)
{
	double angle, x, y;
	glColor3d(0.7, 0.7, 0.7);
	glBegin(GL_TRIANGLE_FAN);
	//Oświetlenie przedniej sciany
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		//Oświetlenie kazdego ze stripów
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	//Oświetlenie tylniej ściany
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0f, 0.0f, h);
	for (angle = 0.0f; angle >= -(2.0f * GL_PI); angle -= (GL_PI / 20.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		//glVertex3d(x, y, 0.0);
		glVertex3d(x, y, h);
	}
	glEnd();
}

void stozek(double promien, double wysokosc, int segmenty) {
	double kat;
	double x, y;

	// Wierzchołek stożka
	double wierzcholek_x = 0.0;
	double wierzcholek_y = wysokosc;
	double wierzcholek_z = 0.0;

	// Rysowanie podstawy stożka
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0.5, 0);  // Kolor podstawy
	glVertex3d(0.0, 0.0, 0.0);  // Środek podstawy
	for (int i = 0; i <= segmenty; i++) {
		kat = 2.0 * GL_PI * i / segmenty;
		x = promien * cos(kat);
		y = promien * sin(kat);
		glVertex3d(x, 0.0, y);
	}
	glEnd();

	// Rysowanie ścian stożka
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(1, 0.0, 0);  // Kolor ścian bocznych
	glVertex3d(wierzcholek_x, wierzcholek_y, wierzcholek_z);  // Wierzchołek stożka
	for (int i = 0; i <= segmenty; i++) {
		kat = 2 * GL_PI * i / segmenty;
		x = promien * cos(kat);
		y = promien * sin(kat);
		glVertex3d(y, 0.0, x);
	}
	glEnd();
}

void ostroslup(double wysokosc) {
	glBegin(GL_QUADS);
	glColor3d(1, 0.5, 0);
		glVertex3d(-20, -20, 20);
		glVertex3d(-20, -20, -20);
		glVertex3d(20, -20, -20);
		glVertex3d(20, -20, 20);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3d(1, 0, 0);
		glVertex3d(20, -20, 20);
		glVertex3d(0, wysokosc, 0);   // Wierzchołek ostrosłupa
		glVertex3d(-20, -20, 20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 1, 0);
	glVertex3d(20, -20, -20);
	glVertex3d(0, wysokosc, 0);   // Wierzchołek ostrosłupa
	glVertex3d(20, -20, 20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(0.5, 0.5, 1);
	glVertex3d(-20, -20, 20);
	glVertex3d(0, wysokosc, 0);   // Wierzchołek ostrosłupa
	glVertex3d(-20, -20, -20);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0.5, 1);
	glVertex3d(-20, -20, -20);
	glVertex3d(0, wysokosc, 0);  // Wierzchołek ostrosłupa
	glVertex3d(20, -20, -20);
	glEnd();



}

void szescian(void)
{
	
	glBegin(GL_QUADS);
	glColor3d(1, 0.5, 0);
		glVertex3d(20, 20, 20);
		glVertex3d(-20, 20, 20);
		glVertex3d(-20, -20, 20);
		glVertex3d(20, -20, 20);
	glColor3d(0, 0.5, 1);
		glVertex3d(20, 20, 20);
		glVertex3d(20, -20, 20);
		glVertex3d(20, -20, -20);
		glVertex3d(20, 20, -20);
	glColor3d(0, 1, 0);
		glVertex3d(-20, 20, 20);
		glVertex3d(20, 20, 20);
		glVertex3d(20, 20, -20);
		glVertex3d(-20, 20, -20);
	glColor3d(1, 0.5, 1);
		glVertex3d(-20, -20, 20);
		glVertex3d(-20, -20, -20);
		glVertex3d(20, -20, -20);
		glVertex3d(20, -20, 20);
	glColor3d(1, 0, 0);
		glVertex3d(-20, 20, -20);
		glVertex3d(-20, -20, -20);
		glVertex3d(-20, -20, 20);
		glVertex3d(-20, 20, 20);
	glColor3d(1, 1, 0);
		glVertex3d(-20, 20, -20);
		glVertex3d(20, 20, -20);
		glVertex3d(20, -20, -20);
		glVertex3d(-20, -20, -20);

	glEnd();
}

void graniastoslup(double a, double b, double h) {

	glColor3d(0.972, 0.96, 0.915);
	// Dolna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0.0, 0, -1);
	glVertex3d(a, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(a, b, 0);
	glEnd();

	// Przednia ściana
	glBegin(GL_QUADS);
	glNormal3d(0, -1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, 0, h);
	glVertex3d(0, 0, h);
	glEnd();

	// Lewa ściana
	glBegin(GL_QUADS);
	glNormal3d(-1, 0, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, h);
	glVertex3d(0, b, h);
	glEnd();

	// Tylna ściana
	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	glVertex3d(a, b, 0);
	glVertex3d(0, b, 0);
	glVertex3d(0, b, h);
	glVertex3d(a, b, h);
	glEnd();

	// Prawa ściana
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glVertex3d(a, 0, 0);
	glVertex3d(a, b, 0);
	glVertex3d(a, b, h);
	glVertex3d(a, 0, h);
	glEnd();

	// Górna podstawa
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex3d(0, 0, h);
	glVertex3d(a, 0, h);
	glVertex3d(a, b, h);
	glVertex3d(0, b, h);
	glEnd();
}

void robot(double d1, double d2, double d3, double d4) {
	glPushMatrix();
	
	glRotated(-90, 1, 0, 0);
	glRotated(d4, 0, 0, 1);
	glTranslated(0, 0, -50);
	walec_light(30, 5); //podstawa

	glTranslated(0, 0, 5);
	walec_light(10, 40);//stojak1

	glTranslated(0, 0, 40);
	glRotated(d1, 0, 0, 1);
	walec_light(10, 40);//stojak2

	glTranslated(0, 0, 40);
	glRotated(90, 0, 1, 0);
	glTranslated(0, 0, -20);
	walec_light(10, 40);//poprzeczny

	
	glTranslated(0, 0, +40);
	glRotated(90 + d2, 0, 0, 1);
	ramie(15, 10, 5, 30);

	glTranslated(30, 0, -5);
	glRotated(d3, 0, 0, 1);
	ramie(15, 10, 5, 30);

	glPopMatrix();
}

void tasma(void) {


	glColor3d(1, 0, 1);
	glTranslated(50, -50, -450);
	graniastoslup(15, 25, 500);

	glTranslated(100, 0, 0);
	graniastoslup(15, 25, 500);


	glColor3d(1, 1, 1); // Ustawienie koloru na biały dla poprawnego wyświetlenia tekstury
	glTranslated(-45, 18, 249);

	glPushMatrix();

	// Nakładanie tekstury
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]); // Ustawienie tekstury
	// Rozrysowanie ścian graniastosłupa z teksturą
	glBegin(GL_QUADS);

	// Przednia ściana
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.0, 1.0); glVertex3d(-50, -5, 250);
	glTexCoord2d(1.0, 1.0); glVertex3d(50, -5, 250);
	glTexCoord2d(1.0, 0.0); glVertex3d(50, 5, 250);
	glTexCoord2d(0.0, 0.0); glVertex3d(-50, 5, 250);

	// Tylna ściana
	glNormal3d(0, 0, -1);
	glTexCoord2d(0.0, 1.0); glVertex3d(-50, -5, -249);
	glTexCoord2d(0.0, 0.0); glVertex3d(-50, 5, -249);
	glTexCoord2d(1.0, 0.0); glVertex3d(50, 5, -249);
	glTexCoord2d(1.0, 1.0); glVertex3d(50, -5, -249);

	// Prawa ściana
	glNormal3d(1, 0, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(50, -5, 250);
	glTexCoord2d(1.0, 1.0); glVertex3d(50, -5, -249);
	glTexCoord2d(1.0, 0.0); glVertex3d(50, 5, -249);
	glTexCoord2d(0.0, 0.0); glVertex3d(50, 5, 250);

	// Lewa ściana
	glNormal3d(-1, 0, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-50, -5, 250);
	glTexCoord2d(1.0, 1.0); glVertex3d(-50, -5, -249);
	glTexCoord2d(1.0, 0.0); glVertex3d(-50, 5, -249);
	glTexCoord2d(0.0, 0.0); glVertex3d(-50, 5, 250);

	// Dolna ściana
	glNormal3d(0, -1, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-50, -5, 250);
	glTexCoord2d(0.0, 0.0); glVertex3d(-50, -5, -249);
	glTexCoord2d(1.0, 0.0); glVertex3d(50, -5, -249);
	glTexCoord2d(1.0, 1.0); glVertex3d(50, -5, 250);

	
	

	// Górna ściana
	glNormal3d(0, 1, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-50, 5, 250);
	glTexCoord2d(1.0, 1.0); glVertex3d(50, 5, 250);
	glTexCoord2d(1.0, 0.0); glVertex3d(50, 5, -249);
	glTexCoord2d(0.0, 0.0); glVertex3d(-50, 5, -249);

	glEnd();

	glDisable(GL_TEXTURE_2D); // Wyłączenie tekstur
	glPopMatrix();
}


void robot_projekt(double d1, double d2, double d3, double d4, double d5) {
	glPushMatrix();

	glTranslated(-100, 0, -200);

	glRotated(-90, 1, 0, 0);
	glRotated(d1, 0, 0, 1); 
	glTranslated(0, 0, -50);
	walec_light(15, 10); // podstawa

	glTranslated(0, 0, 5);
	walec_light(10, 50); // stojak1

	glRotated(d2, 0, 0, 1);  // Używamy rot1
	glTranslated(0, 0, 50);
	kula_tex(12); // staw1

	glRotated(90, 0, 1, 0);
	walec_light(10, 25); // odnoga1

	glRotated(d3, 0, 0, 1);  // Używamy rot2
	glTranslated(0, 0, 25);
	kula_tex(12); // staw2

	glRotated(-90, 0, 1, 0);
	walec_light(10, 50); // stojak2

	glTranslated(0, 0, 50);
	glRotated(d4, 0, 0, 1);  // Używamy rot3
	kula_tex(12); // staw3

	glRotated(-90, 0, 1, 0);
	walec_light(10, 25); // odnoga2

	glTranslated(0, 0, 25);
	glRotated(d5, 0, 0, 1);  // Używamy rot4
	kula_tex(12); // staw4

	glRotated(90, 0, 1, 0);
	walec_light(10, 50); // stojak3

	glTranslated(0, 0, 50);
	kula_tex(25); //głowa


	glPopMatrix();
}

void robot_projekt2(double d1, double d2, double d3, double d4, double d5, double moveX, double moveZ) {
	glPushMatrix();

	glTranslated(moveX, -50 , moveZ);
	glRotated(-90, 1, 0, 0);
	glRotated(d1, 0, 0, 1); 
	walec_light(25, 20); // podstawa

	glTranslated(0, 0, 20);
	kula_robot(15); 

	
	glRotated(d2, 0, 1, 0);
	glRotated(-35, 0, 1, 0);
	walec_light(10, 75); 

	
	
	glTranslated(0, 0, 75);
	kula_robot(12);

	glRotated(d3, 0, 1, 0);
	glRotated(90, 0, 1, 0);
	walec_light(10, 60);

	
	glTranslated(0, 0, 60);
	glRotated(60+d4, 0, 1, 0);
	kula_robot(12); 
	
	walec_light(10, 45);
	glTranslated(0,0,45);
	kula_robot(11);

	
	glRotated(90, 1, 0, 0);
	glRotated(-90, 0, 0, 1);
	glTranslated(0,0,-3);

	
	glPushMatrix();
	glTranslated(0, 0, 6);          
	glRotated(180, 0, 1, 0);        
	glRotated(60, 0, 0, 1);        
	glRotated(d5, 0, 0, 1); 
	ramie(7, 4, 5, 20); 
	glPopMatrix();

	
	glPushMatrix();
	glTranslated(0, 0, 6);          
	glRotated(180, 0, 1, 0);        
	glRotated(-60, 0, 0, 1);        
	glRotated(-d5, 0, 0, 1); 
	ramie(7, 4, 5, 20);             
	glPopMatrix();



	glPopMatrix();
}


void podloga() {


	// Nakładanie tekstury
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[4]); // Ustawienie tekstury

	


	// Rozrysowanie ścian graniastosłupa z teksturą
	glBegin(GL_QUADS);
	// Przednia ściana
	glNormal3d(0, 0, 1);
	glTexCoord2d(0.0, 1.0); glVertex3d(-400, -5, 400);
	glTexCoord2d(1.0, 1.0); glVertex3d(400, -5, 400);
	glTexCoord2d(1.0, 0.0); glVertex3d(400, 5, 400);
	glTexCoord2d(0.0, 0.0); glVertex3d(-400, 5, 400);

	// Tylna ściana
	glNormal3d(0, 0, -1);
	glTexCoord2d(0.0, 1.0); glVertex3d(-400, -5, -400);
	glTexCoord2d(0.0, 0.0); glVertex3d(-400, 5, -400);
	glTexCoord2d(1.0, 0.0); glVertex3d(400, 5, -400);
	glTexCoord2d(1.0, 1.0); glVertex3d(400, -5, -400);

	// Prawa ściana
	glNormal3d(1, 0, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(400, -5, 400);
	glTexCoord2d(1.0, 1.0); glVertex3d(400, -5, -400);
	glTexCoord2d(1.0, 0.0); glVertex3d(400, 5, -400);
	glTexCoord2d(0.0, 0.0); glVertex3d(400, 5, 400);

	// Lewa ściana
	glNormal3d(-1, 0, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-400, -5, 400);
	glTexCoord2d(0.0, 0.0); glVertex3d(-400, 5, 400);
	glTexCoord2d(1.0, 0.0); glVertex3d(-400, 5, -400);
	glTexCoord2d(1.0, 1.0); glVertex3d(-400, -5, -400);

	// Dolna ściana
	glNormal3d(0, -1, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-400, -5, 400);
	glTexCoord2d(0.0, 0.0); glVertex3d(-400, -5, -400);
	glTexCoord2d(1.0, 0.0); glVertex3d(400, -5, -400);
	glTexCoord2d(1.0, 1.0); glVertex3d(400, -5, 400);

	// Górna ściana
	glNormal3d(0, 1, 0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-400, 5, 400);
	glTexCoord2d(1.0, 1.0); glVertex3d(400, 5, 400);
	glTexCoord2d(1.0, 0.0); glVertex3d(400, 5, -400);
	glTexCoord2d(0.0, 0.0); glVertex3d(-400, 5, -400);


	glEnd();

	glDisable(GL_TEXTURE_2D); // Wyłączenie tekstur


}

	


void dwa_roboty() {
	
	robot(rot1, rot2, rot3,rot4);
	glTranslated(60, 0, 0);
	robot(rot1, rot2, rot3, rot4);
}

void trail() {
	glPushMatrix();
	glTranslated(-295, -10, 200);
	glRotated(90,1,0,0);
	walec_light(20, 10);

	glRotated(-90,1,0,0);
	glTranslated(0, -5, -400);
	walec_light(5, 395);

	glRotated(90, 0, 1, 0);
	walec_light(5, 200);

	glTranslated(0, -5, 190);
	glRotated(-90, 1, 0, 0);
	walec_light(20, 10);


	glPopMatrix();
}

void all(void) {

	glTranslated(0, -50, 0);
	podloga();
	glTranslated(0, 50, 0);


	robot_projekt(rot11, rot12, rot13, rot14, rot15);

	glTranslated(-190, 0, 0);
	robot_projekt2(rot6, rot7, rot8, rot9, rot10, moveX, moveZ);
	glTranslated(190, 0, 0);


	robot_projekt2(rot1, rot2, rot3, rot4, rot5, 0, 0);
	tasma();

	glTranslated(-200, 17.5, 200);
	skrzynka();
	glTranslated(200, -17.5, -200);

	glPushMatrix();
	kula(mkulaX, mkulaY, mkulaZ);
	glPopMatrix();

	trail();
}
// Called to draw scene
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	/////////////////////////////////////////////////////////////////
	// MIEJSCE NA KOD OPENGL DO TWORZENIA WLASNYCH SCEN:		   //
	/////////////////////////////////////////////////////////////////

	//Sposób na odróźnienie "przedniej" i "tylniej" ściany wielokąta:
	// KOM LAB4  
	glPolygonMode(GL_BACK, GL_LINE);

	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//Wyrysowanie prostokata:
	//glRectd(-10.0, -10.0, 20.0, 20.0);
	

	//robot(rot1, rot2, rot3,rot4);

	//kula_robot();

	//tasma();

	//dwa_roboty();
	//walec01();

	//Wyrysowanie szescianu
	//szescian();

	//Wyrysownaie walca
	//walec(30, 50);

	//Wyrysownie oświetlonego walca
	//walec_light(30, 50);
	
	//Wyrysowanie ramienia
	//yramie(10, 30, 20, 60);

	//kula();

	//ostroslup(100);

	//WEJSCIOWKA
	///graniastoslup(10,10,10);

	//stozek(20, 40, 40);
	glTranslatef(observerX, observerY, observerZ);


	

	glTranslated(0, -50, -200);
	podloga();
	glTranslated(0, 50, 200);

	
	robot_projekt(rot11, rot12, rot13, rot14, rot15);
	
	glTranslated(-190, 0, 0);
	robot_projekt2(rot6, rot7, rot8, rot9, rot10, moveX, moveZ);
	glTranslated(190, 0, 0);

	
	robot_projekt2(rot1, rot2, rot3, rot4, rot5,0,0);
	tasma();

	glTranslated(-200, 17.5, 200);
	skrzynka();
	glTranslated(200, -17.5, -200);

	glPushMatrix();
	kula(mkulaX, mkulaY, mkulaZ);
	glPopMatrix();

	trail();
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	


	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

												// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}


// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)

	
	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

								   // Build mask of all 1's.  This creates a number represented by having
								   // the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
								   // pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = NULL;
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		400, 400,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}




// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	switch (message)
	{
	case WM_TIMER:
		if (wParam == 101)
		{
			//Ruch kuli na tasmie
			if (licznik == 0) {
				if (mkulaZ < 400) {
					mkulaZ += 10.0;
				}

				// Sprawdzenie zakończenia ruchu kuli
				if (mkulaZ >= 400) {
					licznik = 1; // Przejdź do następnej akcji 
				}
			}

			// Podniesienie kuli
			if (licznik == 1) {
				BOOL allFinished = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot2 < 52) {
					rot2 += 2.0;
					allFinished = FALSE; // rot2 jeszcze w ruchu
				}
				if (rot3 > -29) {
					rot3 -= 2.0;
					allFinished = FALSE; // rot3 jeszcze w ruchu
				}
				if (rot4 < 22) {
					rot4 += 2.0;
					allFinished = FALSE; // rot4 jeszcze w ruchu
				}

				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished == TRUE) {
					licznik = 2; // Przejdź do rot5
				}
			}

			// Zacisniecie szczypiec
			if (licznik == 2) {
				if (rot5 > -16) {
					rot5 -= 2.0;
				}

				// Sprawdzenie zakończenia akcji 
				if (rot5 <= -16) {
					licznik = 3; // Oznacz zakończenie wszystkich akcji
				}
			}

			// Podnoszenie pilki 1
			if (licznik == 3) {
				BOOL allFinished1 = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot2 >= 40) {
					rot2 -= 2.0;
					mkulaX -= 0.4;
					mkulaY += 5.9;
					allFinished1 = FALSE; // rot2 jeszcze w ruchu
				}
				if (rot3 >= -40) {
					rot3 -= 2.0;
					mkulaY += 1.7;
					allFinished1 = FALSE; // rot3 jeszcze w ruchu
				}


				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished1 == TRUE) {
					licznik = 4;
				}
			}

			//Obrót w strone skrzynki
			if (licznik == 4) {
				if (rot1 > -180) {

					mkulaX -= 8.98 * sin(anglekula);
					mkulaZ += 8.98 * cos(anglekula);
					anglekula += (GL_PI / 35);
					rot1 -= 5.0;
				}

				if (anglekula >= GL_PI) {
					licznik = 5; // Przejdź do następnej akcji
				}
			}

			//Otworzenie szczypiec
			if (licznik == 5) {
				if (rot5 < 0) {
					rot5 += 2.0;
				}
				// Sprawdzenie zakończenia akcji rot5
				if (rot5 >= 0) {
					licznik = 6; // Oznacz zakończenie wszystkich akcji
				}
			}

			if (licznik == 6) {
				if (mkulaY > 0) {
					mkulaY -= 5.0;
				}
				// Sprawdzenie czy kula spadla
				if (mkulaY <= 0) {
					licznik = 7;
				}
			}

			if (licznik == 7) {
				if (mkulaX < -192) {
					mkulaX += 2.0;
					mkulaZ -= 1.4;
				}
				// Korekta kuli w skrzynce
				if (mkulaX >= -192) {
					licznik = 8;
				}
			}

			//Obrot do bazowej pozycji
			if (licznik == 8) {
				if (rot1 < 0) {
					rot1 += 5.0;
				}

				if (rot1 >= 0) {
					licznik = 9; // Przejdź do następnej akcji
				}
			}

			//Powrot do bazowej pozycji
			if (licznik == 9) {
				BOOL allFinished3 = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot2 > 0) {
					rot2 -= 2.0;
					allFinished3 = FALSE; // rot2 jeszcze w ruchu
				}
				if (rot3 < 0) {
					rot3 += 2.0;
					allFinished3 = FALSE; // rot3 jeszcze w ruchu
				}
				if (rot4 > 0) {
					rot4 -= 2.0;
					allFinished3 = FALSE; // rot4 jeszcze w ruchu
				}

				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished3 == TRUE) {
					licznik = 10; // Przejdź do rot5
				}
			}


			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			// Schylanie sie
			if (licznik == 10) {
				BOOL allFinished4 = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot7 < 52) {
					rot7 += 2.0;
					allFinished4 = FALSE; // rot2 jeszcze w ruchu
				}
				if (rot8 > -29) {
					rot8 -= 2.0;
					allFinished4 = FALSE; // rot3 jeszcze w ruchu
				}
				if (rot9 < 22) {
					rot9 += 2.0;
					allFinished4 = FALSE; // rot4 jeszcze w ruchu
				}

				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished4 == TRUE) {
					licznik = 11; // Przejdź do rot5
				}
			}

			// Zacisniecie szczypiec
			if (licznik == 11) {
				if (rot10 > -16) {
					rot10 -= 2.0;
				}

				// Sprawdzenie zakończenia akcji rot5
				if (rot10 <= -16) {
					licznik = 12; // Oznacz zakończenie wszystkich akcji
				}
			}

			// Podnoszenie pilki 2
			if (licznik == 12) {
				BOOL allFinished5 = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot7 >= 40) {
					rot7 -= 2.0;
					mkulaX -= 0.4;
					mkulaY += 5.9;
					allFinished5 = FALSE; // rot7 jeszcze w ruchu
				}
				if (rot8 >= -40) {
					rot8 -= 2.0;
					mkulaY += 1.7;
					allFinished5 = FALSE; // rot8 jeszcze w ruchu
				}

				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished5 == TRUE) {
					licznik = 13; //
				}
			}

			//Otworzenie szczypiec
			if (licznik == 14) {
				if (rot10 < 0) {
					rot10 += 2.0;
				}
				// Sprawdzenie zakończenia akcji rot10
				if (rot10 >= 0) {
					licznik = 15;
				}
			}

			if (licznik == 15) {
				if (mkulaY > 0) {
					mkulaY -= 5.0;
				}
				// Sprawdzenie czy kula spadla
				if (mkulaY <= 0) {
					licznik = 16;
				}
			}

			if (licznik == 16) {
				if (mkulaX < 0) {
					mkulaX += 2.0;
					mkulaZ -= 2.5;
				}
				// Korekta kuli na tasmie
				if (mkulaX >= 0) {
					licznik = 17;
				}
			}

			//Powrot do bazowej pozycji
			if (licznik == 18) {
				BOOL allFinished3 = TRUE; // Zakładamy, że wszystkie ruchy zakończone

				if (rot7 > 0) {
					rot7 -= 2.0;
					allFinished3 = FALSE; // rot2 jeszcze w ruchu
				}
				if (rot8 < 0) {
					rot8 += 2.0;
					allFinished3 = FALSE; // rot3 jeszcze w ruchu
				}
				if (rot9 > 0) {
					rot9 -= 2.0;
					allFinished3 = FALSE; // rot4 jeszcze w ruchu
				}

				// Jeśli wszystkie ruchy zakończone, przejdź do następnej akcji
				if (allFinished3 == TRUE) {
					licznik = 0; // Przejdź do rot5
				}
			}


			///////////////////////////////////////////////////////////////////////////////
			//Ruch maskotki
			if (licznikpom == 0) {
				if (rot11 >= -180) {
					rot11 -= 20;
					rot12 += 5;
					rot13 -= 5;
					rot14 += 5;
					rot15 -= 5;
				}
				if (rot11 == -180)
					licznikpom = 1;
			}

			if (licznikpom == 1) {
				if (rot11 <= 180) {
					rot11 += 20;
					rot12 -= 5;
					rot13 += 5;
					rot14 -= 5;
					rot15 += 5;
				}
				if (rot11 == 180)
					licznikpom = 0;
			}

		}
			InvalidateRect(hWnd, NULL, FALSE);


	
		break;

	case WM_CREATE:


		//Ustawinenie timera
		SetTimer(hWnd, 101, 200, NULL);

		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(5, &texture[0]);                  // tworzy obiekt tekstury			



		// ładuje pierwszy obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\checker.bmp", &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ładuje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\crate.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);


		// ładuje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\ROCK.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[2]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);
		

		// ładuje czwarty obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\WATER.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[3]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);


		if (bitmapData)
			free(bitmapData);

		// ładuje piaty obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\GRASS.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[4]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);


		if (bitmapData)
			free(bitmapData);

	

		// ustalenie sposobu mieszania tekstury z tłem
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:

		//Usuwanie timera
		KillTimer(hWnd, 101);

		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;

		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.
	case WM_KEYDOWN:
	{
		if (wParam == VK_DOWN)
			xRot -= 5.0f;

		if (wParam == VK_UP)
			xRot += 5.0f;

		if (wParam == VK_LEFT)
			yRot -= 5.0f;

		if (wParam == VK_RIGHT)
			yRot += 5.0f;

		xRot = (const int)xRot % 360;
		yRot = (const int)yRot % 360;


		
		if (wParam == 'W')
		{
			observerZ += 10;	
		}
		if (wParam == 'A')
		{
			observerX += 10;	
		}
		if (wParam == 'D')
		{
			observerX -= 10;
		}
		if (wParam == 'S')
		{
			observerZ -= 10;
		}
		if (wParam == 'Q')
		{
			observerY += 10;
		}
		if (wParam == 'E')
		{
			observerY -= 10;
		}


		if (licznik == 13) {

			if (wParam == '1') {
				if (moveX < 190 && moveZ == -395) {
					moveX += 5.0f;
					mkulaX += 5.0f;
				}
			}

			if (wParam == '3') {
				if (moveX > 0 && moveZ == -395) {
					moveX -= 5.0f;
					mkulaX -= 5.0f;
				}
			}
			

			if (wParam == '2') {
				if (moveZ < 0 && moveX == 0) {
					moveZ += 5.0f;
					mkulaZ += 5.0f;
				}
			}
			
			if (wParam == '4') {
				if (-395 < moveZ && moveX == 0){
					moveZ -= 5.0f;
					mkulaZ -= 5.0f;
				}
			}
			if (wParam == VK_SPACE) {
				if (moveZ == -395 && moveX == 190) {
					anglekula = 0.0f;
					licznik = 14;
				}
			}
		}


		if (licznik == 17) {
	
			if (wParam == '1') {
				if (moveX < 190 && moveZ == -395) {
					moveX += 5.0f;
				}
			}

			if (wParam == '3') {
				if (moveX > 0 && moveZ == -395) {
					moveX -= 5.0f;
				}
			}


			if (wParam == '2') {
				if (moveZ < 0 && moveX == 0) {
					moveZ += 5.0f;
				}
			}

			if (wParam == '4') {
				if (-395 < moveZ && moveX == 0) {
					moveZ -= 5.0f;
				}
			}
			if (wParam == VK_SPACE) {
				if (moveZ == 0 && moveX == 0) {
					licznik = 18;
				}
			}
		}


		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;

	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}
