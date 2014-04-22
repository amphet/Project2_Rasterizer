#pragma once
#include "Global.h"


class CRasterizer
{
public:
	CRasterizer();
	~CRasterizer();

	void Launch(Vertex* pVertex, Triangular_Face* vFace);

};

