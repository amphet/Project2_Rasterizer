

#include "global.h"
#include <math.h>
#include <stdio.h>

Vertex* g_pVertex;
Triangular_Face* g_pTriangular_Face;
objLoader *objData;


Vertex* getMeshByIndex(int i)
{
	int cnt = 0;

	/*
	Vertex* now = g_pVertex->next;
	while (1)
	{
		if (cnt == i) return now;
		now = now->next;
		cnt++;
	}*/
	return NULL;
}

void Mat3D_Mul(Matrix3D& m, Matrix3D& m1, Matrix3D& m2)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m.v[i][j] = 0;
			for (int k = 0; k < 4; k++)
				m.v[i][j] = m1.v[i][k] * m2.v[k][j];
		}
	}
}

float Dot_Vector3D(_POINT3D v1, _POINT3D v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}


void Normalize_Vector3D(_POINT3D& v)
{
	float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x = v.x / len;
	v.y = v.y / len;
	v.z = v.z / len;
	v.w = 1.0f;
}

void Mat3D_Vec_Mul(Vector& p, Vector& v, Matrix3D& m)
{
	p.x = v.x*m.v[0][0] + v.y*m.v[1][0] + v.z*m.v[2][0] + m.v[3][0];
	p.y = v.x*m.v[0][1] + v.y*m.v[1][1] + v.z*m.v[2][1] + m.v[3][1];
	p.z = v.x*m.v[0][2] + v.y*m.v[1][2] + v.z*m.v[2][2] + m.v[3][2];
	p.w = 1.0f;
}

void Add_Vector3D(_POINT3D& v, _POINT3D& v1, _POINT3D& v2)
{
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	v.z = v1.z + v2.z;
	v.w = 1.0f;
}


void Sub_Vector3D(_POINT3D& v, _POINT3D& v1, _POINT3D& v2)
{
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;
	v.w = 1.0f;
}

