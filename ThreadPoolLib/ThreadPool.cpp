#include "stdafx.h"



ThreadPool::ThreadPool(size_t _minthreads, size_t _maxthreads, size_t _lifetime)
		:tasks(),workers(),active(true)
{
	minthreads = _minthreads;
	maxthreads = _maxthreads;
	lifetime = _lifetime;
	InitializeCriticalSection(&taskscs);	
	InitializeCriticalSection(&workerscs);	
	thr_deleter_fn_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_deleter_fn, (LPVOID)this, 0, NULL);
	thr_manager_fn_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_manager_fn, (LPVOID)this, 0, NULL);
	//notNullTasksQueueEvent = ::CreateEvent(NULL,TRUE,//���
		//FALSE,NULL);
}

ThreadPool::~ThreadPool(void)
{
		active = false;
		::WaitForSingleObject(thr_deleter_fn_handle,INFINITE);
		::WaitForSingleObject(thr_manager_fn_handle,INFINITE);
		DeleteCriticalSection(&taskscs);
		DeleteCriticalSection(&workerscs);
}
void ThreadPool::thread_manager_fn(LPVOID param)
{
	bool foundfreeworker;
	ThreadPool* pool = (ThreadPool*) param;
	bool isallwork = false;
	fn_type task;
	while(pool->active)
	{
		isallwork = false;
		Sleep(20);//������ �������
		//::WaitForSingleObject(pool->notNullTasksQueueEvent,INFINITE);
		while (!isallwork)
		{
			EnterCriticalSection(&pool->taskscs);
			if (pool->tasks.empty())
			{
				isallwork = true;
				//ResetEvent(pool->notNullTasksQueueEvent);
				LeaveCriticalSection(&pool->taskscs);
				break;
			}
			//LeaveCriticalSection(&pool->taskscs);
			//EnterCriticalSection(&pool->taskscs);
			task = pool->tasks.front();
			pool->tasks.pop();
			if (pool->tasks.size() == 0)
			{
				//ResetEvent(pool->notNullTasksQueueEvent);
			}
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

void ThreadPool::thread_deleter_fn(LPVOID param)
{
	ThreadPool* pool = (ThreadPool*) param;
		
	while(pool->active)
	{
		Sleep(100);
		EnterCriticalSection(&pool->workerscs);
		DWORD curTime = GetTickCount64();
		if (pool->workers.size() > pool->minthreads)
		{
			for (int i = 0; i < pool->workers.size();i++)
				if (!pool->workers[i]->IsWorking())
					if (curTime - pool->workers[i]->GetLastFinishTime() > pool->lifetime)
					{
						pool->workers.erase( pool->workers.begin() + i);
					}
		}
		LeaveCriticalSection(&pool->workerscs);
	}
}

void ThreadPool::AddTask(DLLFUNC fn, LPVOID params)
{
	EnterCriticalSection(&taskscs);
	std::function<void()> f = std::function<void()>(std::bind(fn,(LPVOID)this,(LPVOID) params));
	tasks.push(f);
	LeaveCriticalSection(&taskscs);
}


int ThreadPool::WorkersCount(int* working, int* resting, int* itasks)
{
	int result = 0;
	EnterCriticalSection(&workerscs);
	for (int i =0; i< workers.size(); i++)
		if (workers[i]->IsWorking())
			result++;
	*working = result;
	*resting = workers.size() - result;
	LeaveCriticalSection(&workerscs);
	EnterCriticalSection(&taskscs);
	*itasks = this->tasks.size();
	LeaveCriticalSection(&taskscs);
	return *working + *resting;
}

void Exit(ThreadPool** thrplpt)
{
	exit(1);
}