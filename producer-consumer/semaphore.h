#include <mutex>
#include <condition_variable>
#include <atomic>

class semaphore
{
public:
	semaphore(int c = 0) : counter{c}
	{
		shutdown.store(false);
	}

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
		if(shutdown.load())
			return true;
		std::unique_lock<std::mutex> ulock(m);
		cv.wait(ulock, [this]() { return (counter > 0 || shutdown); });
		counter--;
		return false;
	}

	void setCounter (int c) 
	{
		counter = c;
	}

	void doShutdown () 
	{
		//std::unique_lock<std::mutex> ulock(m);
		shutdown.store(true);
		cv.notify_all();
	}

	bool isShutdown ()
	{
		//std::unique_lock<std::mutex> ulock(m);
		//shutdown = true;
		return shutdown.load();//shutdown;
	}

	/* data */
private:

	int counter;
	std::atomic_bool shutdown;
	std::mutex m;
	std::condition_variable cv;
};