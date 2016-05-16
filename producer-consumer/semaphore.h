#include <mutex>
#include <condition_variable>

class semaphore
{
public:
	semaphore(int c = 0) : counter{c}
	{}

	~semaphore()
	{}

	void wait ()
	{
		std::unique_lock<std::mutex> ulock(m);
		cv.wait(ulock, [this]() { return counter > 0; });
		counter--;
	}

	void signal () 
	{
		std::unique_lock<std::mutex> ulock(m);
		counter++;
		cv.notify_one();
	}

	bool waitOrShutdown ()
	{
		std::unique_lock<std::mutex> ulock(m);
		cv.wait(ulock, [this]() { return (counter > 0 || shutdown); });
		if (shutdown)
		{
			return true;
		}
		counter--;
		return false;
	}

	void setCounter (int c) 
	{
		counter = c;
	}

	void doShutdown () 
	{
		std::unique_lock<std::mutex> ulock(m);
		shutdown = true;
		cv.notify_all();
	}

	bool isShutdown ()
	{
		std::unique_lock<std::mutex> ulock(m);
		return shutdown;
	}

	/* data */
private:

	int counter;
	bool shutdown = false;
	std::mutex m;
	std::condition_variable cv;
};