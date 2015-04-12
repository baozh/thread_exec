/*=====================================================================================
模块名  ：
文件名  ：locker.h
相关文件：
实现功能：实现一个锁(封装linux和windows上的锁api)
作者    ：包增辉(bao.z.h.2008@gmail.com)
版权    ：
---------------------------------------------------------------------------------------
修改记录：
日期        版本    修改人      走读人      修改记录
2014/12/6   V1.0    包增辉                    创建
========================================================================================*/

#ifndef _NORMAL_LOCKER_H_
#define _NORMAL_LOCKER_H_

#include "type_def.h"

#ifdef _LINUX_
#include <pthread.h>
#endif

#ifdef _MSC_VER
#include <process.h>
#endif

class Locker
{
public:
	Locker()
	{
#ifdef _LINUX_
		pthread_mutex_init(&m_tMutex, NULL);
#endif
#ifdef _MSC_VER
		m_tMutex = CreateMutex(NULL, FALSE, "aaa");
#endif
	};

	~Locker()
	{
#ifdef _LINUX_
		s32 ret = pthread_mutex_destroy(&m_tMutex);
#endif
		 	
#ifdef _MSC_VER
		if (m_tMutex != NULL)
		{
			CloseHandle(m_tMutex);
			m_tMutex = NULL;
		}
#endif
	};

	BOOL32 Lock()
	{
		BOOL32 bResult = FALSE;
#ifdef _LINUX_
		s32 ret = pthread_mutex_lock(&m_tMutex);
		bResult = ret == 0 ? TRUE : FALSE;
#endif
	
#ifdef _MSC_VER
		s32 ret = WaitForSingleObject(m_tMutex,INFINITE);
		bResult = ret == WAIT_FAILED ? FALSE : TRUE;
#endif
		return bResult;
	};

	BOOL32 UnLock()
	{
		BOOL32 bResult = FALSE;
		
#ifdef _LINUX_
		s32 ret = pthread_mutex_unlock(&m_tMutex);
		bResult = ret == 0 ? TRUE : FALSE;
#endif
		
#ifdef _MSC_VER
		s32 ret = ReleaseMutex(m_tMutex);  //失败返回0
		bResult = ret == 0 ? FALSE : TRUE;
#endif
		return bResult;
	};

private:
#ifdef _LINUX_
	pthread_mutex_t m_tMutex;
#endif
	
#ifdef _MSC_VER
	HANDLE m_tMutex;
#endif
};

#endif
