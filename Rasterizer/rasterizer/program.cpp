// Include Directive
// 


/****************************************************************




OBJLoader : example.txt
global.h : 전역변수, 구조체정의






*******************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <memory.h>
#include <math.h>

#include "glut.h"

#include <iostream>

#include "Global.h"
#include "VertexShader.h"
#include "Tessellator.h"
#include "Rasterizer.h"
#include "PixelShader.h"

// Constants Directive
//
#define	SCREEN_WIDTH 640
#define	SCREEN_HEIGHT 480
#define COLOR_DEPTH 3


// Global variables
//
typedef unsigned char byte;
byte g_pScreenImage[SCREEN_HEIGHT][SCREEN_WIDTH][COLOR_DEPTH];


// Static variables
//
static GLdouble g_dZoomFactor = 1.0;
static GLint g_iHeight;


void drawLine(int x0, int y0, int xEnd, int yEnd);
void drawPixel(int x, int y);

inline int myRound(const float a) { return int(a + 0.5); }

void initilize();//초기화작업 = input assembler. 지금은 obj파일 읽어서 저장하는것만 집어넣음
void finalize();//종료작업. 동적할당 삭제처리 여기서 하면 될것
void ScreenBufferSet();


objLoader *objData;
CVertexShader VertexShader;
CTessellator Tessellator;
CRasterizer Rasterizer;
CPixelShader PixelShader;



// TODO: g_pScreenImage 메모리를 채우면 됩니다.
void makeCheckImage(void)
{
	memset(g_pScreenImage, 100, sizeof (g_pScreenImage));
	
	//Pipeline

	Vertex* pVertex_Converted = VertexShader.Launch(g_pVertex);
	Triangular_Face* pFace_Converted = Tessellator.Launch(objData->faceList);
	
	//?? = Rasterizer.Launch(pVertex_Converted , pFace_Converted);
	//래스터라이저의 리턴값을 뭘로 줘야할지 몰라서 일단은 void로 놓음
	
	PixelShader.Launch();
	//마찬가지로 input/output몰라서 일단 void로 처리


	ScreenBufferSet();//이 함수에서 g_pScreenImage배열에 값을 집어넣는 처리를 하면될듯함


}


void ScreenBufferSet()
{
	//참조용. 지금구조에서는 작동안함
	/*
	Triangular_Face* temp2 = g_pTriangular_Face->next;
	while (1)
	{
		if (temp2->next == NULL) break;
		int a, b, c;
		a = temp2->a;
		b = temp2->b;
		c = temp2->c;

		temp2 = temp2->next;

		Vertex* aa;
		Vertex* bb;
		Vertex* cc;
		aa = getMeshByIndex(a);
		bb = getMeshByIndex(b);
		cc = getMeshByIndex(c);

		drawLine(aa->x, aa->y, bb->x, bb->y);
		drawLine(bb->x, bb->y, cc->x, cc->y);
		drawLine(cc->x, cc->y, aa->x, aa->y);


	}
	*/
}



void initilize()
{
	//set dummy head
	g_pVertex = new Vertex;
	g_pVertex->x = 0;
	g_pVertex->y = 0;
	g_pVertex->z = 0;
	g_pVertex->next = NULL;

	g_pTriangular_Face = new Triangular_Face;
	g_pTriangular_Face->a = 0;
	g_pTriangular_Face->b = 0;
	g_pTriangular_Face->c = 0;
	g_pTriangular_Face->next = NULL;

	objData = new objLoader();
	objData->load("cube.obj");
	Vertex* temp = g_pVertex;

	for (int i = 0; i<objData->vertexCount; i++)
	{
		obj_vector *o = objData->vertexList[i];
		temp->next = new Vertex;
		temp = temp->next;
		temp->x = o->e[0];
		temp->y = o->e[1];
		temp->z = o->e[2];
		temp->next = NULL;

	}
	
	
}


void finalize()
{

	Vertex* now = g_pVertex;
	Vertex* prev;
	while (1)
	{
		if (now == NULL) break;
		prev = now;
		now = now->next;
		delete prev;
	}

	Triangular_Face* now2 = g_pTriangular_Face;
	Triangular_Face* prev2;
	while (1)
	{
		if (now2 == NULL) break;
		prev2 = now2;
		now2 = now2->next;
		delete prev2;
	}

	delete objData;

}


void drawLine(int x0, int y0, int xEnd,int yEnd)
{
	int dx = xEnd - x0, dy = yEnd - y0, steps, k;
	float xIncrement, yIncrement, x = x0, y = y0;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else
		steps = abs(dy);

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);
	
	drawPixel(round(x), round(y));
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		drawPixel(round(x), round(y));
	}
			
}

void drawPixel(int x, int y)
{
	if (0 <= x && x <= SCREEN_WIDTH && 0 <= y&&y <= SCREEN_HEIGHT)
		g_pScreenImage[y][x][0] = 255;
}


//-----------------------------------------------------------------------------------------------------------------------
//
// DO NOT EDIT!!
//
//-----------------------------------------------------------------------------------------------------------------------
void init( void)
{	
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel( GL_FLAT );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}

void display( void)
{
	glClear( GL_COLOR_BUFFER_BIT );

	makeCheckImage();
	glRasterPos2i( 0, 480 );
	glPixelZoom( 1.f, -1.f );
	glDrawPixels( SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, g_pScreenImage );
	glFlush();
}

void reshape( int w, int h)
{
	glViewport( 0, 0,( GLsizei) w,( GLsizei) h );
	g_iHeight =( GLint) h;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0,( GLdouble) w, 0.0,( GLdouble) h );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void motion( int x, int y)
{
	printf( "[motion] x: %d, y: %d\n", x, y );


		

	
	/*
	static GLint screeny;

	screeny = g_iHeight -( GLint) y;
	glRasterPos2i( x, screeny );
	//glPixelZoom( g_dZoomFactor, g_dZoomFactor );
	glCopyPixels( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR );
	glPixelZoom( 1.0, 1.0 );*/
	display();
	glFlush();
	


}

void keyboard( unsigned char key, int x, int y)
{
	printf( "[keyboard] key: %c\n", key );
	
	HMODULE hCurrentModule = nullptr;

	switch( key )
	{	

	case 'r':
	case 'R':
		g_dZoomFactor = 1.0;
		glutPostRedisplay();
		printf( "g_dZoomFactor reset to 1.0\n" );

		break;

	case 'z':
		g_dZoomFactor += 0.5;
		if ( g_dZoomFactor >= 5.0) 
		{
			g_dZoomFactor = 5.0;
		}
		printf( "g_dZoomFactor is now %4.1f\n", g_dZoomFactor );
		break;

	case 'Z':
		g_dZoomFactor -= 0.5;
		if ( g_dZoomFactor <= 0.5) 
		{
			g_dZoomFactor = 0.5;
		}
		printf( "g_dZoomFactor is now %4.1f\n", g_dZoomFactor );
		break;

	case 27:
		finalize();
		exit( 0 );
		break;

	default:
		break;
	}
}

int main( int argc, char** argv)
{
	initilize();

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( argv[0] );
	init();
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutMotionFunc( motion );
	glutMainLoop();
	return 0; 
}
