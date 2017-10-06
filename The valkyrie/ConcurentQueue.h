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
	if (queue.empty())
	{
		waiter.wait(locker, [&] {return !queue.empty() || completionFlag; });
		if (completionFlag)
			return false;
	}
	object = queue.front();
	queue.pop();
	return true;
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
	waiter.notify_all();
}
