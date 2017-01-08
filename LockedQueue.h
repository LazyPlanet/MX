#include <mutex>
#include <queue>

namespace Adoter
{

template<class T>
class LockedQueue
{
private:
    	std::mutex _lock;
	bool _canceled;
	std::queue<T> _queue;
public:    
	void Cancel()
	{
		std::lock_guard<std::mutex> lock(_lock);
		_canceled = true;
	}

	bool Cancelled()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _canceled;
	}

	void Lock()
	{
		this->_lock.lock();
	}

	void Unlock()
	{
		this->_lock.unlock();
	}

	void Add(const T& item)
	{
		lock();
		_queue.push_back(item);
		unlock();
	}
};

}
