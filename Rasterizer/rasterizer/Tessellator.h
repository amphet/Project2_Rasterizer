#pragma once
#include "global.h"


class CTessellator
{
public:
	CTessellator();
	~CTessellator();

	Triangular_Face* Launch(obj_face** original_face);
	Triangular_Face* m_pFace;


};

