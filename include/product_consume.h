/*======================================================================================
ģ����  ��
�ļ���  ��product_consume.h
����ļ���product_consume.cpp
ʵ�ֹ��ܣ�ʵ�� ������-������ģʽ.
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
----------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2014/12/26   V1.0    ������                    ����
========================================================================================*/

#ifndef _MTG_MCCHN_H_
#define _MTG_MCCHN_H_

#include "osp.h"
#include "locker.h"
#include "recur_locker.h"
#include "rw_locker.h"
#include <queue>

using namespace std;

class Task
{
public:
    Task();
    ~Task(void);
    void Init();
	void Release();
	
	void StartThread();
	void StopTread();
	
	BOOL32 IsProductWorking() {return m_byProductState==1;}
	BOOL32 IsConsumeWorking() {return m_byConsumeState==1;}

private:

	SEMHANDLE            m_hEmpty;
	SEMHANDLE            m_hFull;

	SEMHANDLE            m_hQueueLock;
	//Locker			 m_tLocker;
	//RecurLocker		 m_tLocker;
	RwLocker             m_tLocker;
	queue<u32>           m_qQueue;

	friend void*         ProductProc(void* pParam);
	TASKHANDLE           m_hProductThreadID;
	u8                   m_byProductState;

	friend void*         ConsumeProc(void* pParam);
	TASKHANDLE           m_hConsumeThreadID;
	
	friend void*         ConsumeProc1(void* pParam);
	TASKHANDLE           m_hConsumeThreadID1;

	friend void*         ConsumeProc2(void* pParam);
	TASKHANDLE           m_hConsumeThreadID2;

	friend void*         ConsumeProc3(void* pParam);
	TASKHANDLE           m_hConsumeThreadID3;

	u8                   m_byConsumeState;
};

#endif // _MTG_MCCHN_H_
