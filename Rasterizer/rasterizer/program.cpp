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
float g_pZBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

// Static variables
//
static GLdouble g_dZoomFactor = 1.0;
static GLint g_iHeight;


void drawLine(int x0, int y0, int xEnd, int yEnd);
void drawPixel(int x, int y);

void drawLineColor(int x0, int y0, float z0, int xEnd, int yEnd,float zEnd );
void drawPixelColor(int x, int y,int z);

inline int myRound(const float a) { return int(a + 0.5); }

void initilize();//초기화작업 = input assembler. 지금은 obj파일 읽어서 저장하는것만 집어넣음
void finalize();//종료작업. 동적할당 삭제처리 여기서 하면 될것
void ScreenBufferSet();
float fMaxZ;
float fMinZ;

objLoader *objData;
CVertexShader VertexShader;
CTessellator Tessellator;
CRasterizer Rasterizer;
CPixelShader PixelShader;
int g_cnt;


// TODO: g_pScreenImage 메모리를 채우면 됩니다.
void makeCheckImage(void)
{
	//printf("adadf\n");
	memset(g_pScreenImage, 255, sizeof (g_pScreenImage));
	
	Update_Camera(g_lpCamera3D);
	// printf("camera : %f %f %f\n", g_lpCamera3D->pos.x, g_lpCamera3D->pos.y, g_lpCamera3D->pos.z);
	//_POINT3D tempcam;

	int colour=100;
	Vertex* pVertex_Converted = VertexShader.Launch(objData);
	
//	Triangular_Face* pFace_Converted = Tessellator.Launch(objData->faceList);
	//objData->vertexList

	//?? = Rasterizer.Launch(pVertex_Converted , pFace_Converted);
	//래스터라이저의 리턴값을 뭘로 줘야할지 몰라서 일단은 void로 놓음
	
//	PixelShader.Launch();
	//마찬가지로 input/output몰라서 일단 void로 처리


	//ScreenBufferSet();//이 함수에서 g_pScreenImage배열에 값을 집어넣는 처리를 하면될듯함
	fMaxZ = VertexShader.m_fMaxZ;
	fMinZ = VertexShader.m_fMinZ;


	//backface culling
	int cnt = 0;
	for (int i = 0; i < objData->faceCount; i++)
	{
		int a = objData->faceList[i]->vertex_index[0];
		int b = objData->faceList[i]->vertex_index[1];
		int c = objData->faceList[i]->vertex_index[2];
		
		
		
		_POINT3D p1, p2, p3,N;
		p1.x = VertexShader.m_pPoints[a].x;
		p1.y = VertexShader.m_pPoints[a].y;
		p1.z = VertexShader.m_pPoints[a].z;

		p2.x = VertexShader.m_pPoints[b].x;
		p2.y = VertexShader.m_pPoints[b].y;
		p2.z = VertexShader.m_pPoints[b].z;

		p3.x = VertexShader.m_pPoints[c].x;
		p3.y = VertexShader.m_pPoints[c].y;
		p3.z = VertexShader.m_pPoints[c].z;

		
		_POINT3D v1, v2;
		v1.x = p2.x - p1.x;
		v1.y = p2.y - p1.y;
		v1.z = p2.z - p1.z;
		v1.w = 1;

		v2.x = p3.x - p1.x;
		v2.y = p3.y - p1.y;
		v2.z = p3.z - p1.z;
		v2.w = 1;

		Cross_Vector3D(N, v1, v2);
		_POINT3D S, avg;
		
		//g_lpCamera3D->
		//Sub_Vector3D(S, g_lpCamera3D->pos, p1);
		
		//float check = N.x*p1.x + N.y*p1.y + N.z*p1.z;

		if (N.z > 0){
			objData->faceList[i]->bIsBackface = true;
			cnt++;
		}
		else objData->faceList[i]->bIsBackface = false;

	}
	g_cnt = cnt;
	//z buffering & rasterize
	for (int i = 0; i < 640; i++)
	{
		for (int j = 0; j < 480; j++)
			g_pZBuffer[j][i] = FLT_MAX;
	}
	//memset(g_pZBuffer, FLT_MAX, sizeof (g_pZBuffer));

//	printf("%f\n", fMaxZ);
//	printf("is %d\n", objData->faceCount);
	for (int i = 0; i < objData->faceCount; i++)
	{
	//	printf("at %d\n", i);
		if (objData->faceList[i]->bIsBackface) continue;//후면이면 안해도될것

		int a = objData->faceList[i]->vertex_index[0];
		int b = objData->faceList[i]->vertex_index[1];
		int c = objData->faceList[i]->vertex_index[2];



		_POINT3D p1, p2, p3, N;
		p1.x = VertexShader.m_pPoints[a].x;
		p1.y = VertexShader.m_pPoints[a].y;
		p1.z = VertexShader.m_pPoints[a].z;

		p2.x = VertexShader.m_pPoints[b].x;
		p2.y = VertexShader.m_pPoints[b].y;
		p2.z = VertexShader.m_pPoints[b].z;

		p3.x = VertexShader.m_pPoints[c].x;
		p3.y = VertexShader.m_pPoints[c].y;
		p3.z = VertexShader.m_pPoints[c].z;

		float maxX, minX, maxY, minY;

		maxX = max(p1.x, p2.x);
		maxX = max(maxX, p3.x);
		if (maxX > 640) maxX = 638;

		maxY = max(p1.y, p2.y);
		maxY = max(maxY, p3.y);
		if (maxY > 480) maxY = 478;

		minX = min(p1.x, p2.x);
		minX = min(minX, p3.x);
		if (minX < 0) minX = 1;

		minY = min(p1.y, p2.y);
		minY = min(minY, p3.y);

		if (minY < 0)minY = 1;

		//drawLine(p1.x, p1.y, p2.x, p2.y);
		//drawLine(p2.x, p2.y, p3.x, p3.y);
		//drawLine(p3.x, p3.y, p1.x, p1.y);
		
		//법선벡터
		_POINT3D v1, v2;
		v1.x = p2.x - p1.x;
		v1.y = p2.y - p1.y;
		//z값 계산 필요
		v1.z = p2.z - p1.z;
		v1.w = 1;

		v2.x = p3.x - p1.x;
		v2.y = p3.y - p1.y;
		v2.z = p3.z - p1.z;
		v2.w = 1;

		Cross_Vector3D(N, v1, v2);

		
		
		//lighting value 계산
		//편의상 백색 물체에 백색광이라고 함. 
		_POINT3D Centroid;
		Centroid.x = (p1.x + p2.x + p3.x) / 3;
		Centroid.y = (p1.y + p2.y + p3.y) / 3;
		Centroid.z = (p1.z + p2.z + p3.z) / 3;

		_POINT3D LightVec,L;
		Sub_Vector3D(LightVec, VertexShader.m_lighting, Centroid);
		float size = Length_Vector3D(LightVec);
		L.x = LightVec.x / size;
		L.y = LightVec.y / size;
		L.z = LightVec.z / size;
		float dist = sqrt((VertexShader.m_lighting.x - Centroid.x)*(VertexShader.m_lighting.x - Centroid.x)
			+ (VertexShader.m_lighting.y - Centroid.y)*(VertexShader.m_lighting.y - Centroid.y)
			+ (VertexShader.m_lighting.z - Centroid.z)*(VertexShader.m_lighting.z - Centroid.z)
			);


		float ka = 0.8;
		float kd = 0.021;

		int ia = 200;
		int id = 155;

		float ar = ka*ia;

		float dr = kd*id*
			(N.x*L.x +
			N.y*L.y +
			N.z*L.z);

		float light = ar + dr / (0 + 0.03 * dist + 0 * dist*dist);
		int colour = round(light);
		if (colour > 255) colour = 255;
		else if (colour < 0) colour = 0;


		for (int y = round(minY - 1); y < round(maxY + 1); y++)
		{
			for (int x = round(minX - 1); x < round(maxX + 1); x++)
			{
				float aa, bb, cc;
				aa = (p1.y - p2.y)*x + (p2.x - p1.x)*y + p1.x*p2.y - p2.x*p1.y;
				bb = (p2.y - p3.y)*x + (p3.x - p2.x)*y + p2.x*p3.y - p3.x*p2.y;
				cc = (p3.y - p1.y)*x + (p1.x - p3.x)*y + p3.x*p1.y - p1.x*p3.y;

				if (aa <  0 && bb  <   0 && cc  < 0)
				{

					float dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
					if (dd < g_pZBuffer[y][x])
					{
						//lighting
						
						drawPixelColor(x, y, colour);
						g_pZBuffer[y][x] = dd;
					}
					

				}
			
				

			}
		}
		
	}
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
	
	objData = new objLoader();
	objData->load("teapot.obj");
	m_ptMouse.x = 320;
	m_ptMouse.y = 240;
	
	
}


