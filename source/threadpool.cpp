
#include "threadpool.h"

static u32 g_Count		= 0;
static u32 g_FindCount	= 0;
static u32 g_CreateFail	= 0;

CThreadInfo::CThreadInfo( void )
{
	m_pTask			= NULL;
	m_bIsTerminated = FALSE;
	m_bHasTask		= FALSE;
	m_byIdleTimes	= 0;
	m_dwId			= 0;
	m_dwHandle		= 0;
}

CThreadInfo::~CThreadInfo()
{
}

BOOL32 CThreadInfo::IsTerminateThread( void )
{
	return m_bIsTerminated;
}

BOOL32 CThreadInfo::HasTask( void )
{
	return m_bHasTask;
}

CTaskObject* CThreadInfo::GetTask( void )
{
	return m_pTask;
}

void CThreadInfo::SetTask( CTaskObject* p )
{
	if (NULL == p)
	{
		m_pTask = NULL;
		m_bHasTask = FALSE;
	}
	else
	{
		m_pTask = p;
		m_bHasTask = TRUE;
	}
}

void CThreadInfo::TerminateThread( void )
{
	m_bIsTerminated = TRUE;
}

u32 CThreadInfo::GetId( void )
{
	return m_dwId;
}

void CThreadInfo::SetId( u32 dwId )
{
	m_dwId = dwId;
}

u32 CThreadInfo::GetHandle( void )
{
	return m_dwHandle;
}

void CThreadInfo::SetHandle( u32 dwHandle )
{
	m_dwHandle = dwHandle;
}

u8 CThreadInfo::GetIdleTimes( void )
{
	return m_byIdleTimes;
}

void CThreadInfo::SetIdleTimes( u8 by )
{
	m_byIdleTimes = by;
}

void CThreadInfo::IdleTime( void )
{
	++m_byIdleTimes;
}

void* ThreadsTaskProc(void* pParam)
{
	CThreadInfo* pInfo = (CThreadInfo*)pParam;
	while(FALSE == pInfo->IsTerminateThread())
	{
		if (TRUE == pInfo->HasTask())
		{
			CTaskObject* pTask = pInfo->GetTask();
			pTask->Run();
			pInfo->SetTask( NULL );
			pInfo->SetIdleTimes( 0 );
			delete pTask;

			g_cThreadsManager.DecBusyNum();
		}
		else
		{
			pInfo->IdleTime();
			if (pInfo->GetIdleTimes() > MAX_IDLE_TIMES)
			{
				if (FALSE == g_cThreadsManager.EnterLock())
				{
					return 0;
				}
				
				if (g_cThreadsManager.GetPoolSize() > THREADS_POOL_SIZE && FALSE == pInfo->HasTask())
				{
					g_cThreadsManager.ReleaseThread( pInfo->GetHandle() );
				}
				else
				{
					pInfo->SetIdleTimes( 0 );
					//u32 dwPoolSize = g_cThreadsManager.GetPoolSize();
					//u32 dwBusyNum  = g_cThreadsManager.GetBusyNum();
					/*printf("CreateCount:%d, BusyThreads:%d, PoolSize:%d, FindCount:%d, CreateThreadsFail:%d\n", 
						g_Count, dwBusyNum, dwPoolSize, g_FindCount, g_CreateFail);*/
				}
				
				g_cThreadsManager.LeaveLock();
			}
			
			OspDelay( 500 );//0.5秒
		}
	}
	
	delete pInfo;
	return 0;
}

/************************************************************************/
/* CThreadsManager                                                      */
/************************************************************************/
CThreadsManager::CThreadsManager( void )
{
	m_dwPoolSize	= 0;
	m_dwBusyNum		= 0;
	m_dwTestCount	= 0;
	CreateThreads( THREADS_POOL_SIZE );
	OspSemBCreate(&m_hSem);
//	m_iter = m_ThreadMap.begin();
}

