// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#include "targetver.h"
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <Windows.h>
#include <Shlwapi.h>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
typedef std::function<void()> fn_type;
typedef void (*DLLFUNC)(LPVOID,LPVOID);
// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы
#include "DLLManager.h"
#include "ThreadPool.h"