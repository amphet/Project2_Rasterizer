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
	//WaitForMultipleObjects(VERTEX_THREAD_NUM, hEndEvents, true, INFINITE);
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
bool CVertexShader::startup(objLoader* pLoader)
{
	m_nVertexCount = pLoader->vertexCount;
	//m_pVertex = new Vertex[m_nVertexCount];
	g_VerCnt = m_nVertexCount;

	m_pPoints = new __POINT3D[m_nVertexCount];
	m_pNormVec = new __POINT3D[pLoader->faceCount];
	//m_pVertexTest = new char[m_nVertexCount];
	printf("# of vertice : %d\n", m_nVertexCount);
	m_pObjLoader = pLoader;


	int from = 0;
	int cnt = 0;
	int mod = m_nVertexCount % VERTEX_THREAD_NUM;

	for (int i = 0; i < VERTEX_THREAD_NUM; i++)
	{
		cnt = m_nVertexCount / VERTEX_THREAD_NUM;
		if (i < mod) cnt++;
		//m_Threads[i] = std::thread(&MatTrans, i, from, from + cnt, this);
		//m_Threads[i].detach();

		//SetThreadPriority(m_Threads[i].native_handle(), THREAD_PRIORITY_HIGHEST);
		from += cnt;
	}
	/*
	//create normal vec
	for (int i = 0; i < m_nVertexCount; i++)
	{
		m_pPoints[i].x = pLoader->vertexList[i]->e[0];
		m_pPoints[i].y = pLoader->vertexList[i]->e[1];
		m_pPoints[i].z = pLoader->vertexList[i]->e[2];
		m_pPoints[i].w = 0;
	}
	_POINT3D p1, p2, p3, N;
	_POINT3D v1, v2;
	for (int i = 0; i < pLoader->faceCount; i++)
	{
		int a = pLoader->faceList[i]->vertex_index[0];
		int b = pLoader->faceList[i]->vertex_index[1];
		int c = pLoader->faceList[i]->vertex_index[2];




		p1.x = m_pPoints[a].x;
		p1.y = m_pPoints[a].y;
		p1.z = m_pPoints[a].z;

		p2.x = m_pPoints[b].x;
		p2.y = m_pPoints[b].y;
		p2.z = m_pPoints[b].z;

		p3.x = m_pPoints[c].x;
		p3.y = m_pPoints[c].y;
		p3.z = m_pPoints[c].z;



		v1.x = p2.x - p1.x;
		v1.y = p2.y - p1.y;
		v1.z = p2.z - p1.z;
		v1.w = 1;

		v2.x = p3.x - p1.x;
		v2.y = p3.y - p1.y;
		v2.z = p3.z - p1.z;
		v2.w = 1;

		Cross_Vector3D(N, v1, v2);
		
		Normalize_Vector3D(N);
		m_pNormVec[i].x = N.x;
		m_pNormVec[i].y = N.y;
		m_pNormVec[i].z = N.z;
		m_pNormVec[i].w = N.w;
		

	}*/
	initialized = true;
	return true;
}
Vertex* CVertexShader::Launch(objLoader* pLoader)
{
	//if (initialized) return m_pVertex;
	if (initialized == false) startup(pLoader);

	//	float zoom = 1000.0f;
	//printf("-------------------------------------------\n");
//	for (int i = 0; i < m_nVertexCount; i++)
//		m_pVertexTest[i] = 0;
	_POINT3D p;
	
	if (0)//m_bIsMultiThread)
	{
		for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			//SetThreadPriority(m_Threads[i].native_handle(), THREAD_PRIORITY_HIGHEST);
			SetEvent(hEvents[i]);
		}


		//for (int i = 0; i < VERTEX_THREAD_NUM; i++)
		{
			//SetThreadPriority(m_Threads[i].native_handle(), THREAD_PRIORITY_LOWEST);
			//SetEvent(hEvents[i]);
		}
	}
	else
	{
	
		for (int i = 0; i < m_nVertexCount; i++)
		{

			m_pPoints[i].x = pLoader->vertexList[i]->e[0];
			m_pPoints[i].y = pLoader->vertexList[i]->e[1];
			m_pPoints[i].z = pLoader->vertexList[i]->e[2];
			m_pPoints[i].w = 0;
			
			Mult_VM3D(p, m_pPoints[i], g_lpCamera3D->mcam);
			float z = p.z;

			if (z > 1.0f) z = g_lpCamera3D->zoom / z;
			else z = g_lpCamera3D->zoom;
			
			p.x = p.x * z + +g_lpCamera3D->viewport_center_x;;
			p.y = -p.y * z + g_lpCamera3D->viewport_center_y;;

			//p.x = p.x + g_lpCamera3D->viewport_center_x;
			//p.y = -p.y + g_lpCamera3D->viewport_center_y;

			m_pPoints[i].x = p.x;
			m_pPoints[i].y = p.y;
			m_pPoints[i].z = p.z;
			m_pPoints[i].w = p.w;
			/*
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
			m_pPoints[i].y = -m_pPoints[i].y + g_lpCamera3D->viewport_center_y;*/

		//	if (m_pPoints[i].z < m_fMinZ) m_fMinZ = m_pPoints[i].z;
		//	if (m_pPoints[i].z > m_fMaxZ) m_fMaxZ = m_pPoints[i].z;
			//m_pVertexTest[i]++;
			//printf("%f %f %f %f\n", m_pPoints[i].x, m_pPoints[i].y, m_pPoints[i].z);
		}
	}

	
//	for (int i = 0; i < m_nVertexCount; i++)
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
//	if(m_bIsMultiThread) WaitForMultipleObjects(VERTEX_THREAD_NUM, hEndEvents, true, INFINITE);
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
		//to = p->m_nVertexCount;
		while (i < to)
		{
			//printf("THREAD %d is doing %d\n", id, i);
			vtx.x = vList[i]->e[0];
			vtx.y = vList[i]->e[1];
			vtx.z = vList[i]->e[2];
			vtx.w = 0;


			Mult_VM3D(temp, vtx, g_lpCamera3D->mcam);

			z = temp.z;

			if (z > 1.0f) z = zoom / z;
			else z = zoom;
			
			temp.x = temp.x * z + viewport_center_x;;
			temp.y = -temp.y * z + viewport_center_y;;
			//temp.x = temp.x +	viewport_center_x;
			//temp.y = -temp.y + viewport_center_y;
		//	p->m_pVertexTest[i]++;

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