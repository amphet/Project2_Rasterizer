#include "MultiThreadRasterizer.h"



CMultiThreadRasterizer::CMultiThreadRasterizer()
{
	m_nThreadNum = DEFAULT_THREAD_NUM;
	m_bThreadRun = true;
	InitializeCriticalSection(&m_CS);
}


CMultiThreadRasterizer::~CMultiThreadRasterizer()
{
	if(m_bThreadRun)  DeleteRasterizerThreads();

	DeleteCriticalSection(&m_CS);
}


void CMultiThreadRasterizer::CreateRasterizerThreads(int numofthreads)//numofthreas 개 thread 생성
{
	if (numofthreads)	m_nThreadNum = numofthreads;

	printf("create %d threads...\n",m_nThreadNum);
	m_pEvents = new HANDLE[m_nThreadNum];
	m_pEndEvents = new HANDLE[m_nThreadNum];

	for (int i = 0; i < numofthreads; i++)
	{
		m_bThreadRun = true;
		m_pEvents[i] = CreateEvent(NULL, false, false, NULL);
		m_pEndEvents[i] = CreateEvent(NULL, false, false, NULL);

	}
//	m_pThreads = new std::thread[m_nThreadNum];

	for (int i = 0; i < numofthreads; i++)
	{
		m_pThreads[i] = std::thread(&CMultiThreadRasterizer::ThreadFunction, this,i);
		m_pThreads[i].detach();
	}
}
void CMultiThreadRasterizer::DeleteRasterizerThreads()//스레드 모두 삭제
{
	m_bThreadRun = false;
	for (int i = 0; i < m_nThreadNum; i++)
	{
		
		SetEvent(m_pEvents[i]);
	}
	
	WaitForMultipleObjects(m_nThreadNum, m_pEndEvents, true, INFINITE);

	for (int i = 0; i < m_nThreadNum; i++)
	{
		CloseHandle(m_pEvents[i]);
		CloseHandle(m_pEndEvents[i]);
	}
	delete m_pEvents;
	delete m_pEndEvents;


}
void CMultiThreadRasterizer::SetBinningValue(int row, int col){//binning크기에 맞춰 
	m_nBinColCnt = col;
	m_nBinRowCnt = row;

}
void CMultiThreadRasterizer::InvokeRasterizerThreads(){//thread wakeup
	
	printf("hi!!!!!!1\n");
	//reset ZBUFFER
	for (int i = 0; i < 480; i++)
	{
		for (int j = 0; j < 640; j++)
		{
			m_fZBuffer[i][j] = FLT_MAX;
		}
		
	}


	//signal to threads
	m_nfinger=0;

	for (int i = 0; i < m_nThreadNum; i++)
	{

		SetEvent(m_pEvents[i]);
	}
}
void CMultiThreadRasterizer::WaitForRasterizerThreads(){//thread작업끝날때까지 대기
	WaitForMultipleObjects(m_nThreadNum, m_pEndEvents, true, INFINITE);
}

inline int myRound(const float a) { return int(a + 0.5); }


