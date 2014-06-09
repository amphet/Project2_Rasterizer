#include "EdgeTable.h"


CEdgeTable::CEdgeTable(CVertexShader* VShader)
{
	
	m_pVShader = VShader;
}


CEdgeTable::~CEdgeTable()
{
	finalize();
}
bool CEdgeTable::Initialize(int faceID)
{
	//은면제거에서 삭제될것
	/*
	float fMinRecY, fMaxRecY,fMinRecX,fMaxRecX;

	fMinRecY = min(v1.y, v2.y);
	fMinRecY = min(fMinRecY, v3.y);

	fMaxRecY = max(v1.y, v2.y);
	fMaxRecY = max(fMaxRecY, v3.y);

	if (fMinRecY == fMaxRecY) return;

	fMinRecX = min(v1.x, v2.x);
	fMinRecX = min(fMinRecX, v3.x);

	fMaxRecX = max(v1.x, v2.x);
	fMaxRecX = max(fMaxRecX, v3.x);

	if (fMaxRecX == fMinRecX) return;
	*/
//	for (int i = 0;)
	bool bSuccess;

	int nV1Index, nV2Index, nV3Index;
	nV1Index = objData->faceList[faceID]->vertex_index[0];
	nV2Index = objData->faceList[faceID]->vertex_index[1];
	nV3Index = objData->faceList[faceID]->vertex_index[2];

	_POINT3D P1, P2, P3;
	P1.x = m_pVShader->m_pPoints[nV1Index].x;
	P1.y = m_pVShader->m_pPoints[nV1Index].y;
	P1.z = m_pVShader->m_pPoints[nV1Index].z;
	P1.w = m_pVShader->m_pPoints[nV1Index].w;

	P2.x = m_pVShader->m_pPoints[nV2Index].x;
	P2.y = m_pVShader->m_pPoints[nV2Index].y;
	P2.z = m_pVShader->m_pPoints[nV2Index].z;
	P2.w = m_pVShader->m_pPoints[nV2Index].w;
	
	P3.x = m_pVShader->m_pPoints[nV3Index].x;
	P3.y = m_pVShader->m_pPoints[nV3Index].y;
	P3.z = m_pVShader->m_pPoints[nV3Index].z;
	P3.w = m_pVShader->m_pPoints[nV3Index].w;

	float minY;
	minY = min(P1.y, P2.y);
	minY = min(minY, P3.y);

	float maxY;
	maxY = max(P1.y, P2.y);
	maxY = max(maxY, P3.y);

	if (minY == maxY)
	{
		//printf("rasterize init :: err1\n");
		return false;//이경우에는 아마 hidden face culling에서 제외될것
	}
	
	_POINT3D *minPoint, *middlePoint, *maxPoint;//y값 기준 min,middle,maxvertex

	char mask1;
	if (minY == P1.y)
	{
		minPoint = &P1;
		mask1 = 1;
	}
	else if (minY == P2.y){
		minPoint = &P2;
		mask1 = 2;
	}
	else if (minY == P3.y){
		minPoint = &P3;
		mask1 = 4;
	}
	else
	{
		printf("rasterize init :: err2\n");
		return false;
	}

	char mask2;
	if (maxY == P1.y) {
		maxPoint = &P1;
		mask2 = 1;
	}
	else if (maxY == P2.y){
		maxPoint = &P2;
		mask2 = 2;
	}
	else if (maxY == P3.y){
		maxPoint = &P3;
		mask2 = 4;
	}
	else
	{
		printf("rasterize init :: err3\n");
		return false;
	}

	bSuccess = true;
	char mask3 = mask1 + mask2; //110,101,011,중에 하나임
	switch (mask3)
	{
	case 3://011
		middlePoint = &P3;
		break;
	case 5://101
		middlePoint = &P2;
		break;
	case 6://110
		middlePoint = &P1;
		break;
	default:
		bSuccess = false;
		break;
	}
	
	if (!bSuccess){
		printf("rasterize init :: err4\n");
		return false;
	}

	
	EdgeEntry *edges[3] = { NULL, NULL, NULL };
	
	//edge[0]은 min~middle point edge
	if (middlePoint->y - minPoint->y != 0)//수평선 무시
	{
		edges[0] = new EdgeEntry;
		edges[0]->incr = (minPoint->x - middlePoint->x) / (minPoint->y - middlePoint->y);
		edges[0]->xmin = minPoint->x;
		edges[0]->ymin = minPoint->y;
		edges[0]->ymax = middlePoint->y;
	}

	//edge[1]은 middle~max point edge
	if (middlePoint->y - maxPoint->y != 0)//수평선 무시
	{
		edges[1] = new EdgeEntry;
		edges[1]->incr = (maxPoint->x - middlePoint->x) / (maxPoint->y - middlePoint->y);
		edges[1]->xmin = middlePoint->x;
		edges[1]->ymin = middlePoint->y;
		edges[1]->ymax = maxPoint->y;
	}
	
	//edge[2]는 min~max point edge
	if (maxPoint->y - minPoint->y != 0)//수평선 무시
	{
		edges[2] = new EdgeEntry;
		edges[2]->incr = (minPoint->x - maxPoint->x) / (minPoint->y - maxPoint->y);
		edges[2]->xmin = minPoint->x;
		edges[2]->ymin = minPoint->y;
		edges[2]->ymax = maxPoint->y;
	}
	
	
	//pixel cut 추가1!

	//edge table 만들기
	for (int i = 0; i < 3; i++)
	{
		if (edges[i] == NULL) continue;

		insert_index(edges[i]);

	}

	m_MergeList.clear();

}
void CEdgeTable::finalize()
{
	for (std::list<EdgeIndex>::iterator i = m_EdgeTable.begin(); i != m_EdgeTable.end(); i++)
	{

		for (std::list<EdgeEntryWrapper>::iterator j = i->Entry.begin(); j != i->Entry.end(); j++)
		{
			delete j->pEntry;
		}
		//i->Entry.clear();
	}
	m_EdgeTable.clear();


}
 
void CEdgeTable::insert_index(EdgeEntry* entry)
{
	bool bExist = false;
	for (std::list<EdgeIndex>::iterator pos = m_EdgeTable.begin(); pos != m_EdgeTable.end(); pos++)
	{
		
		if (pos->y == entry->ymin)
		{
			insert_Entry(entry, &(pos->Entry));
			bExist = true;
			break;
		}
	}

	if (!bExist)
	{
		EdgeIndex index;
		index.y = entry->ymin;
		m_EdgeTable.push_back(index);
		insert_Entry(entry, &(m_EdgeTable.back().Entry));

	}
}

void CEdgeTable::insert_Entry(EdgeEntry* entry, std::list<EdgeEntryWrapper>* Entries)
{
	EdgeEntryWrapper temp;
	temp.pEntry = entry;

	Entries->push_front(temp);
}