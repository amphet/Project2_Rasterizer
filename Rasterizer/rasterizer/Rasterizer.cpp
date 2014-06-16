#include "Rasterizer.h"
#include <math.h>
#include <iostream>


//temporal function
inline int myRound(const float a) { return int(a + 0.5); }

typedef struct _EdgeRecord{
	int y;
	float xmin, ymax, incr;
} EdgeRecord;

void initEdgeRecord(EdgeRecord *record, int _y, float _xmin, float _ymax, float _incr)
{
	record->y = _y;
	record->xmin = _xmin;
	record->ymax = _ymax;
	record->incr = _incr;
}

EdgeRecord makeEdge(_POINT3D p1, _POINT3D p2)
{
	int _y;
	float _xmin, _ymax;
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
	initEdgeRecord(&ret, _y, _xmin, _ymax, (p1.x - p2.x)/(p1.y - p2.y));
	return ret;
}

void initEdgeTable(EdgeRecord EdgeTable[3], EdgeRecord e1, EdgeRecord e2, EdgeRecord e3)
{
	if(e1.y < e2.y)
	{
		if(e1.y < e3.y)
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
		if(e2.y < e3.y)
		{
			EdgeTable[0] = e2;
			if(e1.y < e3.y)
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

	if(EdgeTable[0].y == EdgeTable[1].y)
	{
		if(EdgeTable[0].xmin > EdgeTable[1].xmin)
		{
			EdgeRecord temp = EdgeTable[0];
			EdgeTable[0] = EdgeTable[1];
			EdgeTable[1] = temp;
		}
	}
	if(EdgeTable[1].y == EdgeTable[2].y)
	{
		if(EdgeTable[1].xmin > EdgeTable[2].xmin)
		{
			EdgeRecord temp = EdgeTable[1];
			EdgeTable[1] = EdgeTable[2];
			EdgeTable[2] = temp;
		}
	}

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

	if (1 / EdgeTable[0].incr == 0)
	{
		EdgeRecord temp1 = EdgeTable[0];
		EdgeRecord temp2 = EdgeTable[1];
		EdgeTable[1] = EdgeTable[2];
		EdgeTable[0] = temp2;
		EdgeTable[2] = temp1;
	}
	else if (1 / EdgeTable[1].incr == 0)
	{
		EdgeRecord temp = EdgeTable[1];
		EdgeTable[1] = EdgeTable[2];
		EdgeTable[2] = temp;

	}
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

CRasterizer::CRasterizer()
{
}


CRasterizer::~CRasterizer()
{
}

void CRasterizer::Launch(_POINT3D p1, _POINT3D p2, _POINT3D p3, float (*screen)[480])
{
	EdgeRecord ETable[3];	//ETable[0]: lowest y entry ~ Etable[3]: highest y entry
	EdgeRecord e1, e2, e3;
	e1 = makeEdge(p1, p2);	// e1: p1~p2
	e2 = makeEdge(p2, p3);	// e2: p2~p3
	e3 = makeEdge(p3, p1);	// e3: p3~p1
	initEdgeTable(ETable, e1, e2, e3);

	printEdgeTable(ETable);
	float topmostY = max(p1.y, p2.y);
	topmostY = max(topmostY, p3.y);
	int topmosty = myRound(topmostY);
//	std::cout << topmosty << "\n";
	int fromx, tox;
	int fidx, tidx;
	fidx = 0;
	tidx = 1;
	int fcnt, tcnt;	// from and to counter
	fcnt = tcnt = 0;
	for (int y = ETable[0].y; y <= topmosty; y++)
	{
		if (1 / ETable[fidx].incr == 0) fromx = myRound(ETable[fidx].xmin);
		else fromx = myRound(ETable[fidx].xmin + fcnt * ETable[fidx].incr);
		if (1 / ETable[tidx].incr == 0) tox = myRound(ETable[tidx].xmin);
		else tox = myRound(ETable[tidx].xmin + tcnt * ETable[tidx].incr);
		std::cout << "from " << fromx << " to " << tox << "/idx: " << fidx << ", " << tidx << "\n";	// debug
		for (int x = fromx; x <= tox; x++)
		{
			screen[x][y] = 1.;
		}
		if (y == ETable[fidx].ymax)
		{
			fidx = 2;
			fcnt = -1;
		}
		else if (y == ETable[tidx].ymax)
		{
			tidx = 2;
			tcnt = -1;
		}
		fcnt++;
		tcnt++;
	}


	return;
}