void CMultiThreadRasterizer::ThreadFunction(int id){//스레드 함수
	
	printf("Thread Created(ID : %d) \n", id);
	

	int a, b, c;

	__POINT3D p1, p2, p3;
	float maxX, minX, maxY, minY;
	float aa, bb, cc, dd;
	int myFinger;
	int XSizeOfBin = 640/m_nBinColCnt;
	int YSizeOfBin = 480/m_nBinRowCnt;
	int XStartOfBin;
	int YStartOfBin;
	int XEndOfBin;
	int YEndOfBin;
	printf("bin size : %d, %d\n", XSizeOfBin, YSizeOfBin);
	int nTotalBinSize = m_nBinColCnt*m_nBinRowCnt;

	printf("%d-1\n", id);
	
	while (1)
	{
		
		WaitForSingleObject(m_pEvents[id], INFINITE);
		//	printf("GET!! : %d\n", id);
		if (m_bThreadRun == false)
		{
			printf("ready to kill thread %d...\n", id);
			break;
		}

		while (1)
		{

			EnterCriticalSection(&m_CS);
			myFinger = m_nfinger;
			
			m_nfinger++;
			LeaveCriticalSection(&m_CS);

			//printf("thread %d get %d\n", id, myFinger);
			if (myFinger >= m_nBinRowCnt*m_nBinColCnt)
			{
				SetEvent(m_pEndEvents[id]);
				break;
			}

			
			int myRow = myFinger / m_nBinColCnt;
			int myCol = myFinger % m_nBinColCnt;
			printf("thread %d is process on (%d,%d)binnning...\n", myRow, myCol);
			/*
			
			(xStartOfBin,Ystartofbin) aaa ... aaaaaaaaaaaaaaaa (xEndOfBin-1,Ystartofbin)
			...
			aaaaa.....							(bin)						...aaaaaaaaa
			....
			(xstartOfBin,Ystartofbin-1) aaa ... aaaaaaaaaaaaaaaa (xEndOfBin-1,Yendofbin-1)

			*/
			XStartOfBin = myCol * XSizeOfBin;
			YStartOfBin = myRow * YSizeOfBin;
			XEndOfBin = XStartOfBin + XSizeOfBin;
			YEndOfBin = YStartOfBin + YSizeOfBin;
			printf("thread %d get bin from (%d,%d) to (%d,%d)\n",id, XStartOfBin, YStartOfBin, XEndOfBin, YEndOfBin);
			bool hit = false;
			char mode = 0;
			bool bIsInside;
			//char cnt;

			printf("nuofface in threqd : %d\n", m_nNumOfFace);
			int hello_ = 0;
			for (int i = 0; i < m_nNumOfFace; i++)
			{
				if (m_pFace[i].bisBackFace) continue;
				hello_++;
			//	if (i>900)
				//printf("face %d color is :%d\n",i, m_pFace[i].nColor);
				a = m_pFace[i].VertexIndex[0];//objData->faceList[i]->vertex_index[0];
				b = m_pFace[i].VertexIndex[1];//objData->faceList[i]->vertex_index[1];
				c = m_pFace[i].VertexIndex[2];//objData->faceList[i]->vertex_index[2];




				p1.x = m_pVertex[a].x;
				p1.y = m_pVertex[a].y;
				p1.z = m_pVertex[a].z;

				p2.x = m_pVertex[b].x;
				p2.y = m_pVertex[b].y;
				p2.z = m_pVertex[b].z;

				p3.x = m_pVertex[c].x;
				p3.y = m_pVertex[c].y;
				p3.z = m_pVertex[c].z;



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

				bIsInside = false;
				//cnt = 0;

				//if (XStartOfBin <= maxX && minX < XEndOfBin && YStartOfBin <= maxY && minY < YEndOfBin) bIsInside = true;
				//if (Hfrom <= maxY && minY < Hto) bIsInside = true;

				//if (!bIsInside) continue;

				


				float k1, k2, k3, k4, k5, k6, k7, k8;

				k1 = p1.x*p2.y - p2.x*p1.y + (p2.x - p1.x)*(myRound(minY) - 2);
				k2 = p2.x*p3.y - p3.x*p2.y + (p3.x - p2.x)*(myRound(minY) - 2);
				k3 = p3.x*p1.y - p1.x*p3.y + (p1.x - p3.x)*(myRound(minY) - 2);

				//isDraw = false;
				for (int y = myRound(minY) - 1; y < myRound(maxY) + 1; y++)
				{
					k1 += (p2.x - p1.x);
					k2 += (p3.x - p2.x);
					k3 += (p1.x - p3.x);

					hit = false;
					if (y < YStartOfBin) continue;
					if (y >= YEndOfBin) break;


					aa = k1 + (p1.y - p2.y) * (myRound(minX) - 2);
					bb = k2 + (p2.y - p3.y) * (myRound(minX) - 2);
					cc = k3 + (p3.y - p1.y) * (myRound(minX) - 2);

					for (int x = myRound(minX) - 1; x < myRound(maxX) + 1; x++)
					{

						aa += (p1.y - p2.y);
						bb += (p2.y - p3.y);
						cc += (p3.y - p1.y);
						//aa = (p1.y - p2.y)*x + (p2.x - p1.x)*y + p1.x*p2.y - p2.x*p1.y;
						//   = p1.y*x - p2.y*x + p2.x*y - p1.x*y + p1.x*p2.y - p2.x*p1.y;

						//bb = (p2.y - p3.y)*x + (p3.x - p2.x)*y + p2.x*p3.y - p3.x*p2.y;
						//   = p2.y*x - p3.y*x + p3.x*y - p2.x*y + p2.x*p3.y - p3.x*p2.y;

						//cc = (p3.y - p1.y)*x + (p1.x - p3.x)*y + p3.x*p1.y - p1.x*p3.y;
						//   = p3.y*x - p1.y*x + p1.x*y - p3.x*y + p3.x*p1.y - p1.x*p3.y;
						if (aa < 0 && bb < 0 && cc < 0)
						{
							if (x < 0 || XEndOfBin <= x || y < YStartOfBin || YEndOfBin <= y) continue;
							hit = true;
							dd = (pFace[i].NormVec.x*(x - p3.x) + pFace[i].NormVec.y*(y - p3.y)) / (-pFace[i].NormVec.z) + p3.z;


							if (dd < m_fZBuffer[y][x])
							{

								//drawPixelColor(x, y, pFace[i].nColor);
								m_pScreenBuffer[y][x][0] = 255;//m_pFace[i].nColor;
								m_pScreenBuffer[y][x][1] = 0;
								m_pScreenBuffer[y][x][2] = 0;
								m_fZBuffer[y][x] = dd;
								///isDraw = true;
							}


						}
						else
						{
							if (hit)
								break;
						}



					}
				}
				//if (!isDraw){
				//	cnt++;
					//printf("%d(%d,%d) is not drawing : %.2f,%.2f/%.2f,%.2f/%.2f,%.2f\n", id,Hfrom,Hto,p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
			//	}

			}
			printf("thread proc : %d\n", hello_);
		}
		//printf("%d  skip %d total : %d\n", id,cnt,totalcnt);


	}
	printf("Thread Kill(ID : %d) \n", id);
	SetEvent(m_pEndEvents[id]);
}

