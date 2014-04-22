
#pragma once



#include "objLoader.h"
#include <windows.h>

typedef struct _Vertex{
	float x;
	float y;
	float z;
	_Vertex* next;
}Vertex;

typedef struct _Tri_Polygon{
	int a;
	int b;
	int c;
	_Tri_Polygon* next;
}Triangular_Face;



///global variable
extern Vertex* g_pVertex;
extern Triangular_Face* g_pTriangular_Face;



//global function
extern Vertex* getMeshByIndex(int i);

