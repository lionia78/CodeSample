/*
 * ThreadPool.cpp
 *
 *  Created on: Dec. 8, 2021
 *      Author: leonid
 */

#include "ThreadPool.h"

ThreadPool::ThreadPool()
	: shuttingDown_(false)
{
	int numThreads = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; i++)
	{
		threads_.emplace_back(std::thread(&ThreadPool::threadLoop, this));
	}

}

ThreadPool::~ThreadPool()
{
	shutdown();
}

void ThreadPool::shutdown()
{

	shuttingDown_ = true;

    condition_.notify_all(); // wake up all threads.

    // Join all threads.
    for (std::thread &th : threads_)
    {
        th.join();
    }

    threads_.clear();
}

void ThreadPool::threadLoop()
{
	while(!shuttingDown_)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(tasksMutex_);
			condition_.wait(lock, [this]()
					{
						return !tasks_.empty() || shuttingDown_;
					});
			if (shuttingDown_)
			{
				break;
			}

			task = tasks_.front();
			tasks_.pop();
		}
		task();
	}
}

void ThreadPool::addTask(std::function<void()> task)
{
	{
		std::unique_lock<std::mutex> lock(tasksMutex_);
		tasks_.push(task);
	}
	condition_.notify_one();
}
