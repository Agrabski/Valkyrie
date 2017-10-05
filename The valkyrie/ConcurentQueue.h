#pragma once


template<typename t> class ConcurrentQueue
{
	std::queue<t> queue;
	std::condition_variable waiter;
	std::mutex lock;
	std::atomic <bool> completionFlag = false;
public:
	bool pop(t& object);
	void push(t object);
	void finish();
};

template<typename t>
inline bool ConcurrentQueue<t>::pop(t & object)
{
	std::unique_lock<std::mutex> locker(lock);
	waiter.wait(locker, [this] {return !queue.empty() || completionFlag; });
	if (!queue.empty())
	{
		object = queue.front();
		queue.pop();
		return true;
	}
	else
		return false;
}

template<typename t>
inline void ConcurrentQueue<t>::push(t object)
{
	std::unique_lock<std::mutex> locker(lock);
	queue.push(object);
	waiter.notify_one();
}

template<typename t>
inline void ConcurrentQueue<t>::finish()
{
	completionFlag = true;
}
