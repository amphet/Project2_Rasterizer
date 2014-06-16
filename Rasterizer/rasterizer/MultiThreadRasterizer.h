#pragma once
#include "Global.h"
#include <thread>
#include "Rasterizer.h"

#define DONT_CHANGE 0

#define DEFAULT_THREAD_NUM 1
class CMultiThreadRasterizer
{
public:
	CMultiThreadRasterizer();
	~CMultiThreadRasterizer();
	
	std::thread m_pThreads[9];
	HANDLE *m_pEvents;
	HANDLE *m_pEndEvents;
	HANDLE *m_pKillEvents;
	bool m_bThreadRun;
	CRITICAL_SECTION m_CS;
	

	
	MyFaceInfo *m_pFace;
	void SetFacePtr(MyFaceInfo *ptr){ m_pFace = ptr; }
	_POINT3D* m_pVertex;
	void SetVertexPtr(_POINT3D* ptr){ m_pVertex = ptr; }
	void SetNumOfFace(int numFace){ m_nNumOfFace = numFace; }
	
	float m_fZBuffer[480][640];
	byte (*m_pScreenBuffer)[640][3];
	void SetScreenBufferPtr(byte (*ptr)[640][3]){ m_pScreenBuffer = ptr; }
	
	

	int m_nfinger;
	int m_nThreadNum;
	int m_nBinRowCnt;//bin의 row 수
	int m_nBinColCnt;//bin의 column 수
	int m_nNumOfFace;
	
	
	void CreateRasterizerThreads(int numofthreads);//numofthreas 개 thread 생성
	void DeleteRasterizerThreads();//스레드 모두 삭제
	void SetBinningValue(int row, int col);//binning 정보 설정
	void InvokeRasterizerThreads();//signaling
	void WaitForRasterizerThreads();//스레드 작업 끝날때까지 대기
	void ThreadFunction(int id);
	void Rasterizer();

};