void Update_Camera(_LPCAMERA3D& lpCamera3D)
{
	_MATRIX3D mt_inv, mx_inv, my_inv, mz_inv, m_temp, m_rot;

	float theta_x = lpCamera3D->dir.x;
	float theta_y = lpCamera3D->dir.y;
	float theta_z = lpCamera3D->dir.z;

	Init_Matrix3D(mt_inv, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -lpCamera3D->pos.x, -lpCamera3D->pos.y, -lpCamera3D->pos.z, 1);

	float cos_theta = cos(theta_x);
	float sin_theta = -sin(theta_x);
	Init_Matrix3D(mx_inv, 1, 0, 0, 0, 0, cos_theta, sin_theta, 0, 0, -sin_theta, cos_theta, 0, 0, 0, 0, 1);

	cos_theta = cos(theta_y);
	sin_theta = -sin(theta_y);
	Init_Matrix3D(my_inv, cos_theta, 0, -sin_theta, 0, 0, 1, 0, 0, sin_theta, 0, cos_theta, 0, 0, 0, 0, 1);

	cos_theta = cos(theta_z);
	sin_theta = -sin(theta_z);
	Init_Matrix3D(mz_inv, cos_theta, sin_theta, 0, 0, -sin_theta, cos_theta, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

	Mult_MM3D(m_temp, mz_inv, my_inv);
	Mult_MM3D(m_rot, m_temp, mx_inv);
	Mult_MM3D(lpCamera3D->mcam, mt_inv, m_rot);
	/*printf("CAMERA MATRIX\n");
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", 
		lpCamera3D->mcam.m00, lpCamera3D->mcam.m00, lpCamera3D->mcam.m00, lpCamera3D->mcam.m03,
		lpCamera3D->mcam.m10, lpCamera3D->mcam.m11, lpCamera3D->mcam.m12, lpCamera3D->mcam.m13,
		lpCamera3D->mcam.m20, lpCamera3D->mcam.m21, lpCamera3D->mcam.m22, lpCamera3D->mcam.m23,
		lpCamera3D->mcam.m30, lpCamera3D->mcam.m31, lpCamera3D->mcam.m32, lpCamera3D->mcam.m33 );
	printf("\n");*/
}

void Mult_MM3D(_MATRIX3D& m, _MATRIX3D& m1, _MATRIX3D& m2)
{
	m.m00 = m1.m00*m2.m00 + m1.m01*m2.m10 + m1.m02*m2.m20;
	m.m01 = m1.m00*m2.m01 + m1.m01*m2.m11 + m1.m02*m2.m21;
	m.m02 = m1.m00*m2.m02 + m1.m01*m2.m12 + m1.m02*m2.m22;
	m.m03 = 0.0f;

	m.m10 = m1.m10*m2.m00 + m1.m11*m2.m10 + m1.m12*m2.m20;
	m.m11 = m1.m10*m2.m01 + m1.m11*m2.m11 + m1.m12*m2.m21;
	m.m12 = m1.m10*m2.m02 + m1.m11*m2.m12 + m1.m12*m2.m22;
	m.m13 = 0.0f;

	m.m20 = m1.m20*m2.m00 + m1.m21*m2.m10 + m1.m22*m2.m20;
	m.m21 = m1.m20*m2.m01 + m1.m21*m2.m11 + m1.m22*m2.m21;
	m.m22 = m1.m20*m2.m02 + m1.m21*m2.m12 + m1.m22*m2.m22;
	m.m23 = 0.0f;

	m.m30 = m1.m30*m2.m00 + m1.m31*m2.m10 + m1.m32*m2.m20 + m2.m30;
	m.m31 = m1.m30*m2.m01 + m1.m31*m2.m11 + m1.m32*m2.m21 + m2.m31;
	m.m32 = m1.m30*m2.m02 + m1.m31*m2.m12 + m1.m32*m2.m22 + m2.m32;
	m.m33 = 1.0f;
}

void Mult_VM3D(_POINT3D& p, _POINT3D& v, _MATRIX3D& m)
{
	p.x = v.x*m.m00 + v.y*m.m10 + v.z*m.m20 + m.m30;
	p.y = v.x*m.m01 + v.y*m.m11 + v.z*m.m21 + m.m31;
	p.z = v.x*m.m02 + v.y*m.m12 + v.z*m.m22 + m.m32;
	p.w = 1.0f;
}

void Init_Matrix3D(_MATRIX3D& m)
{
	m.m00 = 1.0f;
	m.m01 = 0.0f;
	m.m02 = 0.0f;
	m.m03 = 0.0f;
	m.m10 = 0.0f;
	m.m11 = 1.0f;
	m.m12 = 0.0f;
	m.m13 = 0.0f;
	m.m20 = 0.0f;
	m.m21 = 0.0f;
	m.m22 = 1.0f;
	m.m23 = 0.0f;
	m.m30 = 0.0f;
	m.m31 = 0.0f;
	m.m32 = 0.0f;
	m.m33 = 1.0f;
}

void Init_Matrix3D(_MATRIX3D& m, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	m.m00 = m00;
	m.m01 = m01;
	m.m02 = m02;
	m.m03 = m03;
	m.m10 = m10;
	m.m11 = m11;
	m.m12 = m12;
	m.m13 = m13;
	m.m20 = m20;
	m.m21 = m21;
	m.m22 = m22;
	m.m23 = m23;
	m.m30 = m30;
	m.m31 = m31;
	m.m32 = m32;
	m.m33 = m33;
}


void Init_Vector3D(_POINT3D& v, float x, float y, float z)
{
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = 1.0f;
}
_LPCAMERA3D g_lpCamera3D = NULL;

_LPCAMERA3D Init_Camera(_POINT3D pos, _POINT3D dir, _POINT3D target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height)
{
	_LPCAMERA3D lpCamera3D = new _CAMERA3D;

	lpCamera3D->pos = pos;
	lpCamera3D->dir = dir;
	lpCamera3D->target = target;
	Init_Vector3D(lpCamera3D->u, 1, 0, 0);
	Init_Vector3D(lpCamera3D->v, 0, 1, 0);
	Init_Vector3D(lpCamera3D->n, 0, 0, 1);
	lpCamera3D->near_clip_z = near_clip_z;
	lpCamera3D->far_clip_z = far_clip_z;
	lpCamera3D->viewport_width = viewport_width;
	lpCamera3D->viewport_height = viewport_height;
	lpCamera3D->viewport_center_x = (viewport_width - 1) / 2.0f;
	lpCamera3D->viewport_center_y = (viewport_height - 1) / 2.0f;
	lpCamera3D->aspect_ratio = (float)viewport_width / (float)viewport_height;
	Init_Matrix3D(lpCamera3D->mcam);
	Init_Matrix3D(lpCamera3D->mper);
	Init_Matrix3D(lpCamera3D->mscr);
	lpCamera3D->fov = fov;
	lpCamera3D->viewplane_width = 2.0f;
	lpCamera3D->viewplane_height = 2.0f / lpCamera3D->aspect_ratio;
	float tan_fov_div2 = tan((fov / 2.0f) / 180.0f*3.141592);
	lpCamera3D->view_dist_h = 0.5f * lpCamera3D->viewplane_width * tan_fov_div2;
	lpCamera3D->view_dist_v = 0.5f * lpCamera3D->viewplane_height * tan_fov_div2;
	lpCamera3D->zoom = 0.5f * lpCamera3D->viewport_width * tan_fov_div2;

	if (fov == 90.0f)
	{
		_POINT3D ptOrigin, vn;
		Init_Vector3D(ptOrigin, 0, 0, 0);

		Init_Vector3D(vn, 1, 0, -1);
		Init_Plane3D(lpCamera3D->rt_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, -1, 0, -1);
		Init_Plane3D(lpCamera3D->lt_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, 0, 1, -1);
		Init_Plane3D(lpCamera3D->tp_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, 0, -1, -1);
		Init_Plane3D(lpCamera3D->bt_clip_plane, ptOrigin, vn, TRUE);
	}
	else
	{
		_POINT3D ptOrigin, vn;
		Init_Vector3D(ptOrigin, 0, 0, 0);

		Init_Vector3D(vn, lpCamera3D->view_dist_h, 0, -(lpCamera3D->viewplane_width / 2.0f));
		Init_Plane3D(lpCamera3D->rt_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, -lpCamera3D->view_dist_h, 0, -(lpCamera3D->viewplane_width / 2.0f));
		Init_Plane3D(lpCamera3D->lt_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, 0, lpCamera3D->view_dist_v, -(lpCamera3D->viewplane_width / 2.0f));
		Init_Plane3D(lpCamera3D->tp_clip_plane, ptOrigin, vn, TRUE);

		Init_Vector3D(vn, 0, -lpCamera3D->view_dist_v, -(lpCamera3D->viewplane_width / 2.0f));
		Init_Plane3D(lpCamera3D->bt_clip_plane, ptOrigin, vn, TRUE);
	}

	return lpCamera3D;
}

void Init_Plane3D(_PLANE3D& p, _POINT3D o, _POINT3D n, BOOL bNormalize)
{
	p.p = o;
	p.n = n;
	if (bNormalize == TRUE)
	{
		float len = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
		p.n.x = p.n.x / len;
		p.n.y = p.n.y / len;
		p.n.z = p.n.z / len;
	}
}

POINT m_ptMouse;

void Update_Mouse(long x, long y, _LPCAMERA3D& lpCamera3D)
{
	if (lpCamera3D != NULL)
	{
		long mouseX, mouseY;
	//	POINT ptMouse = m_ptMouse;
		//ScreenToClient(m_hWnd, &ptMouse);
		mouseX = m_ptMouse.x;
		mouseY = m_ptMouse.y;

		float angleX = 0;
		float angleY = 0;
		if (x - mouseX > 0)
		{
			angleY = CAMERA_OFFSET;
		}
		else if (x - mouseX < 0)
		{
			angleY = -CAMERA_OFFSET;
		}
		if (y - mouseY > 0)
		{
			angleX = CAMERA_OFFSET;
		}
		else if (y - mouseY < 0)
		{
			angleX = -CAMERA_OFFSET;
		}
		lpCamera3D->dir.y += angleY;
		if (lpCamera3D->dir.y >= 2 * 3.141592)
		{
			lpCamera3D->dir.y -= 2 * 3.141592;
		}
		else if (lpCamera3D->dir.y <= -2 * 3.141592)
		{
			lpCamera3D->dir.y += 2 * 3.141592;
		}
		lpCamera3D->dir.x += angleX;
		if (lpCamera3D->dir.x >= 2 * 3.141592)
		{
			lpCamera3D->dir.x -= 2 * 3.141592;
		}
		else if (lpCamera3D->dir.x <= -2 * 3.141592)
		{
			lpCamera3D->dir.x += 2 * 3.141592;
		}
		lpCamera3D->target.x = cos(lpCamera3D->dir.x)*sin(lpCamera3D->dir.y);
		lpCamera3D->target.z = cos(lpCamera3D->dir.x)*cos(lpCamera3D->dir.y);
		lpCamera3D->target.y = sin(-lpCamera3D->dir.x);
	}

	m_ptMouse.x = x;
	m_ptMouse.y = y;
	//SetCursorPos(m_ptMouse.x, m_ptMouse.y);
}


float Length_Vector3D(_POINT3D v)
{
	return(sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
}

void Cross_Vector3D(_POINT3D& v, _POINT3D v1, _POINT3D v2)
{
	_POINT3D n;
	Init_Vector3D(n, v1.y*v2.z - v2.y*v1.z, -v1.x*v2.z + v2.x*v1.z, v1.x*v2.y - v2.x*v1.y);
	float v1_len = Length_Vector3D(v1);
	float v2_len = Length_Vector3D(v2);

	float cos_fi = (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z) / (v1_len * v2_len);
	float sin_fi = sqrt(1.0f - cos_fi*cos_fi);

	v.x = sin_fi * n.x;
	v.y = sin_fi * n.y;
	v.z = sin_fi * n.z;
	v.w = 1.0f;
}


MyFaceInfo *pFace;
 _POINT3D *pVertice;