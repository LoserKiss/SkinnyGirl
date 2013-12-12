// FunctionsDLL.cpp: определяет экспортированные функции для приложения DLL.
//
#include "stdafx.h"

extern "C"
{
	void NormalizeDirPath(std::wstring* dir)
	{
		if (wcsncmp(&dir->back(),L"/",1))
		{
			dir->push_back(L'//');
			dir->push_back(L'*');
		}
		else
			dir->push_back(L'*');
	}
	void __declspec(dllexport) List(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		if (vect->size() != 1)
		{
			printf("Size:Unacceptable params!\n)");
			return;
		}
		WIN32_FIND_DATAW wfd;
		std::wstring dir = const_cast<WCHAR*>(vect[0].data()->data());
		NormalizeDirPath(&dir);
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
		if (vect->size() != 0)
		{
			printf("Size:Unacceptable params!\n)");
			return;
		}
		int working= 0;//просто так
		int resting = 0;
		int tasks = 0;
		pool->WorkersCount(&working,&resting, &tasks);
		std::cout<<"Working: "<<working<<" Resting: "<< resting <<
			" Tasks: "<< tasks << "\n";
	}
	#pragma warning(disable : 4996)
	void __declspec(dllexport) Copy(LPVOID poolarg, LPVOID vectargs)
	 {
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		if (vect->size() != 2)
		{
			printf("Copy:Unacceptable params!\n)");
			return;
		}
		WIN32_FIND_DATAW wfd;
		std::wstring dirFrom = std::wstring(vect->at(0));
		std::wstring dirTo= std::wstring(vect->at(1));
		std::wstring folderName;
		if (dirFrom.back() == L'/')
		{
			dirFrom.pop_back();
		}
		folderName = dirFrom.substr(dirFrom.find_last_of(L"/")+ 1);
		NormalizeDirPath(&dirFrom);
		NormalizeDirPath(&dirTo);
		dirTo.pop_back();// * не нужна
		dirTo += folderName;
		dirTo+= L"/";
		HANDLE  hFind = FindFirstFileW(dirFrom.data(), &wfd);
		std::wstring fullFileName;
		std::wstring fullNewFileName;
		fullNewFileName.clear();
		fullNewFileName+= dirTo;
		CreateDirectory(fullNewFileName.data(),NULL);
		std::vector<std::wstring> vasya;
		std::wstring* newPathTo;
		std::wstring* newPathFrom;
		std::vector<std::wstring>* newVectParams;
		int vas;
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				if (!wcscmp(wfd.cFileName,L".") || !wcscmp(wfd.cFileName,L".."))
					 continue;
					if ((vas = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
					{
						//если директория
						WCHAR newpath[_MAX_PATH];
						newpath[0] = L'\0';
						wcscat(newpath,dirTo.data());
						newPathTo = new std::wstring(newpath);
						wcscpy(newpath,L"");
						wcscat(newpath,dirFrom.data());
						newpath[wcslen(newpath)-1] = L'\0';
						wcscat(newpath,wfd.cFileName);
						newPathFrom = new std::wstring(newpath);
						newVectParams = new std::vector<std::wstring>();
						newVectParams->push_back(*newPathFrom);
						newVectParams->push_back(*newPathTo);
						pool->AddTask(Copy,newVectParams);
					}
					else
					{
						vasya.push_back(wfd.cFileName);// список файлов на копирование
					}
				
			} while (NULL != FindNextFileW(hFind, &wfd));
			FindClose(hFind);
			for (int i = 0; i< vasya.size(); i++)
			{
				fullFileName.clear();
				fullFileName+= dirFrom;
				fullFileName.pop_back(); //удаляем *
				fullFileName+=vasya[i];
				fullNewFileName.clear();
				fullNewFileName+= dirTo;
				fullNewFileName+=vasya[i];
				int err = CopyFile(fullFileName.data(),fullNewFileName.data(),FALSE);
				if (err == 0)
				{
					std::wcout << L"Failed to copy "<<fullFileName.data()<<L"\n"<<std::endl;
				}
			}
		}
		delete vect;
	}
	class DefendedInt64
	{
	public:
		DefendedInt64()
		{
			InitializeCriticalSection(&cs);
			val = 0;
			count = 0;
		}
		~DefendedInt64()
		{
			DeleteCriticalSection(&cs);
		}
		int64_t GetInt64()
		{
			int64_t vasya;
			EnterCriticalSection(&cs);
			vasya = val;
			LeaveCriticalSection(&cs);
			return vasya;
		}
		void SetInt64(int64_t value)
		{
			EnterCriticalSection(&cs);
			val = value;
			LeaveCriticalSection(&cs);
		}
		void IncreaseInt64(int64_t value)
		{
			EnterCriticalSection(&cs);
			val += value;
			count++;
			LeaveCriticalSection(&cs);
		}
		int count;
	private:
		
		CRITICAL_SECTION cs;
		int64_t val;
	};
	void FolderSize(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<LPVOID>* vect = (std::vector<LPVOID>*) vectargs;
		if (vect->size() != 3)
		{
			printf("WRONGWRONGWRONGWRONGWRONGWRONG\n");
		}
		WIN32_FIND_DATAW wfd;
		std::wstring* pdir = (std::wstring*) vect->at(0);
		std::wstring dir = *pdir;
		if (wcscmp(&dir.back(),L"*"))
		{
			dir.push_back(L'*');
		}
		HANDLE  hFind = FindFirstFileW(dir.data(), &wfd);
		int64_t folderFilesSize = 0;
		int vas;
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				if (!wcscmp(wfd.cFileName,L".") || !wcscmp(wfd.cFileName,L".."))
					 continue;
				if ((vas = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					folderFilesSize += (wfd.nFileSizeHigh * (MAXDWORD+1)) + wfd.nFileSizeLow;
				}
			} while (NULL != FindNextFileW(hFind, &wfd));
			FindClose(hFind);
			DefendedInt64* vasya;
			vasya = (DefendedInt64*) vect->at(1);
			vasya->IncreaseInt64(folderFilesSize);
		}
		HANDLE eventik = (HANDLE) vect->at(2);
		SetEvent(eventik);
		//delete pdir;
		//delete vect;
	}
	#pragma warning(disable : 4996)
	void FolderWalker(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<LPVOID> vect = *(std::vector<LPVOID>*) vectargs;
		WIN32_FIND_DATAW wfd;
		std::wstring* dir = (std::wstring*) vect.at(0);
		DefendedInt64* pdint64 = (DefendedInt64* )vect.at(1);
		std::vector<HANDLE>* allevents =(std::vector<HANDLE>*)vect.at(2);
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		std::vector<LPVOID>* entervect = new std::vector<LPVOID>();
		entervect->push_back(dir);
		entervect->push_back(pdint64);
		HANDLE myEvent = ::CreateEvent(NULL,TRUE,//без
		FALSE,NULL);
		entervect->push_back(myEvent);
		allevents->push_back(myEvent); //добавляем ивент этой папки в таблицу ивентов
		pool->AddTask(&FolderSize,(LPVOID)entervect);
		if (wcsncmp(&dir->back(),L"/",1))
		{
			dir->push_back(L'//');
			dir->push_back(L'*');
		}
		else
			dir->push_back(L'*');
		HANDLE  hFind = FindFirstFileW(dir->data(), &wfd);
		std::vector<LPVOID>* newvect;
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				if (!wcscmp(wfd.cFileName,L".") || !wcscmp(wfd.cFileName,L".."))
					 continue;
				int vas;
				if ( (vas = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					//если директория
					WCHAR NewPath[ _MAX_PATH];
					newvect = new std::vector<LPVOID>();
					const wchar_t* dirdata = dir->data();
					std::wstring* newpathstr = new std::wstring(dirdata);
					newpathstr->pop_back();
					dirdata = newpathstr->data();/////////////////////////////////////
					wcscpy(NewPath,dirdata);
					wcscat( NewPath, wfd.cFileName);
					delete newpathstr;
					newpathstr = new std::wstring(NewPath);
					newvect->push_back(newpathstr);
					newvect->push_back(vect[1]);
					newvect->push_back(vect[2]);
					int currentrecursiondeep = *(int*)vect[3];
					currentrecursiondeep++;
					newvect->push_back(&currentrecursiondeep);
					FolderWalker(poolarg,newvect);
				}
			} while (NULL != FindNextFileW(hFind, &wfd));
			FindClose(hFind);
			if (*(int*)vect[3] == 1)//корневая папка
			{
				HANDLE vas;
				for (int i = 0; i< allevents->size(); i++)
				{
					vas = allevents->at(i);
					::WaitForSingleObject(vas,INFINITE);
				}
				Sleep(100);
			}
		}
	}
	void __declspec(dllexport) Size(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		if (vect->size() != 1)
		{
			printf("Size:Unacceptable params!\n");
			return;
		}
		std::vector<LPVOID> mainvect =  std::vector<LPVOID>();
		mainvect.push_back(&vect->at(0));// 0 - путь
		DefendedInt64 curSize = DefendedInt64();
		mainvect.push_back(&curSize);// 1 - текущий размер
		std::vector<HANDLE>* allevents = new std::vector<HANDLE>();
		mainvect.push_back(allevents);
		int firstrecursiondeep = 1;
		mainvect.push_back(&firstrecursiondeep);
		FolderWalker(poolarg,&mainvect);
		printf("Summary size: %lld\n", (long long) curSize.GetInt64());
		for(int i =0; i< allevents->size();i++)
		{
			CloseHandle(allevents->at(i));
		}
		delete allevents;
	}






	void  Hehey(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		int vas = *(int*)vectargs;
		for (int i = 0; i< vas ; i++)
			printf("%d",i);
		printf("\n");
	}	
	void __declspec(dllexport) Vasya(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		int* hue =(int*) malloc(sizeof(int));
		*hue = _wtoi(vect->at(0).data());
		for (int i=0; i< 2; i++)
		{
			pool->AddTask(Hehey,hue);
		}
	}
	void WorkerGotDream(LPVOID poolarg, LPVOID vectargs)
	{
		Sleep(10000);
	}
	void __declspec(dllexport) GetSleep(LPVOID poolarg, LPVOID vectargs)
	{
		ThreadPool* pool =(ThreadPool*) poolarg;
		std::vector<std::wstring>* vect = (std::vector<std::wstring>*) vectargs;
		int count = _wtoi(vect->at(0).data());
		for (int i = 0; i< count; i++)
			pool->AddTask(WorkerGotDream,NULL);
	}

}