/*=====================================================================================
ģ����  ��
�ļ���  ��locker.h
����ļ���
ʵ�ֹ��ܣ�ʵ��һ����(��װlinux��windows�ϵ���api)
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
---------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2014/12/6   V1.0    ������                    ����
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
		s32 ret = ReleaseMutex(m_tMutex);  //ʧ�ܷ���0
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
