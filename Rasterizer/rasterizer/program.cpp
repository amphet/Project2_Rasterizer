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
#include "EdgeTable.h"

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
void Benchmark();
void PipelineCheck();

//float fMaxZ;
//float fMinZ;

bool bIsScanline;
bool bIsWireFrame;


CVertexShader VertexShader;
CTessellator Tessellator;
CRasterizer Rasterizer;
CPixelShader PixelShader;
CEdgeTable EdgeTable(&VertexShader);

int g_cnt;
//
//typedef struct __EdgeEntry
//{
//	float ymax;
//	float xmin;
//	float incr;
//	__EdgeEntry *pNext;
//} EdgeEntry_NEW;
//
//typedef struct __EdgeEntryWrapper
//{
//	float fScanLineNum;
//	EdgeEntry_NEW *entry;
//}EdgeEntryWrapper_NEW;

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
//	fMaxZ = VertexShader.m_fMaxZ;
	//fMinZ = VertexShader.m_fMinZ;

	int cnt = 0;


	_POINT3D p1, p2, p3, N;
	_POINT3D v1, v2;
	_POINT3D Centroid,L;

	//EdgeEntryWrapper_NEW EdgeTable[2];
//	EdgeEntry_NEW pEdgeEntry1, pEdgeEntry2, pEdgeEntry3;

	float aa, bb, cc, dd;
	int a, b, c;

	float fMinInterX, fMaxInterX;

	
	for (int i = 0; i < 480; i++)
	{
		for (int j = 0; j < 640; j++)
			g_pZBuffer[i][j] = FLT_MAX;
	}
	
	//if (VertexShader.m_bIsMultiThread == false)
	{
		for (int i = 0; i < objData->faceCount; i++)
		{
			a = objData->faceList[i]->vertex_index[0];
			b = objData->faceList[i]->vertex_index[1];
			c = objData->faceList[i]->vertex_index[2];




			p1.x = VertexShader.m_pPoints[a].x;
			p1.y = VertexShader.m_pPoints[a].y;
			p1.z = VertexShader.m_pPoints[a].z;

			p2.x = VertexShader.m_pPoints[b].x;
			p2.y = VertexShader.m_pPoints[b].y;
			p2.z = VertexShader.m_pPoints[b].z;

			p3.x = VertexShader.m_pPoints[c].x;
			p3.y = VertexShader.m_pPoints[c].y;
			p3.z = VertexShader.m_pPoints[c].z;


			
			v1.x = p2.x - p1.x;
			v1.y = p2.y - p1.y;
			v1.z = p2.z - p1.z;
			v1.w = 1;

			v2.x = p3.x - p1.x;
			v2.y = p3.y - p1.y;
			v2.z = p3.z - p1.z;
			v2.w = 1;

			Cross_Vector3D(N, v1, v2);
			//_POINT3D S, avg;

			//g_lpCamera3D->
			//Sub_Vector3D(S, g_lpCamera3D->pos, p1);

			//float check = N.x*p1.x + N.y*p1.y + N.z*p1.z;
			VertexShader.m_pNormVec[i].x = N.x;
			VertexShader.m_pNormVec[i].y = N.y;
			VertexShader.m_pNormVec[i].z = N.z;
			VertexShader.m_pNormVec[i].w = N.w;
			Normalize_Vector3D(VertexShader.m_pNormVec[i]);

			//hidden face culling
			if (N.z > 0){
				objData->faceList[i]->bIsBackface = true;

				cnt++;
			}
			else objData->faceList[i]->bIsBackface = false;

			if (objData->faceList[i]->bIsBackface) continue;//후면이면 안해도될것

			float maxX, minX, maxY, minY;

			maxX = max(p1.x, p2.x);
			maxX = max(maxX, p3.x);
			//if (maxX > 640) maxX = 638;

			maxY = max(p1.y, p2.y);
			maxY = max(maxY, p3.y);
			//if (maxY > 480) maxY = 478;

			minX = min(p1.x, p2.x);
			minX = min(minX, p3.x);
		//	if (minX < 0) minX = 1;

			minY = min(p1.y, p2.y);
			minY = min(minY, p3.y);

			

			if (bIsWireFrame)
			{
				drawLine(p1.x, p1.y, p2.x, p2.y);
				drawLine(p2.x, p2.y, p3.x, p3.y);
				drawLine(p3.x, p3.y, p1.x, p1.y);
				continue;
			}
			

			//lighting value 계산
			//조명색상 수정은 drawpixelcolor()
			N.x = VertexShader.m_pNormVec[i].x;
			N.y = VertexShader.m_pNormVec[i].y;
			N.z = VertexShader.m_pNormVec[i].z;
			N.w = VertexShader.m_pNormVec[i].w;

			
			Centroid.x = (p1.x + p2.x + p3.x) / 3;
			Centroid.y = (p1.y + p2.y + p3.y) / 3;
			Centroid.z = (p1.z + p2.z + p3.z) / 3;

			Sub_Vector3D(L, VertexShader.m_lighting, Centroid);
			Normalize_Vector3D(L);

			float dist = sqrt((VertexShader.m_lighting.x - Centroid.x)*(VertexShader.m_lighting.x - Centroid.x)
				+ (VertexShader.m_lighting.y - Centroid.y)*(VertexShader.m_lighting.y - Centroid.y)
				+ (VertexShader.m_lighting.z - Centroid.z)*(VertexShader.m_lighting.z - Centroid.z)
				);

			//teapot(0.45,0.9)
			float ka = 0.45;
			float kd = 0.9;

			int ia = 255;
			int id = 255;

			float ar = ka*ia;

			float dr = kd*id*(N.x*L.x + N.y*L.y + N.z*L.z);


			float light = (ar + dr) / (0 + 0.003*dist);
			int colour = myRound(light);
			if (colour > 255) colour = 255;
			else if (colour < 0) colour = 0;



			////////////////////////
			//rasterize + zbuffer
			if (!bIsScanline)
			{
				bool hit = false;
				char mode = 0;


				for (int y = myRound(minY) - 1; y < myRound(maxY) + 1; y++)
				{

					hit = false;
					if (y < 0) continue;
					if (y > SCREEN_HEIGHT) break;
					for (int x = myRound(minX) - 1; x < myRound(maxX) + 1; x++)
					{

						aa = (p1.y - p2.y)*x + (p2.x - p1.x)*y + p1.x*p2.y - p2.x*p1.y;
						bb = (p2.y - p3.y)*x + (p3.x - p2.x)*y + p2.x*p3.y - p3.x*p2.y;
						cc = (p3.y - p1.y)*x + (p1.x - p3.x)*y + p3.x*p1.y - p1.x*p3.y;

						if (aa <  0 && bb  <   0 && cc  < 0)
						{
							if (x < 0 || SCREEN_WIDTH <= x || y < 0 || SCREEN_HEIGHT <= y) continue;
							hit = true;
							dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
							if (dd < g_pZBuffer[y][x])
							{
								//아래 주석 해제하면 pixel단위 lighting
								/*

								//lighting
								_POINT3D L,NOW;
								Init_Vector3D(NOW, x, y, dd);

								Sub_Vector3D(L, VertexShader.m_lighting, NOW);
								Normalize_Vector3D(L);

								float dist = sqrt((VertexShader.m_lighting.x - NOW.x)*(VertexShader.m_lighting.x - NOW.x)
								+ (VertexShader.m_lighting.y - NOW.y)*(VertexShader.m_lighting.y - NOW.y)
								+ (VertexShader.m_lighting.z - NOW.z)*(VertexShader.m_lighting.z - NOW.z)
								);


								float ka = 0.5;
								float kd = 0.9;

								int ia = 255;
								int id = 255;

								float ar = ka*ia;

								float dr = kd*id*(N.x*L.x + N.y*L.y + N.z*L.z);
								float light = (ar + dr) / (0 + 0.0027*dist + 0 * dist*dist);
								int colour = myRound(light);
								if (colour > 255) colour = 255;
								else if (colour < 0) colour = 0;
								*/


								//ㅡㅡ
								drawPixelColor(x, y, colour);
								g_pZBuffer[y][x] = dd;
							}


						}
						else
						{
							if (hit)
								break;
						}



					}
				}
			}
			else
			{
				//scanline FILL ALgorithm
				//EdgeEntryWrapper_NEW EdgeTable[2];
				float ____minY;
				
				/*


				//edgeEntry from p1 to p2
				pEdgeEntry1.ymax = max(p1.y, p2.y);
				if (p1.y == p2.y)//horizontal line
				{
					pEdgeEntry1.pNext = (EdgeEntry_NEW*)-1;
				}
				else//non-horizontal
				{
					pEdgeEntry1.pNext = NULL;
					pEdgeEntry1.incr = (p1.x - p2.x) / (p1.y - p2.y);
					____minY = min(p1.y, p2.y);
					if (p1.y == ____minY) pEdgeEntry1.xmin = p1.x;
					else pEdgeEntry1.xmin = p2.x;					
				}
				
				//edgeEntry from p2 to p3
				pEdgeEntry2.ymax = max(p2.y, p3.y);
				if (p2.y == p3.y)//horizontal line
				{
					pEdgeEntry2.pNext = (EdgeEntry_NEW*)-1;
				}
				else//non-horizontal
				{
					pEdgeEntry2.pNext = NULL;
					pEdgeEntry2.incr = (p2.x - p3.x) / (p2.y - p3.y);
					____minY = min(p2.y, p3.y);
					if (p2.y == ____minY) pEdgeEntry2.xmin = p2.x;
					else pEdgeEntry2.xmin = p3.x;
				}

				//edgeEntry from p3 to p1
				pEdgeEntry3.ymax = max(p1.y, p3.y);
				if (p1.y == p3.y)//horizontal line
				{
					pEdgeEntry3.pNext = (EdgeEntry_NEW*)-1;
				}
				else//non-horizontal
				{
					pEdgeEntry3.pNext = NULL;
					pEdgeEntry3.incr = (p1.x - p3.x) / (p1.y - p3.y);
					____minY = min(p1.y, p3.y);
					if (p1.y == ____minY) pEdgeEntry3.xmin = p1.x;
					else pEdgeEntry3.xmin = p3.x;
				}
				*/
				// do something;




				
				// EdgeTable[]
				///////////
				
				EdgeTable.Initialize(i);

				if (EdgeTable.m_EdgeTable.empty())
				{
					printf("????????????????\n");
					continue;
				}
				float bucket[3] = { 0, 0, 0 };
				char cnt = 0;
				for (std::list<EdgeIndex>::iterator pos = EdgeTable.m_EdgeTable.begin(); pos != EdgeTable.m_EdgeTable.end(); pos++)
				{
					bucket[cnt] = pos->y;
					cnt++;
				}
				//printf("%f %f %f\n", bucket[0], bucket[1], bucket[2]);

				float k1, k2, k3,k4;
				
				//z값 보간
				//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;
				//-> dd = k1*x + k2+y + k3으로
				k1 = -(N.x / N.z);
				k2 = -(N.y / N.z);
				k3 = (N.x*p3.x+ N.y*p3.y) / N.z + p3.z;

				float from, to;

				if (bucket[1] == 0)
				{
					for (std::list<EdgeEntryWrapper>::iterator pos = EdgeTable.m_EdgeTable.front().Entry.begin(); pos != EdgeTable.m_EdgeTable.front().Entry.end(); pos++)
					{
						EdgeEntryWrapper ITEM;
						ITEM.pEntry = pos->pEntry;

						EdgeTable.m_MergeList.push_back(ITEM);
					}


					EdgeEntry* Left;
					EdgeEntry* Right;
					EdgeEntry* tmp;
					Left = EdgeTable.m_MergeList.front().pEntry;
					Right = EdgeTable.m_MergeList.back().pEntry;
					if (Left->xmin > Right->xmin)
					{
						tmp = Left;
						Left = Right;
						Right = tmp;
					}

					int xx, yy;
					float dd;

					///clipping??
					//if (maxY > SCREEN_HEIGHT) maxY = SCREEN_HEIGHT;

					///////
					
					k4 = k2*bucket[0] + k3;

					for (float y = bucket[0]; y < maxY; y++)
					{
						if (y < 0)
						{
							k4 += k2;
							continue;
						}
						if (y >= SCREEN_HEIGHT) break;
					
						if (Left->xmin - 1 < 0) from = 0;
						else from = Left->xmin - 1;
						if (SCREEN_WIDTH <= Right->xmin + 1) to = SCREEN_WIDTH;
						else to = Right->xmin + 1;
						
						//yy = myRound(y);
						//k4 = k2*yy + k3;
						dd = k1*from + k4;
						yy = myRound(y);
						for (float x = from ; x < to ; x++)
						{
							//if (x < 0) continue;
							//if (x >= SCREEN_WIDTH) break;
							xx = myRound(x);
							
							//dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
							//dd = -(N.x/N.z)*x + N.x*p3.x/N.z - (N.y/N.z)*y + N.y*p3.y/N.z + p3.z;
							//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;
							
							

							

						//	if (xx < 0 || SCREEN_WIDTH <= xx || yy < 0 || SCREEN_HEIGHT <= yy) continue;
							if (dd < g_pZBuffer[yy][xx])
							{
								drawPixelColor(xx, yy, colour);
								g_pZBuffer[yy][xx] = dd;
							}
							dd += k1;

						}
						Left->xmin = Left->xmin + Left->incr;
						Right->xmin = Right->xmin + Right->incr;

						if (Left->xmin > Right->xmin)
						{
							tmp = Left;
							Left = Right;
							Right = tmp;
						}
						k4 += k2;
					}

				}
				else
				{
					for (std::list<EdgeEntryWrapper>::iterator pos = EdgeTable.m_EdgeTable.front().Entry.begin(); pos != EdgeTable.m_EdgeTable.front().Entry.end(); pos++)
					{
						EdgeEntryWrapper ITEM;
						ITEM.pEntry = pos->pEntry;

						EdgeTable.m_MergeList.push_back(ITEM);
					}


					EdgeEntry* Left;
					EdgeEntry* Right;
					EdgeEntry* tmp;
					Left = EdgeTable.m_MergeList.front().pEntry;
					Right = EdgeTable.m_MergeList.back().pEntry;
					if (Left->xmin > Right->xmin)
					{
						tmp = Left;
						Left = Right;
						Right = tmp;
					}

					int xx, yy;
					float dd;

					k4 = k2*bucket[0] + k3;
					for (float y = bucket[0]; y < bucket[1]; y++)
					{
						if (y < 0)
						{
							k4 += k2;
							continue;
						}
						if (y >= SCREEN_HEIGHT) break;
						
						
						if (Left->xmin - 1 < 0) from = 0;
						else from = Left->xmin - 1;
						if (SCREEN_WIDTH <= Right->xmin + 1) to = SCREEN_WIDTH;
						else to = Right->xmin + 1;


						yy = myRound(y);
						//k4 = k2*yy + k3;
						dd = k1*from + k4;
						for (float x = from; x < to; x++)
						{
							//if (x < 0) continue;
							//if (x >= SCREEN_WIDTH) break;

							xx = myRound(x);
			
							//dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
							//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;
							


							


							//if (xx < 0 || SCREEN_WIDTH <= xx || yy < 0 || SCREEN_HEIGHT <= yy) continue;
							if (dd < g_pZBuffer[yy][xx])
							{
								drawPixelColor(xx, yy, colour);
								g_pZBuffer[yy][xx] = dd;
							}
							dd += k1;
						}
						Left->xmin = Left->xmin + Left->incr;
						Right->xmin = Right->xmin + Right->incr;

						if (Left->xmin > Right->xmin)
						{
							tmp = Left;
							Left = Right;
							Right = tmp;
						}
						k4 += k2;
					}

					Left = EdgeTable.m_MergeList.front().pEntry;
					Right = EdgeTable.m_MergeList.back().pEntry;
					if (Left->ymax == bucket[1]) EdgeTable.m_MergeList.pop_front();
					if (Right->ymax == bucket[1]) EdgeTable.m_MergeList.pop_back();

					for (std::list<EdgeEntryWrapper>::iterator pos = EdgeTable.m_EdgeTable.back().Entry.begin(); pos != EdgeTable.m_EdgeTable.back().Entry.end(); pos++)
					{
						EdgeEntryWrapper ITEM;
						ITEM.pEntry = pos->pEntry;

						EdgeTable.m_MergeList.push_back(ITEM);
					}

					Left = EdgeTable.m_MergeList.front().pEntry;
					Right = EdgeTable.m_MergeList.back().pEntry;
					if (Left->xmin > Right->xmin)
					{
						tmp = Left;
						Left = Right;
						Right = tmp;
					}

					k4 = k2*bucket[1] + k3;
					for (float y = bucket[1]; y < maxY; y++)
					{
						if (y < 0)
						{
							k4 += k2;
							continue;
						}
						if (y >= SCREEN_HEIGHT) break;


						if (Left->xmin - 1 < 0) from = 0;
						else from = Left->xmin - 1;
						if (SCREEN_WIDTH <= Right->xmin + 1) to = SCREEN_WIDTH;
						else to = Right->xmin + 1;

						yy = myRound(y);
						//k4 = k2*yy + k3;
						dd = k1*from + k4;
						for (float x = from ; x < to ; x++)
						{
							if (x < 0) continue;
							if (x >= SCREEN_WIDTH) break;

							xx = myRound(x);
							

							//dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
							//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;
							
							//if (xx < 0 || SCREEN_WIDTH <= xx || yy < 0 || SCREEN_HEIGHT <= yy) continue;
							if (dd < g_pZBuffer[yy][xx])
							{
								drawPixelColor(xx, yy, colour);
								g_pZBuffer[yy][xx] = dd;
							}
							dd += k1;
						}
						Left->xmin = Left->xmin + Left->incr;
						Right->xmin = Right->xmin + Right->incr;

						if (Left->xmin > Right->xmin)
						{
							tmp = Left;
							Left = Right;
							Right = tmp;
						}
						k4 += k2;
					}

				}





				EdgeTable.finalize();
				
				
			}









			///////////////////////
			
		}
	}
