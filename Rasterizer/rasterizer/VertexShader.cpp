#include "VertexShader.h"


CVertexShader::CVertexShader()
{
}


CVertexShader::~CVertexShader()
{
}

bool CVertexShader::Setup(void* something)
{
	//���⼭ ��ǥ��ȯ��� ����

	return true;
}
Vertex* CVertexShader::Launch(Vertex* original_vertex)
{
	for (Vertex* ptr = original_vertex; ptr != NULL; ptr = ptr->next)
	{
		//m_pVertex�� ������ ��� next�� Transform(ptr)�� �޾��ָ� �ɰ�

	}

	return m_pVertex;
}

Vertex* CVertexShader::Transform(Vertex* input)
{
	//���⼭ matrix transform
	Vertex* something = new Vertex;

	//do something


	return something;
}