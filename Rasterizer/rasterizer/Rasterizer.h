#pragma once
#include "Global.h"


class CRasterizer
{
public:
	CRasterizer();
	~CRasterizer();

	void Launch(_POINT3D p1, _POINT3D p2, _POINT3D p3, _POINT3D Norm, float(*zBuff)[640], char color);
	BYTE(*m_pScreen)[640][3];
	void SetScreenBufferPtr(BYTE(*ptr)[640][3]){ m_pScreen = ptr; }
	void SetBinningSize(int x0, int y0, int x1, int y1)
	{
		m_nX0 = x0;
		m_nY0 = y0;
		m_nX1 = x1;
		m_nY1 = y1;
	}
	int m_nX0,m_nY0,m_nX1,m_nY1;
	
	
};

