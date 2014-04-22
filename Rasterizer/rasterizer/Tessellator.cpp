#include "Tessellator.h"


CTessellator::CTessellator()
{
	m_pFace = NULL;//dummy head로 수정하는게 좋을것

}


CTessellator::~CTessellator()
{
}

Triangular_Face* CTessellator::Launch(obj_face** original_face)
{
	/*
	objLoader에 있는 폴리곤 정보를 가져다 tessellation처리
	Vertex Shader의 launch와 마찬가지로 변환한 face들을 m_pFace에 linked list로 저장한뒤 head를 리턴


	내가보기에는 고정된 폴리곤이니까 처음한번만 tessellation하고, 이후에는 이미 저장된 m_pFace를 리턴하면 될것같음
	
	예를들면 멤버변수 m_bConverted 만들어서 생성자에서 초기값 false로 넣어준다음에

	if(m_bConverted == false)
	{
		original_face정보를 가져다 삼각형 폴리곤으로 쪼갠뒤, m_pFace에 append

		m_bConverted = true;
	}

	return m_pFace;
	
	이런식으로


	original_face[i] 이런식으로 접근하면 될것임.
	e.g. 그냥 삼각형 폴리곤일때 처리하는방법
	
	Triangular_Face* temp2 = g_pTriangular_Face;
	for (int i = 0; i < objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];
		temp2->next = new Triangular_Face;
		temp2 = temp2->next;
		temp2->a = o->vertex_index[0];
		temp2->b = o->vertex_index[1];
		temp2->c = o->vertex_index[2];
		temp2->next = NULL;
	
	}

	실제 구현시에는 OBJLoader의 example.txt참조해서 폴리곤 종류에 따라 처리가 가능하도록 해야 할듯

	*/


	return m_pFace;
}
