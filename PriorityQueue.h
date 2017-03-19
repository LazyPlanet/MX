#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>

#include "P_Header.h"

namespace Adoter
{

/*
 * 类功能：
 *
 * 具有优先级的消息队列.
 *
 * 理论上协议的处理也应该先放到队列，然后顺序处理，但本架构完全交给ASIO处理.
 *
 * 如果系统后面有丢包或者处理错误，可以增加消息队列对Client<->Server之间的处理.
 *
 *
 * */

template <typename T>
class PriorityQueue
{
private:
	std::priority_queue<T> _queue;
	std::mutex _mutex;
public:
	void Emplace(const T item) //此处不要传入引用
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.emplace(item);
	}
	
	const T& Peek()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _queue.top();
	}
	
	const T& GetNext()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		const T& item = _queue.top();	
		
		_queue.pop();
		return item;
	}
	
	void Pop()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.pop();
	}
	
	bool Empty()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _queue.empty();
	}
	
	size_t Size()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _queue.size();
	}

	void Lock()
	{
		this->_mutex.lock();
	}

	void UnLock()
	{
		this->_mutex.unlock();
	}
};

}