CThreadsManager::~CThreadsManager()
{
	ReleaseAll();
	OspSemDelete(m_hSem);
}

void CThreadsManager::RunTask( CTaskObject* pTaskObject )
{
	if (FALSE == EnterLock())
	{
		return;
	}
	
	//float fBusyNum = m_dwBusyNum * 100 / THRESHOLD_VALUE;
	if ( m_dwBusyNum<<1 >= m_dwPoolSize )
	{
		CreateThreads( THREADS_STEP_INC );
	}
	
	BOOL32 bFind = FALSE;
	// 直接从上一次的下一个开始查找
//	++m_iter;
//	for (; m_iter != m_ThreadMap.end(); ++m_iter)
//	{
//		CThreadInfo* pInfo = m_iter->second;
//		if (FALSE == pInfo->HasTask())
//		{
//			pInfo->SetTask( pTaskObject );
//			bFind = TRUE;
//			break;
//		}
//		++g_FindCount;
//	}

	// 如果没有找到，则从头开始找
//	if (FALSE == bFind)
//	{
    ThreadMap::iterator iter;
		for (iter = m_ThreadMap.begin(); iter != m_ThreadMap.end(); ++iter)
		{
			CThreadInfo* pInfo = iter->second;
			if (FALSE == pInfo->HasTask())
			{
				pInfo->SetTask( pTaskObject );
				break;
			}
			++g_FindCount;
		}
//	}

	LeaveLock();

	++m_dwBusyNum;
}

void CThreadsManager::ReleaseThread( u32 dwHandle )
{
	ThreadMap::iterator iter;
	iter = m_ThreadMap.find( dwHandle );
	if (iter != m_ThreadMap.end())
	{
		CThreadInfo* pInfo = (CThreadInfo*)iter->second;
		pInfo->TerminateThread();
		m_ThreadMap.erase( iter );
	}

	--m_dwPoolSize;
}

void CThreadsManager::ReleaseAll( void )
{
	ThreadMap::iterator iter;
	for (iter = m_ThreadMap.begin(); iter != m_ThreadMap.end(); ++iter)
	{
		CThreadInfo* pInfo = iter->second;
		delete pInfo;
	}

	m_ThreadMap.clear();
}

u32 CThreadsManager::GetPoolSize( void )
{
	return m_dwPoolSize;
}

void CThreadsManager::DecBusyNum( void )
{
	--m_dwBusyNum;
}

u32 CThreadsManager::GetBusyNum( void )
{
	return m_dwBusyNum;
}

BOOL32 CThreadsManager::EnterLock( void )
{
	if (NULL != m_hSem)
	{
		return OspSemTake(m_hSem);
	}

	return FALSE;
}

void CThreadsManager::LeaveLock( void )
{
	if (NULL != m_hSem)
	{
		OspSemGive(m_hSem);
	}
}

void CThreadsManager::CreateThreads( u32 dwNum )
{
	++g_Count;
	u32 hTaskHandle = NULL;
	u32 dwTaskId = 0;
	for( u32 i = 0; i < dwNum; ++i )
	{
		CThreadInfo* pInfo = new CThreadInfo;
		hTaskHandle = (u32)OspTaskCreate(ThreadsTaskProc, "CThreadsManager", TASK_PRIORITY, TASK_STACK_SIZE, (u32)pInfo, 0, &dwTaskId);
		if (NULL == hTaskHandle)
		{
			printf("OspTaskCreate Fail!\n");
			delete pInfo;
			++g_CreateFail;
			return;
		}
		
		++m_dwPoolSize;
		++m_dwTestCount;
		pInfo->SetHandle( hTaskHandle );
		pInfo->SetId( m_dwTestCount );
		//pInfo->SetId( dwTaskId );
		
		m_ThreadMap.insert(ThreadMap::value_type(hTaskHandle, pInfo));
	}

	OspDelay( 500 );
}