void CMultiThreadRasterizer::Rasterizer(){//rasterizer 함수


}


/*******************************************************************************/
/*
pEdgeTable[id]->Initialize(i);

if (pEdgeTable[id]->m_EdgeTable.empty())
{
printf("????????????????\n");
continue;
}
float bucket[3] = { 0, 0, 0 };
char cnt = 0;
for (std::list<EdgeIndex>::iterator pos = pEdgeTable[id]->m_EdgeTable.begin(); pos != pEdgeTable[id]->m_EdgeTable.end(); pos++)
{
bucket[cnt] = pos->y;
cnt++;
}
//printf("%f %f %f\n", bucket[0], bucket[1], bucket[2]);

float k1, k2, k3, k4;

//z값 보간
//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;
//-> dd = k1*x + k2+y + k3으로
k1 = -(VertexShader.m_pNormVec[i].x / VertexShader.m_pNormVec[i].z);
k2 = -(VertexShader.m_pNormVec[i].y / VertexShader.m_pNormVec[i].z);
k3 = (VertexShader.m_pNormVec[i].x*p3.x + VertexShader.m_pNormVec[i].y*p3.y) / VertexShader.m_pNormVec[i].z + p3.z;

float from, to;

if (bucket[1] == 0)
{
for (std::list<EdgeEntryWrapper>::iterator pos = pEdgeTable[id]->m_EdgeTable.front().Entry.begin(); pos != pEdgeTable[id]->m_EdgeTable.front().Entry.end(); pos++)
{
EdgeEntryWrapper ITEM;
ITEM.pEntry = pos->pEntry;

pEdgeTable[id]->m_MergeList.push_back(ITEM);
}


EdgeEntry* Left;
EdgeEntry* Right;
EdgeEntry* tmp;
Left = pEdgeTable[id]->m_MergeList.front().pEntry;
Right = pEdgeTable[id]->m_MergeList.back().pEntry;
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
if (y < Hfrom)
{
k4 += k2;
continue;
}
if (y >= Hto) break;

if (Left->xmin - 1 < 0) from = 0;
else from = Left->xmin - 1;
if (SCREEN_WIDTH <= Right->xmin + 1) to = SCREEN_WIDTH;
else to = Right->xmin + 1;

//yy = myRound(y);
//k4 = k2*yy + k3;
dd = k1*from + k4;
yy = myRound(y);
for (float x = from; x < to; x++)
{
if (x < 0) continue;
if (x >= SCREEN_WIDTH) break;
xx = myRound(x);

//dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
//dd = -(N.x/N.z)*x + N.x*p3.x/N.z - (N.y/N.z)*y + N.y*p3.y/N.z + p3.z;
//dd = -((N.x / N.z)*x + (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;





//	if (xx < 0 || SCREEN_WIDTH <= xx || yy < 0 || SCREEN_HEIGHT <= yy) continue;
if (dd < g_pZBuffer[yy][xx])
{
drawPixelColor(xx, yy, pColor[i]);
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
for (std::list<EdgeEntryWrapper>::iterator pos = pEdgeTable[id]->m_EdgeTable.front().Entry.begin(); pos != pEdgeTable[id]->m_EdgeTable.front().Entry.end(); pos++)
{
EdgeEntryWrapper ITEM;
ITEM.pEntry = pos->pEntry;

pEdgeTable[id]->m_MergeList.push_back(ITEM);
}


EdgeEntry* Left;
EdgeEntry* Right;
EdgeEntry* tmp;
Left = pEdgeTable[id]->m_MergeList.front().pEntry;
Right = pEdgeTable[id]->m_MergeList.back().pEntry;
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
if (y < Hfrom)
{
k4 += k2;
continue;
}
if (y >= Hto) break;


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
drawPixelColor(xx, yy, pColor[i]);
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

Left = pEdgeTable[id]->m_MergeList.front().pEntry;
Right = pEdgeTable[id]->m_MergeList.back().pEntry;
if (Left->ymax == bucket[1]) pEdgeTable[id]->m_MergeList.pop_front();
if (Right->ymax == bucket[1]) pEdgeTable[id]->m_MergeList.pop_back();

for (std::list<EdgeEntryWrapper>::iterator pos = pEdgeTable[id]->m_EdgeTable.back().Entry.begin(); pos != pEdgeTable[id]->m_EdgeTable.back().Entry.end(); pos++)
{
EdgeEntryWrapper ITEM;
ITEM.pEntry = pos->pEntry;

pEdgeTable[id]->m_MergeList.push_back(ITEM);
}

Left = pEdgeTable[id]->m_MergeList.front().pEntry;
Right = pEdgeTable[id]->m_MergeList.back().pEntry;
if (Left->xmin > Right->xmin)
{
tmp = Left;
Left = Right;
Right = tmp;
}

k4 = k2*bucket[1] + k3;
for (float y = bucket[1]; y < maxY; y++)
{
if (y < Hfrom)
{
k4 += k2;
continue;
}
if (y >= Hto) break;


if (Left->xmin - 1 < 0) from = 0;
else from = Left->xmin - 1;
if (SCREEN_WIDTH <= Right->xmin + 1) to = SCREEN_WIDTH;
else to = Right->xmin + 1;

yy = myRound(y);
//k4 = k2*yy + k3;
dd = k1*from + k4;
for (float x = from; x < to; x++)
{
if (x < 0) continue;
if (x >= SCREEN_WIDTH) break;

xx = myRound(x);


//dd = (N.x*(x - p3.x) + N.y*(y - p3.y)) / (-N.z) + p3.z;
//dd = -((N.x / N.z)*x +\ (N.y / N.z)*y) + N.x*p3.x / N.z + N.y*p3.y / N.z + p3.z;

//if (xx < 0 || SCREEN_WIDTH <= xx || yy < 0 || SCREEN_HEIGHT <= yy) continue;
if (dd < g_pZBuffer[yy][xx])
{
drawPixelColor(xx, yy, pColor[i]);
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





pEdgeTable[id]->finalize();


*/
