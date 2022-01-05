/*
 * ThreadPool.h
 *
 *  Created on: Dec. 8, 2021
 *      Author: leonid
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

public:
	void addTask(std::function<void()> task);

private:
	void threadLoop();
	void shutdown();

private:
	std::atomic_bool shuttingDown_;
	std::vector<std::thread> threads_;
	std::mutex tasksMutex_;
	std::queue<std::function<void()>> tasks_;
	std::condition_variable condition_;

};

#endif /* THREADPOOL_H_ */
