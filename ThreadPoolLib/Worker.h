#pragma once
#include "stdafx.h"
class Worker
{
public:

    Worker()
		:enabled(true),isworking(false)
    {
		lasttime = GetTickCount64();
		thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_fn, (LPVOID)this, 0, NULL);
		InitializeCriticalSection(&cs);
		awaker = ::CreateEvent(NULL,FALSE,//с автосбросом
			FALSE,NULL);
		finishtask = ::CreateEvent(NULL,FALSE,//с автосбросом
			TRUE,NULL);
	}
    ~Worker()
    {
		EnterCriticalSection(&cs);
		enabled = false;
		isworking = false;
		LeaveCriticalSection(&cs);
		SetEvent(awaker);
		::WaitForSingleObject(thread,INFINITE);
		DeleteCriticalSection(&cs);
		CloseHandle(awaker);
		CloseHandle(finishtask);
    }
	DWORD GetLastFinishTime()
	{
		DWORD lstm;
		EnterCriticalSection(&cs);
		lstm = lasttime;
		LeaveCriticalSection(&cs);
		return lstm;
	}
	bool IsWorking()
	{
		bool ret;
		EnterCriticalSection(&cs);
		ret = isworking;
		LeaveCriticalSection(&cs);
		return ret;
	}
	int GiveTask(fn_type task)
	{
		if (!enabled)
			return -1;
		EnterCriticalSection(&cs);
		if (!isworking)
		{
			task_field = task;
			isworking = true;
			SetEvent(awaker);
		}
		else
		{
			LeaveCriticalSection(&cs);
			::WaitForSingleObject(finishtask,INFINITE);
			EnterCriticalSection(&cs);
			task_field = task;
			isworking = true;
			SetEvent(awaker);
		}
		LeaveCriticalSection(&cs);
		return 0;
	}
private:
	fn_type task_field;
	bool isworking;
	DWORD lasttime;
	CRITICAL_SECTION cs;
	HANDLE awaker;//событие 
	HANDLE finishtask; //событие окончания работы
	HANDLE thread;
	bool enabled;
    static void thread_fn(LPVOID param)
	{
		Worker* ths = (Worker*) param;
		while (ths->enabled)
		{
			::WaitForSingleObject(ths->awaker,INFINITE);
			EnterCriticalSection(&ths->cs);
				if (!ths->enabled)
				{
					LeaveCriticalSection(&ths->cs);
					return;
				}
				ths->isworking = true;
				ResetEvent(ths->finishtask);
			LeaveCriticalSection(&ths->cs);
			if (ths->task_field != NULL)
			{
				ths->task_field();
			}
			EnterCriticalSection(&ths->cs);
				ths->lasttime = GetTickCount64();
				ths->isworking = false;
				SetEvent(ths->finishtask);
			LeaveCriticalSection(&ths->cs);
		}
	}
};
typedef std::shared_ptr<Worker> worker_ptr;

