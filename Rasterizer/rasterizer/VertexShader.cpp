#include "VertexShader.h"


CVertexShader::CVertexShader()
{
}


CVertexShader::~CVertexShader()
{
}

bool CVertexShader::Setup(void* something)
{
	//여기서 좌표변환행렬 설정

	return true;
}
Vertex* CVertexShader::Launch(Vertex* original_vertex)
{
	for (Vertex* ptr = original_vertex; ptr != NULL; ptr = ptr->next)
	{
		//m_pVertex의 마지막 노드 next에 Transform(ptr)을 달아주면 될것

	}

	return m_pVertex;
}

Vertex* CVertexShader::Transform(Vertex* input)
{
	//여기서 matrix transform
	Vertex* something = new Vertex;

	//do something


	return something;
}