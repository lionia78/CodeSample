/*
 * SubStingAnalyzer.cpp
 *
 *  Created on: Dec. 8, 2021
 *      Author: leonid
 */

#include "SubStringAnalyzer.h"
#include "ThreadPool.h"
#include <fstream>
#include <sstream>

SubStringAnalyzer::SubStringAnalyzer(ThreadPool& threadPool)
	: threadPool_(threadPool)
{
	canStartNextStepCnt_ = 0;

}

SubStringAnalyzer::~SubStringAnalyzer()
{
	// TODO Auto-generated destructor stub
}

void SubStringAnalyzer::parseFileToVector(const char* fileName, std::vector<std::string>& outVector)
{
	std::ifstream inFile(fileName);
	if (!inFile.is_open())
	{
		// TODO: log error
		return;
	}

	std::string line;
	while (std::getline(inFile, line))
	{
		outVector.emplace_back(std::string(line, 0, line.length() - 1)); // avoiding new line at the end of the string
	}
}

void SubStringAnalyzer::parseAllWordsFile()
{
	// no synchronization is required as will access different vectors
	parseFileToVector("allwords.txt", allWords_);
	++canStartNextStepCnt_;
	canStartNextStep_.notify_one();
}

void SubStringAnalyzer::parseCommonWordsFile()
{
	// no synchronization is required as will access different vectors
	parseFileToVector("commonwords.txt", commonWords_);
	++canStartNextStepCnt_;
	canStartNextStep_.notify_one();
}

void SubStringAnalyzer::findSubStrings(int index)
{
	// Considering the vector of allWords is accessed by a specific index and
	// we are going to perform only "read" operation
	// we are not going to run into the race condition in multi-threaded environment, so we don't need locking.

	const std::string& word = allWords_[index];
	OutputItem& item = outputVec_[index];
	item.word = word;
	for (const auto& pattern : commonWords_)
	{
		if (word.find(pattern) !=  std::string::npos)
		{
			item.subStrings.emplace_back(pattern);
		}
	}

	++canStartNextStepCnt_;
	canStartNextStep_.notify_one();
}

void SubStringAnalyzer::createOutput()
{
	std::ofstream outFile("output.txt");
	if (!outFile.is_open())
	{
		// TODO: log error
		return;
	}

	for (const auto& outputItem : outputVec_)
	{
		std::string outLine(outputItem.word + ": ");
		std::ostringstream subStrs;
		std::string delimeter = "";
		for (const auto& subStr : outputItem.subStrings)
		{
			subStrs << delimeter << subStr ;
			delimeter = ", ";
		}
		subStrs << "\n";
		outLine.append(subStrs.str());
		outFile << outLine;
	}
}

void SubStringAnalyzer::analyze()
{
	// In more complex case of input files we can specify input files as a configuration
	// and count numOfInputFiles while parsing the configuration.
	// For this case it's just local hard coded variable;
	int numOfInputFiles = 2;
	threadPool_.addTask(std::bind(&SubStringAnalyzer::parseAllWordsFile, this));
	threadPool_.addTask(std::bind(&SubStringAnalyzer::parseCommonWordsFile, this));

	{
		std::unique_lock<std::mutex> lock(mutex_);
		canStartNextStep_.wait(lock, [&numOfInputFiles, this]()
			{
				return canStartNextStepCnt_ == numOfInputFiles;
			});
	}

	canStartNextStepCnt_ = 0;
	outputVec_.resize(allWords_.size());

	for (unsigned int i = 0; i < allWords_.size(); ++i)
	{
		auto func = [i, this]() -> void
		{
			findSubStrings(i);
		};
		threadPool_.addTask(func);
	}

	{
		std::unique_lock<std::mutex> lock(mutex_);
		canStartNextStep_.wait(lock, [this]()
			{
				return canStartNextStepCnt_ == (int)allWords_.size();
			});
	}

	createOutput();
}
