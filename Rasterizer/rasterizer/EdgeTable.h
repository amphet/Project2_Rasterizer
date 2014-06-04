#pragma once
#include "global.h"
#include "VertexShader.h"
#include <list>


typedef struct _EdgeEntry
{
	float ymax;
	float ymin;
	float xmin;
	float incr;

} EdgeEntry;

typedef struct _EdgeEntryWrapper
{
	EdgeEntry* pEntry;
}EdgeEntryWrapper;

typedef struct _EdgeIndex
{
	float y;
	std::list<EdgeEntryWrapper> Entry;
}EdgeIndex;


class CEdgeTable
{
public:
	CEdgeTable(CVertexShader* VShader);
	~CEdgeTable();

	bool Initialize(int faceID);
	void finalize();

	CVertexShader* m_pVShader;
	
	std::list<EdgeIndex> m_EdgeTable;
	std::list<EdgeEntryWrapper> m_MergeList;

	void insert_index(EdgeEntry* entry);
	void insert_Entry(EdgeEntry* entry, std::list<EdgeEntryWrapper>* Entries);
};

