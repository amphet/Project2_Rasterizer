#pragma once
#include "Global.h"
class CVertexShader
{
public:
	CVertexShader();
	~CVertexShader();

	bool initialized;
	bool Setup(void* something);
	Vertex* Launch(objLoader* pLoader);
	bool Transform(Vertex* input);
	Vertex* m_pVertex;
	_POINT3D* m_pPoints;

	Matrix3D WorldMat;
	Matrix3D ViewMat;
	int m_nVertexCount;
};

