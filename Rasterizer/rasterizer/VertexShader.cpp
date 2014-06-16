#include "VertexShader.h"
#include <stdio.h>
#include <ppl.h>
#include <array>

void MatTrans(int id,int from,int to, CVertexShader* p);
int g_VerCnt;

CVertexShader::CVertexShader()
{
	initialized = false;
//	m_bIsMultiThread = false;
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

	
}


CVertexShader::~CVertexShader()
{
	delete m_pPoints;

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
bool CVertexShader::startup(int Vertexcnt)
{
	m_nVertexCount = Vertexcnt;
	
	m_pPoints = new _POINT3D[m_nVertexCount];
	
	printf("# of vertice : %d\n", m_nVertexCount);

	return true;
}
Vertex* CVertexShader::Launch()
{
	//if (initialized) return m_pVertex;
	
	_POINT3D p,cp;
	
	
	for (int i = 0; i < m_nVertexCount; i++)
	{

		/*m_pPoints[i].x = pVertice[i].x;
		m_pPoints[i].y = pVertice[i].y;
		m_pPoints[i].z = pVertice[i].z;
		m_pPoints[i].w = 0;*/
		
		cp.x = pVertice[i].x;
		cp.y = pVertice[i].y;
		cp.z = pVertice[i].z;
		cp.w = 0;

		Mult_VM3D(p, cp, g_lpCamera3D->mcam);
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



	return 0;

	
}

bool CVertexShader::Transform(Vertex* input)
{
	//¿©±â¼­ matrix transform
	


	return true;
}

