

#include "global.h"



Vertex* g_pVertex;
Triangular_Face* g_pTriangular_Face;



Vertex* getMeshByIndex(int i)
{
	int cnt = 0;


	Vertex* now = g_pVertex->next;
	while (1)
	{
		if (cnt == i) return now;
		now = now->next;
		cnt++;
	}
}