//	else
	{

	}

	
	g_cnt = cnt;



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
	//objData->load("symphysis.obj");
	objData->load("teapot.obj");
	//objData->load("bone.obj");
//	objData->load("roi.obj");

	m_ptMouse.x = 320;
	m_ptMouse.y = 240;
	bIsScanline = false;
	bIsWireFrame = false;
	printf("hello");

	
	
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


	drawPixelColor(myRound(x), myRound(y),z);
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		z += zIncrement;
		drawPixelColor(myRound(x), myRound(y),z);
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
		g_pScreenImage[y][x][1] = 0;
		g_pScreenImage[y][x][2] = 0;
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
	
	drawPixel(myRound(x), myRound(y));
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		drawPixel(myRound(x), myRound(y));
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
	case 'l':
		g_lpCamera3D->dir.x = 0.0f;
		g_lpCamera3D->dir.y = 0.0f;
		g_lpCamera3D->dir.z = 0.0f;
		g_lpCamera3D->target.x = 0.0f;
		g_lpCamera3D->target.y = 0.0f;
		g_lpCamera3D->target.z = 1.0f;
		break;
	case '1':
		printf("Sequential Pipeline Test(inside check)\n");
		VertexShader.m_bIsMultiThread = false;
		bIsScanline = false;
		Benchmark();
		break;
	case '2':
		printf("Sequential Pipeline Test(Scanline)\n");
		VertexShader.m_bIsMultiThread = false;
		bIsScanline = true;
		Benchmark();
		break;
	case '3':
		printf("Parallel Pipeline Test(inside check)\n");
		VertexShader.m_bIsMultiThread = true;
		bIsScanline = false;
		Benchmark();
		break;
	case '4':
		printf("Wire Frame Rendering Test\n");
		VertexShader.m_bIsMultiThread = false;
		bIsScanline = false;
		bIsWireFrame = true;
		Benchmark();
		break;
	case '9':
		printf("Serial Pipeline Test\n");
		VertexShader.m_bIsMultiThread = false;
		PipelineCheck();
		break;
	case '0':
		printf("Parallel Pipeline Test\n");
		VertexShader.m_bIsMultiThread = true;
		PipelineCheck();
		break;

	default:
		break;
		
	}
	display();
	VertexShader.m_bIsMultiThread = false;
	bIsScanline = false;
	bIsWireFrame = false;
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

