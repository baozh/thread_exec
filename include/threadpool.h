/*=====================================================================================
ģ����  ���̳߳ع�����
�ļ���  ��threadpool.h
����ļ���
ʵ�ֹ��ܣ�ʵ���̳߳ع���
����    ��������(bao.z.h.2008@gmail.com)
��Ȩ    ��
---------------------------------------------------------------------------------------
�޸ļ�¼��
����        �汾    �޸���      �߶���      �޸ļ�¼
2015/3/25   V1.0    ������                    ����
========================================================================================*/


/************************************************************************/
/* ˵��                                                                 */
/* �̵߳���������ϵͳ������											    */
/* windowsĬ�������һ������������߳���Ϊ2000     						*/
/* linux��һ�����ܵ�����߳�����3931����һ��������            			*/
/************************************************************************/
/************************************************************************/
// ʹ�÷���������ʾ
/*
CThreadsManager g_cThreadsManager;
// ʵ�־����������(��дRun����)
class CTask : public CTaskObject
{
public:
void Run( void ) {}
};

  int main()
  {
  CTask cTask;
  g_cThreadsManager.RunTask( &cTask );
  return 0;
}
*/
/************************************************************************/


#ifndef _THREADS_MANAGER_H_
#define _THREADS_MANAGER_H_

#pragma warning(disable:4786)

#include "osp.h"
#include <map>
using namespace std;


/************************************************************************/
/* ��������                                                             */
/************************************************************************/
// �̳߳������ٵ��̸߳���
const u32 THREADS_POOL_SIZE = 20;
// �߳������ٶ�
const u8 THREADS_STEP_INC   = 10;
// �߳�Ĭ�����ȼ�
const u8  TASK_PRIORITY     = 90;
// �߳�Ĭ�϶�ջ��С
const u32 TASK_STACK_SIZE   = 512*1024;
// �����д���(������������д��������ͷŸ��߳�)
const u8  MAX_IDLE_TIMES    = 60;//0.5��ÿ��


/************************************************************************/
/* CTaskObject��                                                        */
/* �������Ļ��࣬ʹ������Ҫ���ɸû��࣬��ʵ��Run����                  */
/************************************************************************/
class CTaskObject
{
public:
	virtual void Run( void ) = 0;
	virtual ~CTaskObject() {}
};


/************************************************************************/
/* CThreadInfo��                                                        */
/* �߳���Ϣ�࣬ά���̵߳�״̬����Ϣ                                     */
/************************************************************************/
class CThreadInfo
{
public:
	CThreadInfo( void );
	~CThreadInfo();
	// �ж��߳��Ƿ����
	BOOL32 IsTerminateThread( void );
	// �жϵ�ǰ�Ƿ�������
	BOOL32 HasTask( void );
	// ��ֹ�߳�
	void TerminateThread( void );
	// ��ȡ�������
	CTaskObject* GetTask( void );
	// �����������(���ÿգ���ʾû������)
	void SetTask( CTaskObject* p );
	// ��ȡ�߳�handle
	u32 GetHandle( void );
	// �����߳�handle
	void SetHandle( u32 dwHandle );
	// ��ȡ�߳�Id
	u32 GetId( void );
	// �����߳�Id
	void SetId( u32 dwId );
	// ��ǰ��ѯ����
	void IdleTime( void );
	// ���ÿ��д���
	void SetIdleTimes( u8 by );
	// ��ȡ���д���
	u8 GetIdleTimes( void );
private:
	// �������
	CTaskObject* m_pTask;
	// �Ƿ���ֹ�߳�
	BOOL32 m_bIsTerminated;
	// �Ƿ�������
	BOOL32 m_bHasTask;
	// ���д���
	u8 m_byIdleTimes;//if > MAX_IDLE_TIMES, �ͷ�
	// �߳�id
	u32 m_dwId;
	// �߳�handle
	u32 m_dwHandle;
};

/************************************************************************/
/* CThreadsManager��                                                    */
/* �̳߳ع����࣬ά���̳߳ص�״̬: �̵߳Ĵ������ͷš������             */
/************************************************************************/
class CThreadsManager
{
public:
	CThreadsManager( void );
	~CThreadsManager();
	// ��������
	void RunTask( CTaskObject* pTaskObject );
	// �ͷ��߳�(�����߳�handle)
	void ReleaseThread( u32 dwHandle );
	// �̳߳�ͬ��������ȡ
	BOOL32 EnterLock( void );
	// �̳߳�ͬ�������ͷ�
	void LeaveLock( void );
	// ��ȡ�̳߳ش�С
	u32 GetPoolSize( void );
	// ��ȡbusy�߳���Ŀ
	u32 GetBusyNum( void );
	// busy�߳���-1
	void DecBusyNum( void );
public:
	// ���ڱȽ��߳�handle��С����
	class CHandleCompare
    {
	public:
        bool operator()(u32 handle1, u32 handle2) const
        {
            if (handle1 < handle2)
			{
				return TRUE;
			}
			
			return FALSE;
        }
    };
private:
	// �����߳�
	void CreateThreads( u32 dwNum );
	// �ͷ������߳�
	void ReleaseAll( void );
private:
	// ͬ���ź���
	SEMHANDLE m_hSem;
	// �̳߳ش�С
	u32 m_dwPoolSize;
	// busy���߳���Ŀ
	u32 m_dwBusyNum;
	// �̳߳�map(�߳�handle���߳���Ϣ��)
	typedef map<u32, CThreadInfo*, CHandleCompare> ThreadMap;
	ThreadMap m_ThreadMap;
	// �����̳߳�map�ĵ�����
	//ThreadMap::iterator m_iter;//ֻ���ڲ��ҿ����߳�
	// ������
	u32 m_dwTestCount;
};

// �̹߳�������ȫ�ֶ���
extern CThreadsManager g_cThreadsManager;

#endif //_THREADS_MANAGER_H_

