
#include "product_consume.h"

BOOL32  SemTake_Debug(SEMHANDLE hSem,s32 nChnlId, u16 wTime, s8* file, s32 nLine);
BOOL32  SemGive_Debug(SEMHANDLE hSem,s32 nChnlId, s8* file, s32 nLine);

#define SEM_TAKEBYTIME(hSem,wTime,nChnlId) SemTake_Debug(hSem,nChnlId, wTime, __FILE__, __LINE__)
#define SEM_TAKE(hSem, nChnlId)            SemTake_Debug(hSem,nChnlId, 0, __FILE__, __LINE__)
#define SEM_GIVE(hSem,nChnlId)             SemGive_Debug(hSem,nChnlId, __FILE__, __LINE__);

void* ProductProc(void* pParam);
void* ConsumeProc(void* pParam);
void* ConsumeProc1(void* pParam);
void* ConsumeProc2(void* pParam);
void* ConsumeProc3(void* pParam);

BOOL32 SemTake_Debug(SEMHANDLE hSem,s32 nChnlId, u16 wTime, s8* file, s32 nLine)
{
    if (NULL != hSem)
    {
        BOOL32 bRet;
		
        static s32 anLast = -1;
        static s32 anLastId = -1; 
		
		if( 0 == wTime )
		{
			bRet = OspSemTake(hSem);
			
		}
		else
		{
			bRet = OspSemTakeByTime(hSem, wTime);
		}
        
        if (bRet != TRUE)
        {  
			printf("SEMTAKE Failed at %s:%d get pid:%d Last Take:%d pid:%d.\n",
                file, nLine, OspTaskSelfID(), anLast, anLastId);
        }
        else
        {
            anLast = nLine;
            anLastId = OspTaskSelfID();
			printf("SEMTAKE SUCCEED at %s:%d get pid:%d Last Take:%d pid:%d.\n",
                file, nLine, OspTaskSelfID(), anLast, anLastId);
        }
        return bRet;
    }
    else
    {
        printf("SEMTAKE Failed for sem not create at %s:%d.\n",file,nLine);
    }
    return FALSE;
}

BOOL32 SemGive_Debug(SEMHANDLE hSem,s32 nChnlId,s8* file, s32 nLine)
{
    if (NULL != hSem)
    {
        BOOL32 bRet;
        static s32 anGiveLast = -1;
        static s32 anGiveLastId = -1; 
		
		bRet = OspSemGive(hSem);
		
        if (bRet != TRUE)
        {  
			printf("SEMGIVE Failed at %s:%d get pid:%d Last Give:%d pid:%d.\n",
                file, nLine, OspTaskSelfID(), anGiveLast, anGiveLastId);
        }
        else
        {
            anGiveLast = nLine;
            anGiveLastId = OspTaskSelfID();
			printf("SEMGIVE SUCCEED at %s:%d get pid:%d Last Give:%d pid:%d.\n",
                file, nLine, OspTaskSelfID(), anGiveLast, anGiveLastId);
        }
        return bRet;
    }
    else
    {
        printf("SEMGIVE Failed for sem not create.\n");
    }
    return FALSE;
}

Task::Task()
{
	Init();

	if(NULL==m_hQueueLock)
	{
		OspSemBCreate(&m_hQueueLock);
	}

	if(NULL==m_hEmpty)
	{
		OspSemBCreate(&m_hEmpty);
		//OspSemGive(m_hEmpty);
		//OspSemCCreate(&m_hEmpty, 100, 10000);
	}

	if(NULL==m_hFull)
	{
		OspSemBCreate(&m_hFull);
		OspSemTake(m_hFull);
		//OspSemCCreate(&m_hFull, 0, 10000);
	}
	
};

Task::~Task(void)
{
	Release();

	if(NULL != m_hQueueLock)
	{
		OspSemDelete(m_hQueueLock);
	}

	if(NULL != m_hEmpty)
	{
		OspSemDelete(m_hEmpty);
	}

	if(NULL != m_hFull)
	{
		OspSemDelete(m_hFull);
	}
};

void Task::Init()
{
	m_qQueue.empty();
	m_hQueueLock = NULL;
	m_hEmpty = NULL;
	m_hFull = NULL;

	m_hProductThreadID = NULL;
	m_byProductState = 1;
	
	m_hConsumeThreadID = NULL;
	m_byConsumeState = 1;


	m_hConsumeThreadID1 = NULL;
	m_hConsumeThreadID2 = NULL;
	m_hConsumeThreadID3 = NULL;
};

void Task::Release()
{
	m_qQueue.empty();
	//m_hQueueLock = NULL;
	
	m_hProductThreadID = NULL;
	m_byProductState = 0;
	
	m_hConsumeThreadID = NULL;
	m_byConsumeState = 0;

	m_hConsumeThreadID1 = NULL;
	m_hConsumeThreadID2 = NULL;
	m_hConsumeThreadID3 = NULL;
};

void Task::StartThread()
{
	//创建 两个线程
	if (NULL == m_hProductThreadID)
	{
		m_hProductThreadID = ::OspTaskCreate(ProductProc, "product", 80, 512 * 1024, (u32)this, 0);
	}

// 	if (NULL == m_hConsumeThreadID)
// 	{
// 		m_hConsumeThreadID = ::OspTaskCreate(ConsumeProc, "consumer", 80, 512 * 1024, (u32)this, 0);
// 	}

	if (NULL == m_hConsumeThreadID1)
	{
		m_hConsumeThreadID1 = ::OspTaskCreate(ConsumeProc1, "consumer1", 80, 512 * 1024, (u32)this, 0);
	}

	if (NULL == m_hConsumeThreadID2)
	{
		m_hConsumeThreadID2 = ::OspTaskCreate(ConsumeProc2, "consumer2", 80, 512 * 1024, (u32)this, 0);
	}

	if (NULL == m_hConsumeThreadID3)
	{
		m_hConsumeThreadID3 = ::OspTaskCreate(ConsumeProc3, "consumer3", 80, 512 * 1024, (u32)this, 0);
	}
};

