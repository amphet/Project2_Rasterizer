#pragma once
#include "Global.h"
#include <thread>
#define VERTEX_THREAD_NUM 6


class CVertexShader
{
public:
	CVertexShader();
	~CVertexShader();

	bool initialized;
	bool m_bIsMultiThread;
	bool Setup(void* something);
	bool startup(objLoader* pLoader);

	Vertex* Launch(objLoader* pLoader);
	bool Transform(Vertex* input);
	Vertex* m_pVertex;
	char* m_pVertexTest;
	_POINT3D* m_pNormVec;
	_POINT3D* m_pPoints;
	_POINT3D m_lighting;
	std::thread m_Threads[VERTEX_THREAD_NUM];
	HANDLE hEvents[VERTEX_THREAD_NUM];
	HANDLE hEndEvents[VERTEX_THREAD_NUM];
	bool bThreadRun[VERTEX_THREAD_NUM];
	objLoader* m_pObjLoader;
	Matrix3D WorldMat;
	Matrix3D ViewMat;
	int m_nVertexCount;
	
	//float m_fMinZ;
	//float m_fMaxZ;
};

