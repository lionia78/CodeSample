//============================================================================
// Name        : EATask.cpp
// Author      : Leonid
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "ThreadPool.h"
#include "SubStringAnalyzer.h"

int main()
{
	ThreadPool threadPool;
	SubStringAnalyzer analyzer(threadPool);
	analyzer.analyze();
	return 0;
}
