#pragma once
#include "stdafx.h"
#include "Worker.h"
 class  ThreadPool
{
public:
	public:
    ThreadPool(size_t _minthreads, size_t _maxthreads, size_t _lifetime);
    ~ThreadPool() ;
	void AddTask(DLLFUNC fn, LPVOID params);
	int WorkersCount(int* working, int* resting, int* tasks);
private:
	bool active;
	size_t minthreads;
	size_t maxthreads;
	size_t lifetime;
	
	HANDLE thr_deleter_fn_handle;
	HANDLE thr_manager_fn_handle;
	static void thread_deleter_fn(LPVOID param);
	static void thread_manager_fn(LPVOID param);
	CRITICAL_SECTION workerscs;
    std::vector<Worker*> workers; 
	CRITICAL_SECTION taskscs;
	//HANDLE notNullTasksQueueEvent;
	std::queue<fn_type> tasks; 
};



void Exit(ThreadPool** thrplpt);