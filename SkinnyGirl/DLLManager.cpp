#include "stdafx.h"

DLLManager::DLLManager(void)
{
}
DLLManager::~DLLManager(void)
{
}
#pragma warning(disable : 4996)
int DLLManager::LoadDLL(void)
{
	WIN32_FIND_DATAW wfd;
 
	LPWSTR name = new WCHAR[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0),(LPWSTR) name, MAX_PATH);
	//PathRemoveFileSpec(name);
    HANDLE const hFind = FindFirstFileW(name, &wfd);
	std::string ext;
	char* vasya = new char[MAX_PATH];
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
			std::wcstombs(vasya,wfd.cFileName,MAX_PATH);
			vasya = strlwr(vasya);
			ext = vasya;
			if(".dll" == ext.substr(ext.find_last_of(".")).data())//))00
		  {
			  HINSTANCE hDllInstance = LoadLibraryEx( wfd.cFileName, 0, DONT_RESOLVE_DLL_REFERENCES );
			  loadedDLLVect.push_back(hDllInstance);
		  }
        } while (NULL != FindNextFileW(hFind, &wfd));
 
        FindClose(hFind);
    }
	return 0;
}
void* DLLManager::GetFuncAdress(std::string name)
{
	void* adr;
	for(int i = 0; i < loadedDLLVect.size(); i++)
	{
		if ((adr = GetProcAddress(loadedDLLVect[i],name.data())) != NULL)
			return adr;
	}
	return NULL;
}