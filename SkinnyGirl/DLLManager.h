#pragma once
#include "stdafx.h"
class DLLManager
{
public:
	DLLFUNC GetFuncAdress(std::string name);
	int LoadDLL(void);
	DLLManager(void);
	~DLLManager(void);
private:
	std::vector<HMODULE> loadedDLLVect;
};
