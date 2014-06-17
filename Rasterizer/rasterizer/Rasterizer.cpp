#include "Rasterizer.h"
#include <math.h>
#include <iostream>


//temporal function
inline int myRound(const float a) { return int(a + 0.5); }
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

enum EdgeRecordStatus
{
	NORMAL_EDGE,
	HORIZON,
	VERTICAL
};

typedef struct _EdgeRecord{
	int y;
	float fy;
	float xmin, ymax, incr;
	EdgeRecordStatus status;
} EdgeRecord;

void initEdgeRecord(EdgeRecord *record, float _y, float _xmin, float _ymax, float _incr, EdgeRecordStatus _status)
{
	record->y = myRound(_y);
	record->fy = _y;
	record->xmin = _xmin;
	record->ymax = _ymax;
	record->incr = _incr;
	record->status = _status;
}

EdgeRecord makeEdge(_POINT3D p1, _POINT3D p2)
{
	int _y;
	float _xmin, _ymax;
	float _incr = 0;

	if (p1.y < p2.y)
	{
		_y = myRound(p1.y);
		_xmin = p1.x;
		_ymax = p2.y;
	}
	else
	{
		_y = myRound(p2.y);
		_xmin = p2.x;
		_ymax = p1.y;
	}

	EdgeRecord ret;
	EdgeRecordStatus _status = NORMAL_EDGE;
	if (p1.x == p2.x)
	{
		_status = VERTICAL;
		_incr = 0;
	}
	else if (myRound(p1.y) == myRound(p2.y))
	{
		_status = HORIZON;
	}
	else
	{
		_incr = (p1.x - p2.x) / (p1.y - p2.y);
		//		_xmin += _incr * ((float)_y - min(p1.y, p2.y));	// real?? honntoni?
	}
	//	_xmin = myRound(_xmin);
	initEdgeRecord(&ret, _y, _xmin, _ymax, _incr, _status);
	return ret;
}

void initEdgeTable(EdgeRecord EdgeTable[3], EdgeRecord e1, EdgeRecord e2, EdgeRecord e3)
{
	// ascending order with y
	if (e1.y < e2.y)
	{
		if (e1.y < e3.y)
		{
			EdgeTable[0] = e1;
			if (e2.y < e3.y)
			{
				EdgeTable[1] = e2;
				EdgeTable[2] = e3;
			}
			else
			{
				EdgeTable[1] = e3;
				EdgeTable[2] = e2;
			}
		}
		else
		{
			EdgeTable[0] = e3;
			EdgeTable[1] = e1;
			EdgeTable[2] = e2;
		}
	}
	else
	{
		if (e2.y < e3.y)
		{
			EdgeTable[0] = e2;
			if (e1.y < e3.y)
			{
				EdgeTable[1] = e1;
				EdgeTable[2] = e3;
			}
			else
			{
				EdgeTable[1] = e3;
				EdgeTable[2] = e1;
			}
		}
		else
		{
			EdgeTable[0] = e3;
			EdgeTable[1] = e2;
			EdgeTable[2] = e1;
		}
	}

	// ascending order with xmin when y is equal to each other
	if (EdgeTable[0].y == EdgeTable[1].y)
	{
		if (EdgeTable[0].xmin > EdgeTable[1].xmin)
		{
			EdgeRecord temp = EdgeTable[0];
			EdgeTable[0] = EdgeTable[1];
			EdgeTable[1] = temp;
		}
	}
	if (EdgeTable[1].y == EdgeTable[2].y)
	{
		if (EdgeTable[1].xmin > EdgeTable[2].xmin)
		{
			EdgeRecord temp = EdgeTable[1];
			EdgeTable[1] = EdgeTable[2];
			EdgeTable[2] = temp;
		}
	}

	// ascending order with incr when y and xmin are equal to each other
	if (EdgeTable[0].y == EdgeTable[1].y && EdgeTable[0].xmin == EdgeTable[1].xmin)
	{
		if (EdgeTable[0].incr > EdgeTable[1].incr)
		{
			EdgeRecord temp = EdgeTable[0];
			EdgeTable[0] = EdgeTable[1];
			EdgeTable[1] = temp;
		}
	}
	if (EdgeTable[1].y == EdgeTable[2].y && EdgeTable[1].xmin == EdgeTable[2].xmin)
	{
		if (EdgeTable[1].incr > EdgeTable[2].incr)
		{
			EdgeRecord temp = EdgeTable[1];
			EdgeTable[1] = EdgeTable[2];
			EdgeTable[2] = temp;
		}
	}

	// exclude horizon edge
	if (EdgeTable[0].status == HORIZON)
	{
		EdgeRecord temp1 = EdgeTable[0];
		EdgeRecord temp2 = EdgeTable[1];
		EdgeTable[1] = EdgeTable[2];
		EdgeTable[0] = temp2;
		EdgeTable[2] = temp1;
	}
	else if (EdgeTable[1].status == HORIZON)
	{
		EdgeRecord temp = EdgeTable[1];
		EdgeTable[1] = EdgeTable[2];
		EdgeTable[2] = temp;
	}
}

