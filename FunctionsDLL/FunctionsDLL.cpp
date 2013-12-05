// FunctionsDLL.cpp: определяет экспортированные функции для приложения DLL.
//
#include "stdafx.h"

void __declspec(dllexport) List(LPVOID poolarg, LPVOID vectargs)
{
	ThreadPool* pool =(ThreadPool*) poolarg;
	std::vector<std::string>* vect = (std::vector<std::string>*) vectargs;

	WIN32_FIND_DATAW wfd;
	LPWSTR dir = (LPWSTR) vect[0].data()->data();
    HANDLE  hFind = FindFirstFileW(dir, &wfd);
	std::string ext;
	char* vasya = new char[MAX_PATH];
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
			if (wfd.cFileName !=L"." && wfd.cFileName !=L"..")
			{
				std::cout<<  wfd.cFileName;
			}
        } while (NULL != FindNextFileW(hFind, &wfd));
 
        FindClose(hFind);
    }
}
void __declspec(dllexport) Status(LPVOID poolarg, LPVOID vectargs)
{
	ThreadPool* pool =(ThreadPool*) poolarg;
	std::vector<std::string>* vect = (std::vector<std::string>*) vectargs;

	int working;
	int resting;
	pool->WorkersCount(&working,&resting);
	std::cout<<"Working: "<<working<<" Resting: "<< resting;
}
void __declspec(dllexport) Copy(LPVOID poolarg, LPVOID vectargs)
{
	ThreadPool* pool =(ThreadPool*) poolarg;
	std::vector<std::string>* vect = (std::vector<std::string>*) vectargs;

}
void __declspec(dllexport) Size(LPVOID poolarg, LPVOID vectargs)
{
	ThreadPool* pool =(ThreadPool*) poolarg;
	std::vector<std::string>* vect = (std::vector<std::string>*) vectargs;

}