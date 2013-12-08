// FunctionsDLL.cpp: определяет экспортированные функции для приложения DLL.
//
#include "stdafx.h"

extern "C"
{
	void __declspec(dllexport) List(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;

		WIN32_FIND_DATAW wfd;
		std::wstring dir = const_cast<WCHAR*>(vect[0].data()->data());
		if (wcsncmp(&dir.back(),L"/",1))
		{
			dir.push_back(L'//');
			dir.push_back(L'*');
		}
		else
			dir.push_back(L'*');
		HANDLE  hFind = FindFirstFileW(dir.data(), &wfd);
		std::string ext;
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				if (wcsncmp(wfd.cFileName,L".",1) && wcsncmp(wfd.cFileName ,L"..",2))
				{
					wprintf(wfd.cFileName);
					wprintf(L"\n");
				}
			} while (NULL != FindNextFileW(hFind, &wfd));
 
			FindClose(hFind);
		}
	}
	void __declspec(dllexport) Status(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		int working = pool->Vasya();;//просто так
		int resting = 0;
		pool->WorkersCount(&working,&resting);
		std::cout<<"Working: "<<working<<" Resting: "<< resting;
	}
	void __declspec(dllexport) Copy(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		std::vector<std::wstring>* recvect;
		WIN32_FIND_DATAW wfd;
		HANDLE  hFind = FindFirstFileW(vect->at(0).data(), &wfd);
		std::string ext;
		std::vector<std::wstring> vasya;
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				if(wfd.cFileName != L"." && wfd.cFileName != L"..")
				{
					vasya.push_back(wfd.cFileName);
				}
			} while (NULL != FindNextFileW(hFind, &wfd));
			FindClose(hFind);
			for (int i = 0; i< vasya.size(); i++)
			{
				//if (PathIsDirectory(vasya[i].data()))
				{
					recvect = new std::vector<std::wstring>();
					recvect->push_back(vasya[i]);
					recvect->push_back(vect->at(1));
					pool->AddTask(&Copy,recvect);
				}
				vasya.erase(vasya.begin() + i);
				i--;
			}
			for (int i = 0; i< vasya.size(); i++)
			{
				//CopyFile();
			}

		}
	}
	void __declspec(dllexport) Size(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::string>* vect = (std::vector<std::string>*) vectargs;


	}
}