bool inScreen(int x, int y)
{
	bool isxin = (0 < x) && (x < SCREEN_WIDTH);
	bool isyin = (0 < y) && (y < SCREEN_HEIGHT);
	return isxin && isyin;
}

float calcDistance(int x, int y, EdgeRecord e)
{
	float a = -1;
	float b = e.incr;
	float c = e.xmin - ((float)e.y) * e.incr;
	return abs(((float)x)*a + ((float)y)*b + c) / sqrt(a*a + b*b);
}

float calcArea(_POINT3D p1, _POINT3D p2, _POINT3D p3)
{
	float s = (p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p2.x*p1.y - p3.x*p2.y - p1.x*p3.y) / 2;
	return abs(s);
}

void printEdge(EdgeRecord e)
{
	std::cout << e.y << "/" << e.xmin << "/" << e.ymax << "/" << e.incr << "\n";
}
void printEdgeTable(EdgeRecord t[3])
{
	std::cout << "e1: ";
	printEdge(t[0]);
	std::cout << "e2: ";
	printEdge(t[1]);
	std::cout << "e3: ";
	printEdge(t[2]);
}
void printPOINT3D(_POINT3D p)
{
	std::cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << "\n";
}

CRasterizer::CRasterizer()
{
}


CRasterizer::~CRasterizer()
{
}

