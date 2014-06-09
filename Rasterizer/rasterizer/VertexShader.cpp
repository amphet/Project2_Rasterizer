#include "VertexShader.h"
#include <stdio.h>
#include <ppl.h>
#include <array>

void MatTrans(int id,int from,int to, CVertexShader* p);
int g_VerCnt;

CVertexShader::CVertexShader()
{
	initialized = false;
	m_bIsMultiThread = false;
	Setup((void*)1);
	_POINT3D cam_pos, cam_dir, cam_target;
	Init_Vector3D(cam_pos, 0, 0, -100);//teapot!!
//	Init_Vector3D(cam_pos, 0, 0, -10);//symphysis!!

	Init_Vector3D(cam_dir, 0, 0, 0);
	Init_Vector3D(cam_target, 0, 0, 1);
	g_lpCamera3D = Init_Camera(cam_pos, cam_dir, cam_target, 5.0f, 5000.0f, 90.0f, 640, 480);
	m_lighting.x = 100;
	m_lighting.y = 100;
	m_lighting.z = 0;

	for (int i = 0; i < VERTEX_THREAD_NUM; i++)
	{
		bThreadRun[i] = true;
		hEvents[i] = CreateEvent(NULL, false, false, NULL);
		hEndEvents[i] = CreateEvent(NULL, false, false, NULL);
		
	}
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
	
	for (int i = 0; i < VERTEX_THREAD_NUM; i++)
	{
		bThreadRun[i] = false;
		SetEvent(hEvents[i]);
	}
	printf("11231313\n");
	WaitForMultipleObjects(VERTEX_THREAD_NUM, hEndEvents, true, INFINITE);
	for (int i = 0; i < VERTEX_THREAD_NUM; i++)
	{
		//m_Threads[i].join();

		CloseHandle(hEvents[i]);
		CloseHandle(hEndEvents[i]);
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
		

		int from = 0;
		int cnt = 0;
		int mod = m_nVertexCount % VERTEX_THREAD_NUM;

		for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			cnt = m_nVertexCount / VERTEX_THREAD_NUM;
			if (i < mod) cnt++;
			m_Threads[i] = std::thread(&MatTrans, i, from, from + cnt, this);
			m_Threads[i].detach();
			from += cnt;
		}
		initialized = true;
	}
//	float zoom = 1000.0f;
	//printf("-------------------------------------------\n");
	//for (int i = 0; i < m_nVertexCount; i++)
	//	m_pVertexTest[i] = 0;
	_POINT3D p;
	
	if (m_bIsMultiThread == false)
	{
		for (int i = 0; i < m_nVertexCount; i++)
		{

			m_pPoints[i].x = pLoader->vertexList[i]->e[0];
			m_pPoints[i].y = pLoader->vertexList[i]->e[1];
			m_pPoints[i].z = pLoader->vertexList[i]->e[2];
			m_pPoints[i].w = 0;
			
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

		//	if (m_pPoints[i].z < m_fMinZ) m_fMinZ = m_pPoints[i].z;
		//	if (m_pPoints[i].z > m_fMaxZ) m_fMaxZ = m_pPoints[i].z;
			//m_pVertexTest[i]++;
			//printf("%f %f %f %f\n", m_pPoints[i].x, m_pPoints[i].y, m_pPoints[i].z);
		}
	}
	else
	{
		for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			SetEvent(hEvents[i]);
		}

		WaitForMultipleObjects(VERTEX_THREAD_NUM, hEndEvents, true, INFINITE);
		//printf("aa");
		
	}
	
	//for (int i = 0; i < m_nVertexCount; i++)
	{
//if (m_pVertexTest[i] != 1) printf("sival! %d:%d\n", i, m_pVertexTest[i]);
	}
	//for lighting
	//teapot(20,80,0)
	m_lighting.x = 20;
	m_lighting.y = 80;
	//teapot m_lighting.z = 100;
	m_lighting.z = 0;
	m_lighting.w = 0;

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




	//printf("HI!");
	
	return m_pVertex;
}

bool CVertexShader::Transform(Vertex* input)
{
	//¿©±â¼­ matrix transform
	


	return true;
}


void MatTrans(int id,int from,int to,CVertexShader* p) {
	printf("Thread %d is covering %d to %d\n",id, from,to);
	/*int i = id*(p->m_nVertexCount / VERTEX_THREAD_NUM);
	int End = (p->m_nVertexCount/VERTEX_THREAD_NUM) * (id+1);
	if (i == VERTEX_THREAD_NUM*(VERTEX_THREAD_NUM - 1) && End < p->m_nVertexCount)
		End = p->m_nVertexCount;
	_POINT3D temp;
	float z;*/
	
	int End = (p->m_nVertexCount);
	_POINT3D temp;
	_POINT3D vtx;

	float z, zoom, viewport_center_x, viewport_center_y;
	int i = from;
	obj_vector** vList = p->m_pObjLoader->vertexList;

	while (1)
	{
		WaitForSingleObject(p->hEvents[id], INFINITE);
	//	printf("GET!! : %d\n", id);
		if (p->bThreadRun[id] == false)
		{
			printf("ready to kill thread %d...\n", id);
			break;
		}

		i = from;
		zoom = g_lpCamera3D->zoom;
		viewport_center_x = g_lpCamera3D->viewport_center_x;
		viewport_center_y = g_lpCamera3D->viewport_center_y;
		while (i < to)
		{
			//printf("THREAD %d is doing %d\n", id, i);
			vtx.x = vList[i]->e[0];
			vtx.y = vList[i]->e[1];
			vtx.z = vList[i]->e[2];
			vtx.w = 0;


			Mult_VM3D(temp, vtx, g_lpCamera3D->mcam);

			z = temp.z;

			if (z > 1.0f)
			{
				z = zoom / z;
				temp.x = temp.x * z;
				temp.y = temp.y * z;
			}
			else
			{
				z = g_lpCamera3D->zoom;
				temp.x = temp.x * z;
				temp.y = temp.y * z;
			}

			temp.x = temp.x +	viewport_center_x;
			temp.y = -temp.y + viewport_center_y;
			//p->m_pVertexTest[i]++;

			p->m_pPoints[i].x = temp.x;
			p->m_pPoints[i].y = temp.y;
			p->m_pPoints[i].z = temp.z;
			p->m_pPoints[i].w = temp.w;

			i++;
		}
		SetEvent(p->hEndEvents[id]);
	}
	printf("Thread %d is killed!\n",id);
	SetEvent(p->hEndEvents[id]);
}