void Task::StopTread()
{
	m_byProductState = 0;
	m_byConsumeState = 0;
};

void* ProductProc(void* pParam)
{
	Task* pTask = (Task*)pParam;
	if (NULL == pTask)
	{
		printf("task is null, consume thread quit!\n");
		return 0;
	}
	
	while (pTask->IsProductWorking())
	{
		for (u32 dwIndex = 0; dwIndex < 100; dwIndex++)
		{
			//OspSemTake(pTask->m_hEmpty);
			//OspSemTake(pTask->m_hQueueLock);

			printf("writer thread add write lock before!\n");
			pTask->m_tLocker.GetReaders();
 			pTask->m_tLocker.WriteLock();
			//pTask->m_tLocker.Lock();
			printf("writer thread add write lock after!\n");
			pTask->m_tLocker.GetReaders();

			printf("push data:%d.\n", dwIndex);
			pTask->m_qQueue.push(dwIndex);
			
			//printf("writer thread release write lock before, readers:%d.!\n", pTask->m_tLocker.GetReaders());
			pTask->m_tLocker.UnLock();
			printf("writer thread release write lock.!\n");
			pTask->m_tLocker.GetReaders();
			//pTask->m_tLocker.UnLock();
			//OspSemGive(pTask->m_hQueueLock);
			//OspSemGive(pTask->m_hFull);

			//OspTaskDelay(1500);
		}
		//OspTaskDelay(1000);
	}
}

void* ConsumeProc(void* pParam)
{
	Task* pTask = (Task*)pParam;
	if (NULL == pTask)
	{
		printf("task is null, consume thread quit!\n");
		return 0;
	}

	while (pTask->IsConsumeWorking())
	{
		OspSemTake(pTask->m_hFull);
		//OspSemTake(pTask->m_hQueueLock);
		//pTask->m_tLocker.Lock();
// 		if (pTask->m_qQueue.empty())
// 		{//没有数据可以处理
// 			OspSemGive(pTask->m_hQueueLock);
// 			OspSemGive(pTask->m_hEmpty);
// 			OspTaskDelay(10);
// 			continue;
// 		}

		//处理接收队列头上的数据
		if (pTask->m_qQueue.empty() == false)
		{
			u32 data = pTask->m_qQueue.front();
			pTask->m_qQueue.pop();
			printf("pop data:%d.\n", data);
		}

		//pTask->m_tLocker.UnLock();
		//OspSemGive(pTask->m_hQueueLock);
		OspSemGive(pTask->m_hEmpty);

		OspTaskDelay(1000);
	}
}

void* ConsumeProc1(void* pParam)
{
	Task* pTask = (Task*)pParam;
	if (NULL == pTask)
	{
		printf("task is null, consume thread quit!\n");
		return 0;
	}
	
	while (pTask->IsConsumeWorking())
	{
		printf("task1 add readlock! before.\n");
		pTask->m_tLocker.GetReaders();
		pTask->m_tLocker.ReadLock();
		printf("task1 add readlock! after.\n");
		pTask->m_tLocker.GetReaders();
		//处理接收队列头上的数据
		if (pTask->m_qQueue.empty() == false)
		{
			u32 data = pTask->m_qQueue.back();

			printf("queue1 back data:%d.\n", data);
		}
		OspTaskDelay(1000);
		pTask->m_tLocker.UnLock();
		printf("task1 release readlock!\n");
		pTask->m_tLocker.GetReaders();
		
		//OspTaskDelay(500);
	}
}

void* ConsumeProc2(void* pParam)
{
	Task* pTask = (Task*)pParam;
	if (NULL == pTask)
	{
		printf("task is null, consume thread quit!\n");
		return 0;
	}
	
	while (pTask->IsConsumeWorking())
	{
		printf("task2 add readlock before.\n");
		pTask->m_tLocker.GetReaders();
		pTask->m_tLocker.ReadLock();
		printf("task2 add readlock after.\n");
		pTask->m_tLocker.GetReaders();
		//处理接收队列头上的数据
		if (pTask->m_qQueue.empty() == false)
		{
			u32 data = pTask->m_qQueue.back();
			
			printf("queue2 back data:%d.\n", data);
		}
		
		OspTaskDelay(1000);
		pTask->m_tLocker.UnLock();
		printf("task2 release readlock!\n");
		pTask->m_tLocker.GetReaders();
		
		//OspTaskDelay(500);
	}
}

void* ConsumeProc3(void* pParam)
{
	Task* pTask = (Task*)pParam;
	if (NULL == pTask)
	{
		printf("task is null, consume thread quit!\n");
		return 0;
	}
	
	while (pTask->IsConsumeWorking())
	{
		printf("task3 add readlock before.\n");
		pTask->m_tLocker.GetReaders();
		pTask->m_tLocker.ReadLock();
		printf("task3 add readlock after.\n");
		pTask->m_tLocker.GetReaders();

		//处理接收队列头上的数据
		if (pTask->m_qQueue.empty() == false)
		{
			u32 data = pTask->m_qQueue.back();
			
			printf("queue3 back data:%d.\n", data);
		}
		OspTaskDelay(1000);
		pTask->m_tLocker.UnLock();
		printf("task3 release readlock! \n");
		pTask->m_tLocker.GetReaders();

		//OspTaskDelay(500);
	}
}