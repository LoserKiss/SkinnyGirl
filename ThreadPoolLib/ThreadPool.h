#pragma once
#include "stdafx.h"
class ThreadPool
{
public:

	public:
    ThreadPool(size_t _minthreads, size_t _maxthreads, size_t _lifetime)
		:tasks(),workers(),active(true)
    {
		minthreads = _minthreads;
		maxthreads = _maxthreads;
		lifetime = _lifetime;
		InitializeCriticalSection(&taskscs);	
		InitializeCriticalSection(&workerscs);	
		thr_deleter_fn_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_deleter_fn, (LPVOID)this, 0, NULL);
		thr_manager_fn_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_manager_fn, (LPVOID)this, 0, NULL);
		notNullTasksQueueEvent = ::CreateEvent(NULL,TRUE,//без
			FALSE,NULL);
	}
    ~ThreadPool() 
	{
		active = false;
		::WaitForSingleObject(thr_deleter_fn_handle,INFINITE);
		::WaitForSingleObject(thr_manager_fn_handle,INFINITE);
		DeleteCriticalSection(&taskscs);
		DeleteCriticalSection(&workerscs);
	}
	void AddTask(LPVOID fn, std::vector<LPVOID> params)
	{
		bool isempty;
		EnterCriticalSection(&taskscs);
		isempty = tasks.empty();
		tasks.push(std::bind(fn,params));
		if (isempty)
		{
			SetEvent(notNullTasksQueueEvent);
		}
		LeaveCriticalSection(&taskscs);
	}
private:
	size_t minthreads;
	size_t maxthreads;
	size_t lifetime;
	bool active;
	HANDLE thr_deleter_fn_handle;
	HANDLE thr_manager_fn_handle;
	static void thread_deleter_fn(LPVOID param)
	{
		ThreadPool* pool = (ThreadPool*) param;
		
		while(pool->active)
		{
			Sleep(pool->lifetime);
			DWORD curTime = GetTickCount64();
			EnterCriticalSection(&pool->workerscs);
			if (pool->workers.size() > pool->minthreads)
			{
				for (int i = 0; i < pool->workers.size();pool++)
					if (!pool->workers[i]->IsWorking())
						if (curTime - pool->workers[i]->GetLastFinishTime() > pool->lifetime)
						{
							pool->workers.erase( pool->workers.begin() + i);
						}
			}
			LeaveCriticalSection(&pool->workerscs);
		}
	}
	static void thread_manager_fn(LPVOID param)
	{
		bool foundfreeworker;
		ThreadPool* pool = (ThreadPool*) param;
		bool isallwork = false;
		fn_type task;
		while(pool->active)
		{
			isallwork = false;
			Sleep(100);//плохое решение
			::WaitForSingleObject(pool->notNullTasksQueueEvent,INFINITE);
			while (!isallwork)
			{
				EnterCriticalSection(&pool->taskscs);
				if (pool->tasks.empty())
				{
					isallwork = true;
					ResetEvent(pool->notNullTasksQueueEvent);
					break;
				}
				LeaveCriticalSection(&pool->taskscs);
				EnterCriticalSection(&pool->taskscs);
				task = pool->tasks.front();
				pool->tasks.pop();
				LeaveCriticalSection(&pool->taskscs);
				EnterCriticalSection(&pool->workerscs);
				foundfreeworker = false;
				for(int i = 0; i< pool->workers.size(); i++)
				{
					if (!pool->workers[i]->IsWorking())
					{
						foundfreeworker = true;
						pool->workers[i]->GiveTask(task);
						break;
					}
				}
				if (!foundfreeworker)
				{
					if (pool->workers.size() < pool->maxthreads)
					{
						pool->workers.push_back( new Worker());
						pool->workers.back()->GiveTask(task);
					}
				}
				LeaveCriticalSection(&pool->workerscs);
			}
		}
	}
	CRITICAL_SECTION workerscs;
    std::vector<Worker*> workers; 
	CRITICAL_SECTION taskscs;
	HANDLE notNullTasksQueueEvent;
	std::queue<fn_type> tasks; 
};

