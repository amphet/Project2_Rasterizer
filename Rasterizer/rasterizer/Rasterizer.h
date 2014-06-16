#pragma once
#include "Global.h"


class CRasterizer
{
public:
	CRasterizer();
	~CRasterizer();

	void Launch(_POINT3D p1, _POINT3D p2, _POINT3D p3, _POINT3D Norm, BYTE(*screen)[640][3], float(*zBuff)[640], char color);

};

