#include "VertexShader.h"
#include <stdio.h>


void MatTrans(int id, CVertexShader* p);
int g_VerCnt;

CVertexShader::CVertexShader()
{
	initialized = false;
	m_bIsMultiThread = false;
	Setup((void*)1);
	_POINT3D cam_pos, cam_dir, cam_target;
	Init_Vector3D(cam_pos, 0, 0, -100);//teapot!!
//	Init_Vector3D(cam_pos, 0, 0, -30);//symphysis!!

	Init_Vector3D(cam_dir, 0, 0, 0);
	Init_Vector3D(cam_target, 0, 0, 1);
	g_lpCamera3D = Init_Camera(cam_pos, cam_dir, cam_target, 5.0f, 5000.0f, 90.0f, 640, 480);
	m_lighting.x = 100;
	m_lighting.y = 100;
	m_lighting.z = 0;

}


CVertexShader::~CVertexShader()
{
	delete m_pVertex;
	delete m_pNormVec;
	delete m_pVertexTest;
	if (g_lpCamera3D != NULL)
	{
		delete g_lpCamera3D;
		g_lpCamera3D = NULL;
	}
}

bool CVertexShader::Setup(void* something)
{
	WorldMat.v[0][0] = 1;
	WorldMat.v[0][1] = 0;
	WorldMat.v[0][2] = 0;
	WorldMat.v[0][3] = 0;

	WorldMat.v[1][0] = 0;
	WorldMat.v[1][1] = 1;
	WorldMat.v[1][2] = 0;
	WorldMat.v[1][3] = 0;

	WorldMat.v[2][0] = 0;
	WorldMat.v[2][1] = 0;
	WorldMat.v[2][2] = 1;
	WorldMat.v[2][3] = 0;

	WorldMat.v[3][0] = 0;
	WorldMat.v[3][1] = 0;
	WorldMat.v[3][2] = 0;
	WorldMat.v[3][3] = 1;


	
	return true;
}
Vertex* CVertexShader::Launch(objLoader* pLoader)
{
	//if (initialized) return m_pVertex;
	if (initialized == false)
	{

		m_nVertexCount = pLoader->vertexCount;
		//m_pVertex = new Vertex[m_nVertexCount];
		g_VerCnt = m_nVertexCount;

		m_pPoints = new __POINT3D[m_nVertexCount];
		m_pNormVec = new __POINT3D[pLoader->faceCount];
		m_pVertexTest = new char[m_nVertexCount];
		printf("# of vertice : %d\n", m_nVertexCount);
		m_pObjLoader = pLoader;
	}

	//printf("-------------------------------------------\n");
	//for (int i = 0; i < m_nVertexCount; i++)
	//	m_pVertexTest[i] = 0;

	if (m_bIsMultiThread == false)
	{
		for (int i = 0; i < m_nVertexCount; i++)
		{

			m_pPoints[i].x = pLoader->vertexList[i]->e[0];
			m_pPoints[i].y = pLoader->vertexList[i]->e[1];
			m_pPoints[i].z = pLoader->vertexList[i]->e[2];
			m_pPoints[i].w = 0;


		}

		//printf("\nviewpoint coordinate : \n");
		m_fMinZ = m_fMaxZ = 0;

		for (int i = 0; i < m_nVertexCount; i++)
		{


			_POINT3D p;
			Mult_VM3D(p, m_pPoints[i], g_lpCamera3D->mcam);
			m_pPoints[i].x = p.x;
			m_pPoints[i].y = p.y;
			m_pPoints[i].z = p.z;
			m_pPoints[i].w = p.w;

			float z = m_pPoints[i].z;

			if (z > 1.0f)
			{
				z = g_lpCamera3D->zoom / z;
				m_pPoints[i].x = m_pPoints[i].x * z;
				m_pPoints[i].y = m_pPoints[i].y * z;
			}
			else
			{
				z = g_lpCamera3D->zoom;
				m_pPoints[i].x = m_pPoints[i].x * z;
				m_pPoints[i].y = m_pPoints[i].y * z;
			}

			m_pPoints[i].x = m_pPoints[i].x + g_lpCamera3D->viewport_center_x;
			m_pPoints[i].y = -m_pPoints[i].y + g_lpCamera3D->viewport_center_y;

			if (m_pPoints[i].z < m_fMinZ) m_fMinZ = m_pPoints[i].z;
			if (m_pPoints[i].z > m_fMaxZ) m_fMaxZ = m_pPoints[i].z;
			//m_pVertexTest[i]++;
			//printf("%f %f %f %f\n", m_pPoints[i].x, m_pPoints[i].y, m_pPoints[i].z);
		}
	}
	else
	{
		for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			m_Threads[i] = std::thread(&MatTrans, i, this);
			//m_Threads[i].detach();
		}
		for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			
			m_Threads[i].join();
			//m_Threads[i].~thread();
		}
		//printf("aa");
	}
	
	//for (int i = 0; i < m_nVertexCount; i++)
	{
		//if (m_pVertexTest[i] != 1) printf("sival! %d:%d\n", i, m_pVertexTest[i]);
	}
	//for lighting
	m_lighting.x = 20;
	m_lighting.y = 80;
	m_lighting.z = 0;
	m_lighting.w = 0;
	_POINT3D p;
	Mult_VM3D(p, m_lighting, g_lpCamera3D->mcam);
	m_lighting.x = p.x;
	m_lighting.y = p.y;
	m_lighting.z = p.z;
	m_lighting.w = p.w;
	float z = m_lighting.z;
	if (z > 1.0f)
	{
		z = g_lpCamera3D->zoom / z;
		m_lighting.x *= z;
		m_lighting.y *= z;
	}
	else
	{
		z = g_lpCamera3D->zoom;
		m_lighting.x *= z;
		m_lighting.y *= z;
	}

	m_lighting.x += g_lpCamera3D->viewport_center_x;
	m_lighting.y = -m_lighting.y + g_lpCamera3D->viewport_center_y;





	initialized = true;
	return m_pVertex;
}

