#pragma once
#include "Global.h"
class CVertexShader
{
public:
	CVertexShader();
	~CVertexShader();

	bool Setup(void* something);
	Vertex* Launch(Vertex* original_vertex);
	Vertex* Transform(Vertex* input);
	Vertex* m_pVertex;


};

