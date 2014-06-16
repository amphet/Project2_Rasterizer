#pragma once
#include "Global.h"
#include <thread>



class CVertexShader
{
public:
	CVertexShader();
	~CVertexShader();

	bool initialized;
	///bool m_bIsMultiThread;
	bool Setup(void* something);
	bool startup(int Vertexcnt);

	Vertex* Launch();
	bool Transform(Vertex* input);
	//Vertex* m_pVertex;
	//char* m_pVertexTest;
	//_POINT3D* m_pNormVec;
	_POINT3D* m_pPoints;//transposed vertice
	

	_POINT3D m_lighting;
	//std::thread m_Threads[VERTEX_THREAD_NUM];
	//HANDLE hEvents[VERTEX_THREAD_NUM];
	//HANDLE hEndEvents[VERTEX_THREAD_NUM];
	//bool bThreadRun[VERTEX_THREAD_NUM];
	
	Matrix3D WorldMat;
	Matrix3D ViewMat;
	int m_nVertexCount;
	
	//float m_fMinZ;
	//float m_fMaxZ;
};

