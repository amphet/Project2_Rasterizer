
#pragma once



#include "objLoader.h"
#include <windows.h>

typedef struct _Vertex{
	float x;
	float y;
	float z;
}Vertex;

typedef struct _Vector{
	float x;
	float y;
	float z;
	float w;
}Vector;
typedef struct _Matrix3D{
	float v[4][4];

} Matrix3D;



extern void Mat3D_Mul(Matrix3D& m, Matrix3D& m1, Matrix3D& m2);
extern void Mat3D_Vec_Mul(Vector& p, Vector& v, Matrix3D& m);
extern objLoader *objData;
typedef struct _Tri_Polygon{
	int a;
	int b;
	int c;
	_Tri_Polygon* next;
}Triangular_Face;


typedef struct __POINT3D
{
	float x;
	float y;
	float z;
	float w;

} _POINT3D, *_LPPOINT3D;


typedef struct _MyFaceInfo
{
	int VertexIndex[3];
	_POINT3D NormVec;
	bool bisBackFace;
	char nColor;
} MyFaceInfo;


typedef struct __PLANE3D
{
	_POINT3D p;
	_POINT3D n;

} _PLANE3D, *_LPPLANE3D;

typedef struct __MATRIX3D
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

} _MATRIX3D, *_LPMATRIX3D;


typedef struct __CAMERA3D
{
	_POINT3D pos;
	_POINT3D dir;
	_POINT3D u;
	_POINT3D v;
	_POINT3D n;
	_POINT3D target;
	float view_dist_h;
	float view_dist_v;
	float fov;
	float near_clip_z;
	float far_clip_z;
	float viewplane_width;
	float viewplane_height;
	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;
	float aspect_ratio;
	float zoom;
	_PLANE3D rt_clip_plane;
	_PLANE3D lt_clip_plane;
	_PLANE3D tp_clip_plane;
	_PLANE3D bt_clip_plane;
	_MATRIX3D mcam;
	_MATRIX3D mper;
	_MATRIX3D mscr;

} _CAMERA3D, *_LPCAMERA3D;

#define CAMERA_OFFSET	0.025f

extern void Add_Vector3D(_POINT3D& v, _POINT3D& v1, _POINT3D& v2);
extern void Sub_Vector3D(_POINT3D& v, _POINT3D& v1, _POINT3D& v2);
extern void Update_Camera(_LPCAMERA3D& lpCamera3D);
extern void Mult_MM3D(_MATRIX3D& m, _MATRIX3D& m1, _MATRIX3D& m2);
extern void Mult_VM3D(_POINT3D& p, _POINT3D& v, _MATRIX3D& m);
extern void Init_Matrix3D(_MATRIX3D& m, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);
extern void Init_Matrix3D(_MATRIX3D& m);
extern _LPCAMERA3D Init_Camera(_POINT3D pos, _POINT3D dir, _POINT3D target, float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height);
///global variable
extern Vertex* g_pVertex;
extern Triangular_Face* g_pTriangular_Face;
extern void Init_Vector3D(_POINT3D& v, float x, float y, float z);
extern void Init_Plane3D(_PLANE3D& p, _POINT3D o, _POINT3D n, BOOL bNormalize);
extern void Update_Mouse(long x, long y, _LPCAMERA3D& lpCamera3D);
//global function
extern Vertex* getMeshByIndex(int i);
extern _LPCAMERA3D g_lpCamera3D;
extern POINT m_ptMouse;
extern void Cross_Vector3D(_POINT3D& v, _POINT3D v1, _POINT3D v2);
extern float Length_Vector3D(_POINT3D v);
extern float Dot_Vector3D(_POINT3D v1, _POINT3D v2);
extern void Normalize_Vector3D(_POINT3D& v);

extern MyFaceInfo *pFace;
extern _POINT3D *pVertice;