void Benchmark()
{
	int i,k;
	float angleX = 0;
	float angleY = 0;
	float theta = 0;
	
	float slice = 0.02f;
	DWORD dwStartTime;
	DWORD dwEndTime;
	DWORD dwTotal = 0;

	int iter = (2 * 3.141592) / slice;
	
	
	
	//g_lpCamera3D->pos.y -= 50;
	float dist = Length_Vector3D(g_lpCamera3D->pos);
	
	
	
	//dist = dist * 2;
	
	//for (k = 0; k < 3; k++)
	{

		g_lpCamera3D->pos.x = 0;
		g_lpCamera3D->pos.y = 0;
		g_lpCamera3D->pos.z = dist;
		

		
		//initDist = initDist/2;
		
		for (i = 0; i < iter; i++)
		{
			//printf("i : %d\n", i);
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
			g_lpCamera3D->pos.x = -dist*sin(theta);
			g_lpCamera3D->pos.z = -dist*cos(theta);
			
			dwStartTime = timeGetTime();
			display();
			dwTotal += timeGetTime() - dwStartTime;
			//printf("%f %f %f\n", g_lpCamera3D->pos.x, g_lpCamera3D->pos.y, g_lpCamera3D->pos.z);
		}
		//dist = dist *0.5;
		
			
	}
	
//	printf("frame : %d\nelapsed time : %d ms\nfps : %f\n", iter * 3, dwTotal, (float)(iter * 3) / dwTotal*1000);
	printf("frame : %d\nelapsed time : %d ms\nfps : %f\n", iter, dwTotal, (float)(iter) / dwTotal * 1000);
		
}

void PipelineCheck()
{
	DWORD dwStartTime;
	DWORD dwEndTime;
	dwStartTime = timeGetTime();
	display();

	dwEndTime = timeGetTime();

	printf("elapsed time : %d ms\n", dwEndTime - dwStartTime);
}