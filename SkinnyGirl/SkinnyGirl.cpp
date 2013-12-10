// SkinnyGirl.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
char *bufBytes;
int threadMinCount = 1;
int threadMaxCount = 1;
int threadLifeTime = 5555;
int len;
#pragma warning(disable : 4996)
 wchar_t *GetWC(const char *c)
{
     size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}
unsigned int split(const std::wstring &txt, std::vector<std::wstring> &strs , char ch)
{
    unsigned int pos = txt.find( ch );
    unsigned int initialPos = 0;
	std::vector<std::wstring> tempvect;
	tempvect.clear();
    strs.clear();

    // Decompose statement
    while( pos != std::wstring::npos ) {
        tempvect.push_back( txt.substr( initialPos, pos - initialPos  ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    tempvect.push_back( txt.substr( initialPos, min( pos, txt.size() ) - initialPos + 1 ) );
	for (int i = 0; i< tempvect.size(); i++)
		if (tempvect[i].data() != L"")
			strs.push_back(tempvect[i]);
    return strs.size();
}
#pragma warning(disable : 4996)
int main(int argc, char *argv[])// min max lifetime
{
	fprintf(stdout,"Welcome to my Thread pool\n");
	if(argc != 4)
    {
        fprintf(stderr, "Programm takes 3 arguments: minCount, maxCount and lifeTime\n");
        return 1;
    }
    threadMinCount = atoi(argv[argc - 3]);
    threadMaxCount = atoi(argv[argc - 2]);
	threadLifeTime = atoi(argv[argc - 1]);
    if(threadMinCount <= 0 || threadMaxCount <= 0 || threadLifeTime <= 0)
    {
        fprintf(stderr, "Invalid count argument!\n");
        return 1;
    }
	ThreadPool* pool = new ThreadPool(threadMinCount,threadMaxCount,threadLifeTime);
	DLLManager man = DLLManager();
	man.LoadDLL();
	DLLFUNC fn;
	char* command = (char*)malloc(sizeof(char) * 100);
	std::wstring strcommand;
	std::vector<std::wstring>* vParsedString;
	while (1)
	{
		gets_s(command, MAXSIZE_T);

		strcommand = std::wstring(GetWC(command));
		if (!strcmp(command,"Exit"))
		{
			Exit(&pool);
			break;
		}
		vParsedString = new std::vector<std::wstring>();
		split(strcommand, *vParsedString,' ');
		if ((vParsedString->size() == 1) &&(!wcscmp(vParsedString->at(0).data(),L"")))
		{
			std::cout << "Nothing is here :(\n";
			continue;
		}
		fn = man.GetFuncAdress(vParsedString->at(0));
		if (fn == NULL)
		{
			std::cout << "There is no such function :(\n";
			continue;
		}
		vParsedString->erase(vParsedString->begin());
		pool->AddTask(fn,vParsedString);
	}
	return 0;
}