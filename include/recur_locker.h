/*=====================================================================================
模块名  ：
文件名  ：recur_locker.h
相关文件：
实现功能：实现一个递归锁
作者    ：包增辉(bao.z.h.2008@gmail.com)
版权    ：
---------------------------------------------------------------------------------------
修改记录：
日期        版本    修改人      走读人      修改记录
2014/12/25   V1.0    包增辉                    创建
========================================================================================*/

#ifndef _RECUR_LOCKER_H_
#define _RECUR_LOCKER_H_

#include "type_def.h"
#include "locker.h"

#ifdef _LINUX_
	#define THREADID			 pthread_t
#elif defined _MSC_VER
	#define THREADID			 DWORD
#endif

typedef u32 (*TaskSelfFunc)(void);

#ifdef _LINUX_
	static u32 LinuxGetThreadID(void)
	{
		TASKHANDLE hTask = pthread_self();
		return (u32)(hTask);
	}
#elif defined _MSC_VER
	static u32 Win32GetThreadID(void)
	{
		return GetCurrentThreadId();
	}
#endif


class RecurLocker
{
public:
	RecurLocker()
	{
		//m_tLocker.Locker();
		m_dwRefCount = 0;
		m_tThreadID = NULL;

#ifdef _LINUX_
		m_tGetSelfID = LinuxGetThreadID;
#elif defined _MSC_VER
		m_tGetSelfID = Win32GetThreadID;
#endif
	}

	~RecurLocker()
	{
		m_dwRefCount = 0;
		m_tThreadID = NULL;
		m_tGetSelfID = NULL;
	}

	BOOL32 Lock()
	{
		if (m_dwRefCount == 0 && m_tLocker.Lock())    //没有加锁
		{
			m_dwRefCount++;
			m_tThreadID = m_tGetSelfID();
			return TRUE;
		}
		else if (m_dwRefCount > 0)   //已加锁
		{
			if (m_tGetSelfID() == m_tThreadID)   //如果是当前线程加锁
			{
				m_dwRefCount++;	
				return TRUE;
			}
			else
			{
				return m_tLocker.Lock();   //加锁，阻塞住
			}
		}
		return TRUE;
	}

	BOOL32 UnLock()
	{
		if (m_dwRefCount <= 0 || m_tGetSelfID() != m_tThreadID)
		{
			return TRUE;
		}

		if (m_tGetSelfID() == m_tThreadID)   //如果是当前线程加的锁
		{
			m_dwRefCount--;

			if (m_dwRefCount <= 0)
			{
				return m_tLocker.UnLock();
			}
			else
			{
				return TRUE;
			}
		}
	}

private:
	Locker m_tLocker;
	THREADID m_tThreadID;
	TaskSelfFunc m_tGetSelfID;
	u32 m_dwRefCount;
};

#endif
