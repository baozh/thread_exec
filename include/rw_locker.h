/*=====================================================================================
ģ����  ��
�ļ���  ��rw_locker.h
����ļ���
ʵ�ֹ��ܣ�ʵ��һ����д��
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
---------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2014/12/26   V1.0    ������                    ����
========================================================================================*/

#ifndef _RW_LOCKER_H_
#define _RW_LOCKER_H_

#include "type_def.h"
#include "locker.h"

typedef enum _RwLockerMode
{
	RW_LOCKER_NONE,
	RW_LOCKER_WR,
	RW_LOCKER_RD,
	RW_LOCKER_NR
}RwLockerMode;

class RwLocker
{
public:
	RwLocker()
	{
		m_enMode = RW_LOCKER_NONE;
		m_dwReaders = 0;
	}

	~RwLocker()
	{
		m_enMode = RW_LOCKER_NONE;
		m_dwReaders = 0;
	}

	BOOL32 ReadLock()
	{
		if (m_enMode == RW_LOCKER_WR)
		{
			BOOL32 bRet = m_tLocker.Lock();
			if (bRet == TRUE)
			{
				m_tReadersLocker.Lock();
				m_dwReaders++;
				m_tReadersLocker.UnLock();
				m_enMode = RW_LOCKER_RD;
			}
			return bRet;
		}
		else if(m_enMode == RW_LOCKER_RD)
		{
			m_tReadersLocker.Lock();
			m_dwReaders++;
			m_tReadersLocker.UnLock();
			return TRUE;
		}
		else    //û�м���
		{
			BOOL32 bRet = m_tLocker.Lock();
			if (bRet == TRUE)
			{
				m_tReadersLocker.Lock();
				m_dwReaders++;
				m_tReadersLocker.UnLock();
				m_enMode = RW_LOCKER_RD;
			}
			return bRet;
		}
	}

	BOOL32 WriteLock()
	{
		if (m_enMode == RW_LOCKER_WR)
		{
			return m_tLocker.Lock();
		}
		else if(m_enMode == RW_LOCKER_RD)
		{
			BOOL32 bRet = m_tLocker.Lock();
			if (bRet == TRUE)
			{
				m_enMode = RW_LOCKER_WR;
			}
			return bRet;
		}
		else    //û�м���
		{
			BOOL32 bRet = m_tLocker.Lock();
			if (bRet == TRUE)
			{
				m_enMode = RW_LOCKER_WR;
			}
			return bRet;
		}
	}

	BOOL32 UnLock()
	{
		if (m_enMode == RW_LOCKER_WR)
		{
			BOOL32 bRet = m_tLocker.UnLock();
			if (bRet == TRUE)
			{
				m_enMode = RW_LOCKER_NONE;
			}
			return bRet;
		}
		else if(m_enMode == RW_LOCKER_RD)
		{
			BOOL32 bRet = TRUE;
			m_tReadersLocker.Lock();
			m_dwReaders--;
			if (m_dwReaders <= 0)
			{
				bRet = m_tLocker.UnLock();
				if (bRet == TRUE)
				{
					m_enMode = RW_LOCKER_NONE;
				}
			}
			m_tReadersLocker.UnLock();
			return bRet;
		}
		return TRUE;
	}

	void GetReaders()
	{
		m_tReadersLocker.Lock();
		printf("readers:%d.\n", m_dwReaders);
		m_tReadersLocker.UnLock();
	}
private:
	Locker m_tLocker;   //������
	Locker m_tReadersLocker;   //��������m_dwReaders
	u32 m_dwReaders;
	RwLockerMode m_enMode;
};

#endif