void CRasterizer::Launch(_POINT3D p1, _POINT3D p2, _POINT3D p3, _POINT3D Norm, float(*zBuff)[640], char color)
{
	int diff1 = abs(p1.y - p2.y);
	int diff2 = abs(p2.y - p3.y);
	int diff3 = abs(p3.y - p1.y);
	if ((diff1 <= 1) && (diff2 <= 1) && (diff3 <= 1))
	{
//		std::cout << "this is real\n";
		return;
	}
	
	EdgeRecord ETable[3];	//ETable[0]: lowest y entry ~ Etable[3]: highest y entry
	EdgeRecord e1, e2, e3;
	e1 = makeEdge(p1, p2);	// e1: p1~p2
	e2 = makeEdge(p2, p3);	// e2: p2~p3
	e3 = makeEdge(p3, p1);	// e3: p3~p1
	initEdgeTable(ETable, e1, e2, e3);

	//	printEdgeTable(ETable);	//debug
	float topmostY = max(p1.y, p2.y);
	topmostY = max(topmostY, p3.y);
	int topmosty = myRound(topmostY);
	//	std::cout << topmosty << "\n";	//debug

//	float ffromx, ftox;
	int fromx, tox;
	int fidx, tidx;
	fidx = 0;
	tidx = 1;
//	int fcnt, tcnt;	// from and to counter
//	fcnt = tcnt = 0;
	float dd;

	/*
	ffromx = (ETable[fidx].incr*(float)ETable[0].y + ETable[fidx].xmin - ETable[fidx].incr*ETable[fidx].fy);
	ftox = (ETable[tidx].incr*(float)ETable[0].y + ETable[tidx].xmin - ETable[tidx].incr*ETable[tidx].fy);
	fromx = myRound(ffromx);
	ftox = myRound(ftox);
	*/

	for (int y = ETable[0].y; y <= topmosty; y++)
	{
		if (m_nY1 <= y) break;
		/*
		if (ETable[2].status == HORIZON)
		{
		printEdgeTable(ETable);
		}
		*/
		if ((float(y) > ETable[fidx].ymax ||
			float(y) > ETable[tidx].ymax))
		{
			if (fidx == 2 || tidx == 2) break;
			if (float(y) > ETable[2].ymax) break;
			if (y == myRound(ETable[fidx].ymax))
			{
				fidx = 2;
//				fcnt = 0;
			}
			else if (y == myRound(ETable[tidx].ymax))
			{
				tidx = 2;
//				tcnt = 0;
			}
//			fcnt++;
//			tcnt++;

			/*
			std::cout << "p1: "; printPOINT3D(p1);
			std::cout << "p2: "; printPOINT3D(p2);
			std::cout << "p3: "; printPOINT3D(p3);
			std::cout << "s: :" << calcArea(p1, p2, p3) << "\n";
			std::cout << "\n";
			continue;
			*/
			//continue;
			
			fromx = myRound(ETable[fidx].incr*(float)y+ETable[fidx].xmin - ETable[fidx].incr*ETable[fidx].fy);
			tox = myRound(ETable[tidx].incr*(float)y+ETable[tidx].xmin - ETable[tidx].incr*ETable[tidx].fy);
			/*
			fromx = myRound(ffromx);
			tox = myRound(ftox);
			*/

		}
		else
		{
			
			fromx = myRound(ETable[fidx].incr*(float)y + ETable[fidx].xmin - ETable[fidx].incr*ETable[fidx].fy);
			tox = myRound(ETable[tidx].incr*(float)y + ETable[tidx].xmin - ETable[tidx].incr*ETable[tidx].fy);
			

			/*
			ffromx += ETable[fidx].incr;
			ftox += ETable[tidx].incr;
			fromx = myRound(ffromx);
			tox = myRound(ftox);
			*/
		}
		/* tiny polygon check */
		/*
		float s = (p1.x*p2.y + p2.x*p3.y + p3.x*p1.y - p2.x*p1.y - p3.x*p2.y - p1.x*p3.y) / 2;
		if(s<1)
		{
		int _x = myRound(p1.x);
		int _y = myRound(p1.y);
		if(inScreen(_x,_y)){
		m_pScreen[_y][_x][0] = color;
		m_pScreen[_y][_x][1] = color;
		m_pScreen[_y][_x][2] = color;
		}
		}
		*/
		/*
		ffromx += ETable[fidx].incr;
		ftox += ETable[tidx].incr;
		fromx = myRound(ffromx);
		tox = myRound(ftox);
		*/

		//		if (1 / ETable[fidx].incr == 0) fromx = myRound(ETable[fidx].xmin);
		/*else*/
		//		fromx = myRound(ETable[fidx].xmin + (float)fcnt * ETable[fidx].incr);	// old
		//fromx = myRound(ETable[fidx].incr*(float)y + ETable[fidx].xmin - ETable[fidx].incr*ETable[fidx].fy);	// new
		//		if (1 / ETable[tidx].incr == 0) tox = myRound(ETable[tidx].xmin);
		/*else*/
		//		tox = myRound(ETable[tidx].xmin + (float)tcnt * ETable[tidx].incr);	// old
		//tox = myRound(ETable[tidx].incr*(float)y + ETable[tidx].xmin - ETable[tidx].incr*ETable[tidx].fy);	// new
		//		std::cout << "from " << fromx << " to " << tox << "/idx: " << fidx << ", " << tidx << "\n";	// debug
		
		/* debug zone */
		/*
		if (fromx > tox)
		{

		float da = ETable[fidx].xmin + (float)fcnt * ETable[fidx].incr;
		float db = ETable[tidx].xmin + (float)tcnt * ETable[tidx].incr;
		int x = 2;
		//			printEdgeTable(ETable);
		//			std::cout << "\n" << fromx << "->" << tox << "\n";
		}
		*/
		//		if (ETable[tidx].status == HORIZON) std::cout << "dont do that\n";
		/*
		if (fromx > tox)
		{
		//int temp = fromx;
		//fromx = tox;
		}
		*/
		
		if (m_nY0 <= y && y < m_nY1)
		{
			for (int x = fromx + 1; x <= tox; x++)
			{
				/* distance check */
				/*
				if (calcDistance(x, y, ETable[fidx]) > 1000.0)
				{
				break;
				}
				*/
				/* check end */

				if (x < m_nX0) continue;
				if (m_nX1 <= x) break;
				dd = (Norm.x*(x - p3.x) + Norm.y*(y - p3.y)) / (-Norm.z) + p3.z;

				if (/*inScreen(x, y) && */(dd < zBuff[y][x]))
				{
					m_pScreen[y][x][0] = color;
					m_pScreen[y][x][1] = color;
					m_pScreen[y][x][2] = color;
					zBuff[y][x] = dd;
				}

				/*debug zone*/
				/*
				if (x > 640)
				{
				printEdgeTable(ETable);
				std::cout << "\n";
				}
				*/
				//if (p1.x > 640) printPOINT3D(p1);
			}
		}

		if (y == myRound(ETable[fidx].ymax))
		{
			fidx = 2;
//			fcnt = 0;
		}
		else if (y == myRound(ETable[tidx].ymax))
		{
			tidx = 2;
//			tcnt = 0;
		}
//		fcnt++;
//		tcnt++;
	}


	return;
}