/*=====================================================================================
模块名  ：线程池管理器
文件名  ：threadpool.h
相关文件：
实现功能：实现线程池管理
作者    ：包增辉(bao.z.h.2008@gmail.com)
版权    ：
---------------------------------------------------------------------------------------
修改记录：
日期        版本    修改人      走读人      修改记录
2015/3/25   V1.0    包增辉                    创建
========================================================================================*/


/************************************************************************/
/* 说明                                                                 */
/* 线程的数量会受系统的限制											    */
/* windows默认情况下一个单进程最大线程数为2000     						*/
/* linux下一个可能的最大线程数量3931。有一定的限制            			*/
/************************************************************************/
/************************************************************************/
// 使用方法如下所示
/*
CThreadsManager g_cThreadsManager;
// 实现具体的任务类(重写Run函数)
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
/* 常量定义                                                             */
/************************************************************************/
// 线程池中最少的线程个数
const u32 THREADS_POOL_SIZE = 20;
// 线程增长速度
const u8 THREADS_STEP_INC   = 10;
// 线程默认优先级
const u8  TASK_PRIORITY     = 90;
// 线程默认堆栈大小
const u32 TASK_STACK_SIZE   = 512*1024;
// 最大空闲次数(如果超过最大空闲次数，则释放该线程)
const u8  MAX_IDLE_TIMES    = 60;//0.5秒每次


/************************************************************************/
/* CTaskObject类                                                        */
/* 任务对象的基类，使用者需要集成该基类，并实现Run函数                  */
/************************************************************************/
class CTaskObject
{
public:
	virtual void Run( void ) = 0;
	virtual ~CTaskObject() {}
};


/************************************************************************/
/* CThreadInfo类                                                        */
/* 线程信息类，维护线程的状态等信息                                     */
/************************************************************************/
class CThreadInfo
{
public:
	CThreadInfo( void );
	~CThreadInfo();
	// 判断线程是否结束
	BOOL32 IsTerminateThread( void );
	// 判断当前是否有任务
	BOOL32 HasTask( void );
	// 终止线程
	void TerminateThread( void );
	// 获取任务对象
	CTaskObject* GetTask( void );
	// 设置任务对象(设置空，表示没有任务)
	void SetTask( CTaskObject* p );
	// 获取线程handle
	u32 GetHandle( void );
	// 设置线程handle
	void SetHandle( u32 dwHandle );
	// 获取线程Id
	u32 GetId( void );
	// 设置线程Id
	void SetId( u32 dwId );
	// 当前轮询空闲
	void IdleTime( void );
	// 设置空闲次数
	void SetIdleTimes( u8 by );
	// 获取空闲次数
	u8 GetIdleTimes( void );
private:
	// 任务对象
	CTaskObject* m_pTask;
	// 是否终止线程
	BOOL32 m_bIsTerminated;
	// 是否有任务
	BOOL32 m_bHasTask;
	// 空闲次数
	u8 m_byIdleTimes;//if > MAX_IDLE_TIMES, 释放
	// 线程id
	u32 m_dwId;
	// 线程handle
	u32 m_dwHandle;
};

/************************************************************************/
/* CThreadsManager类                                                    */
/* 线程池管理类，维护线程池的状态: 线程的创建、释放、分配等             */
/************************************************************************/
class CThreadsManager
{
public:
	CThreadsManager( void );
	~CThreadsManager();
	// 运行任务
	void RunTask( CTaskObject* pTaskObject );
	// 释放线程(根据线程handle)
	void ReleaseThread( u32 dwHandle );
	// 线程池同步锁，获取
	BOOL32 EnterLock( void );
	// 线程池同步锁，释放
	void LeaveLock( void );
	// 获取线程池大小
	u32 GetPoolSize( void );
	// 获取busy线程数目
	u32 GetBusyNum( void );
	// busy线程数-1
	void DecBusyNum( void );
public:
	// 用于比较线程handle大小的类
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
	// 创建线程
	void CreateThreads( u32 dwNum );
	// 释放所有线程
	void ReleaseAll( void );
private:
	// 同步信号量
	SEMHANDLE m_hSem;
	// 线程池大小
	u32 m_dwPoolSize;
	// busy的线程数目
	u32 m_dwBusyNum;
	// 线程池map(线程handle，线程信息对)
	typedef map<u32, CThreadInfo*, CHandleCompare> ThreadMap;
	ThreadMap m_ThreadMap;
	// 访问线程池map的迭代器
	//ThreadMap::iterator m_iter;//只用于查找空闲线程
	// 测试用
	u32 m_dwTestCount;
};

// 线程管理器的全局对象
extern CThreadsManager g_cThreadsManager;

#endif //_THREADS_MANAGER_H_

