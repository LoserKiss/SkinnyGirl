#include "stdafx.h"

DLLManager::DLLManager(void)
{
}
DLLManager::~DLLManager(void)
{
}
int DLLManager::LoadDLL(void)
{
	WIN32_FIND_DATAW wfd;
 
	LPCWSTR name = new WCHAR[MAX_PATH];
	GetModuleFileName(GetModuleHandle(0),(LPWSTR) name, MAX_PATH);
	PathRemoveFileSpec((LPWSTR) name);
    HANDLE const hFind = FindFirstFileW(name, &wfd);
 
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
          if(".dll" == _strlwr((char *) PathFindExtension(wfd.cFileName)))//))00
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