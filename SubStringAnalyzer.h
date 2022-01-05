/*
 * SubStingAnalyzer.h
 *
 *  Created on: Dec. 8, 2021
 *      Author: leonid
 */

#ifndef SUBSTRINGANALYZER_H_
#define SUBSTRINGANALYZER_H_

#include <vector>
#include <string>
#include <atomic>
#include <condition_variable>


class ThreadPool;

class SubStringAnalyzer
{
public:
	SubStringAnalyzer(ThreadPool& threadPool);
	~SubStringAnalyzer();

public:
	void analyze();

private:
	void parseFileToVector(const char* fileName, std::vector<std::string>& outVector);
	void parseAllWordsFile();
	void parseCommonWordsFile();
	void findSubStrings(int index);
	void createOutput();

private:
	struct OutputItem
	{
		std::string word;
		std::vector<std::string> subStrings;
	};

	ThreadPool& threadPool_;
	std::vector<std::string> allWords_;
	std::vector<std::string> commonWords_;
	std::condition_variable canStartNextStep_;
	std::atomic_int canStartNextStepCnt_;
	std::mutex mutex_;
	std::vector<OutputItem> outputVec_;

};

#endif /* SUBSTRINGANALYZER_H_ */