void finalize()
{
	/*
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
	*/
	delete objData;

}

void drawLineColor(int x0, int y0, float z0, int xEnd, int yEnd, float zEnd)
{
	int dx = xEnd - x0, dy = yEnd - y0, steps, k;
	float dz = zEnd - z0;

	float xIncrement, yIncrement, x = x0, y = y0;
	float zIncrement, z = z0;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else
		steps = abs(dy);

	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);
	zIncrement = float(dz) / float(steps);


	drawPixelColor(round(x), round(y),z);
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		z += zIncrement;
		drawPixelColor(round(x), round(y),z);
	}

}
void drawPixelColor(int x, int y, int z)
{
	if (0 <= x && x < SCREEN_WIDTH && 0 <= y&&y < SCREEN_HEIGHT)
	{
		//float hello = z;
		
		//hello = (hello / objData->faceCount) * 255;

		byte colour = (byte)z;
		g_pScreenImage[y][x][0] = colour;
		g_pScreenImage[y][x][1] = colour;
		g_pScreenImage[y][x][2] = colour;
	}

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
	if (0 <= x && x < SCREEN_WIDTH && 0 <= y&&y < SCREEN_HEIGHT)
	{
		g_pScreenImage[y][x][0] = 0;
		g_pScreenImage[y][x][1] = 0;
		g_pScreenImage[y][x][2] = 0;
	}
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
	//printf( "[motion] x: %d, y: %d\n", x, y );


	Update_Mouse(x, y, g_lpCamera3D);

	
	/*
	static GLint screeny;

	screeny = g_iHeight -( GLint) y;
	glRasterPos2i( x, screeny );
	//glPixelZoom( g_dZoomFactor, g_dZoomFactor );
	glCopyPixels( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR );
	glPixelZoom( 1.0, 1.0 );*/
	display();
	//glFlush();
//	printf("motin end\n");


}

