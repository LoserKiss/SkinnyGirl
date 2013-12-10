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
	GetCurrentDirectory(MAX_PATH,name);
	name = wcscat(name, L"\\*");

	HANDLE  hFind = FindFirstFileW(name, &wfd);
	std::string ext;
	char* vasya = new char[MAX_PATH];
    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
			std::wcstombs(vasya,wfd.cFileName,MAX_PATH);
			vasya = strlwr(vasya);
			ext = vasya;
			if (ext.find(".") != std::string::npos)
			{
				ext = ext.substr(ext.find_last_of("."));
				if(!strcmp(const_cast<char*>(ext.data()),".dll") )//))00
					{
						HINSTANCE hDllInstance = LoadLibrary( wfd.cFileName );
						loadedDLLVect.push_back(hDllInstance);
					}
			}
        } while (NULL != FindNextFileW(hFind, &wfd));
        FindClose(hFind);
    }
	return 0;
}
DLLFUNC DLLManager::GetFuncAdress(std::wstring name)
{
	DLLFUNC adr;
	char* vasya = new char[name.size()];
	for(int i = 0; i < loadedDLLVect.size(); i++)
	{
		wcstombs (vasya, name.data(),name.size()+1);
		if ((adr = (DLLFUNC) GetProcAddress(loadedDLLVect[i],vasya)) != NULL)
			return adr;
	}
	return NULL;
}