bool CVertexShader::Transform(Vertex* input)
{
	//¿©±â¼­ matrix transform
	


	return true;
}


void MatTrans(int id,CVertexShader* p) {
	//printf("my id is %d\n", id);
	/*int i = id*(p->m_nVertexCount / VERTEX_THREAD_NUM);
	int End = (p->m_nVertexCount/VERTEX_THREAD_NUM) * (id+1);
	if (i == VERTEX_THREAD_NUM*(VERTEX_THREAD_NUM - 1) && End < p->m_nVertexCount)
		End = p->m_nVertexCount;
	_POINT3D temp;
	float z;*/
	int i = id;
	int End = (p->m_nVertexCount);
	_POINT3D temp;
	float z;

	while (i < End)
	{
		//printf("THREAD %d is doing %d\n", id, i);
		p->m_pPoints[i].x = p->m_pObjLoader->vertexList[i]->e[0];
		p->m_pPoints[i].y = p->m_pObjLoader->vertexList[i]->e[1];
		p->m_pPoints[i].z = p->m_pObjLoader->vertexList[i]->e[2];
		p->m_pPoints[i].w = 0;

		
		Mult_VM3D(temp, p->m_pPoints[i], g_lpCamera3D->mcam);
		p->m_pPoints[i].x = temp.x;
		p->m_pPoints[i].y = temp.y;
		p->m_pPoints[i].z = temp.z;
		p->m_pPoints[i].w = temp.w;

		z = p->m_pPoints[i].z;

		if (z > 1.0f)
		{
			z = g_lpCamera3D->zoom / z;
			p->m_pPoints[i].x = p->m_pPoints[i].x * z;
			p->m_pPoints[i].y = p->m_pPoints[i].y * z;
		}
		else
		{
			z = g_lpCamera3D->zoom;
			p->m_pPoints[i].x = p->m_pPoints[i].x * z;
			p->m_pPoints[i].y = p->m_pPoints[i].y * z;
		}

		p->m_pPoints[i].x = p->m_pPoints[i].x + g_lpCamera3D->viewport_center_x;
		p->m_pPoints[i].y = -p->m_pPoints[i].y + g_lpCamera3D->viewport_center_y;
		p->m_pVertexTest[i]++;
		i += VERTEX_THREAD_NUM;
	}
}