void keyboard( unsigned char key, int x, int y)
{
	//printf( "[keyboard] key: %c\n", key );
	
	HMODULE hCurrentModule = nullptr;
	_POINT3D v;
	int i;
	float angleX = 0;
	float angleY = 0;
	float theta = 0;
	float slice = 0.02f;
	DWORD dwStartTime;
	DWORD dwEndTime;
	
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
	case 'a':
		
		Cross_Vector3D(v, g_lpCamera3D->target, g_lpCamera3D->v);
		Add_Vector3D(g_lpCamera3D->pos, g_lpCamera3D->pos, v);
		//g_lpCamera3D->pos.x -= 1.0f;
		break;
	case 'd':
		//g_lpCamera3D->pos.x += 1.0f;
		
		Cross_Vector3D(v, g_lpCamera3D->target, g_lpCamera3D->v);
		Sub_Vector3D(g_lpCamera3D->pos, g_lpCamera3D->pos, v);
		break;
	case 'w':
		Add_Vector3D(g_lpCamera3D->pos, g_lpCamera3D->pos, g_lpCamera3D->target);
		break;
	case 's':
		Sub_Vector3D(g_lpCamera3D->pos, g_lpCamera3D->pos, g_lpCamera3D->target);
		break;
	case 'L':
		g_lpCamera3D->dir.x = 0.0f;
		g_lpCamera3D->dir.y = 0.0f;
		g_lpCamera3D->dir.z = 0.0f;
		g_lpCamera3D->target.x = 0.0f;
		g_lpCamera3D->target.y = 0.0f;
		g_lpCamera3D->target.z = 1.0f;
		break;
	case '1':
		printf("Camera rotation start!!\n");
		dwStartTime = timeGetTime();
		for (i = 0; i < (2 * 3.141592) / slice; i++)
		{
			
			g_lpCamera3D->dir.y += slice;
			if (g_lpCamera3D->dir.y >= 2 * 3.141592)
			{
				g_lpCamera3D->dir.y -= 2 * 3.141592;
			}
			else if (g_lpCamera3D->dir.y <= -2 * 3.141592)
			{
				g_lpCamera3D->dir.y += 2 * 3.141592;
			}
			//g_lpCamera3D->dir.x += 0.05;
			if (g_lpCamera3D->dir.x >= 2 * 3.141592)
			{
				g_lpCamera3D->dir.x -= 2 * 3.141592;
			}
			else if (g_lpCamera3D->dir.x <= -2 * 3.141592)
			{
				g_lpCamera3D->dir.x += 2 * 3.141592;
			}
			g_lpCamera3D->target.x = cos(g_lpCamera3D->dir.x)*sin(g_lpCamera3D->dir.y);
			g_lpCamera3D->target.z = cos(g_lpCamera3D->dir.x)*cos(g_lpCamera3D->dir.y);
			g_lpCamera3D->target.y = sin(-g_lpCamera3D->dir.x);
			theta += slice;
			g_lpCamera3D->pos.x = -130*sin(theta);
			g_lpCamera3D->pos.z = -130*cos(theta);

			display();
		}
		dwEndTime = timeGetTime();

		printf("elapsed time : %d ms", dwEndTime - dwStartTime);
		break;

	default:
		break;
		
	}
	display();
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

