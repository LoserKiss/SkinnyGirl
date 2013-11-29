// SkinnyGirl.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#define LIMIT 4096
char *programName;
int procCount = 0;
char *bufBytes;
int threadMinCount = 1;
int threadMaxCount = 1;
int threadLifeTime = 1488;
int len;
int _tmain(int argc, char* argv[])// min max lifetime
{
	_splitpath(argv[0],NULL,NULL,programName,NULL);
	if(argc != 4)
    {
        fprintf(stderr, "%s: Programm takes 3 arguments: minCount, maxCount and lifeTime\n", programName);
        return 1;
    }
    threadMinCount = strtol(argv[argc - 3], NULL, 0);
    threadMaxCount = strtol(argv[argc - 2], NULL, 0);
	threadLifeTime = strtol(argv[argc - 1], NULL, 0);
    if(threadMinCount <= 0 || threadMaxCount <= 0 || threadLifeTime <= 0)
    {
        fprintf(stderr, "%s: Invalid count argument!\n", programName);
        return 1;
    }
	MyThreadPool pool = MyThreadPool(threadMinCount,threadMaxCount,threadLifeTime);
	char* command;
	std::string strcommand;
	std::vector<std::string> vParsedString;
	while (1)
	{
		gets(command);
		if (command == "Exit")
			break;
		strcommand = std::string(command);
		split(strcommand, vParsedString,' ');
		pool.GiveTask(vParsedString);
	}
	return 0;
}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    unsigned int pos = txt.find( ch );
    unsigned int initialPos = 0;
	std::vector<std::string> tempvect;
	tempvect.clear();
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        tempvect.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    tempvect.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
	for (int i = 0; i< tempvect.size(); i++)
		if (tempvect[i].data != "")
			strs.push_back(tempvect[i]);
    return strs.size();
}