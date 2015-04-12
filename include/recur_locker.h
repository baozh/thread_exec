/*=====================================================================================
ģ����  ��
�ļ���  ��recur_locker.h
����ļ���
ʵ�ֹ��ܣ�ʵ��һ���ݹ���
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
---------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2014/12/25   V1.0    ������                    ����
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
		if (m_dwRefCount == 0 && m_tLocker.Lock())    //û�м���
		{
			m_dwRefCount++;
			m_tThreadID = m_tGetSelfID();
			return TRUE;
		}
		else if (m_dwRefCount > 0)   //�Ѽ���
		{
			if (m_tGetSelfID() == m_tThreadID)   //����ǵ�ǰ�̼߳���
			{
				m_dwRefCount++;	
				return TRUE;
			}
			else
			{
				return m_tLocker.Lock();   //����������ס
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

		if (m_tGetSelfID() == m_tThreadID)   //����ǵ�ǰ�̼߳ӵ���
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
