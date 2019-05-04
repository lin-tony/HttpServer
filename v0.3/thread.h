#ifndef THREAD_H
#define THREAD_H

#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>

class Thread{
public:
	typedef void* (*ThreadFunc)(void*);
	explicit Thread(ThreadFunc func, void* arg)
		:start_(false),
		 join_(false),
		 arg_(arg),
		 thread_func_(func){
	}

	~Thread();
	void Start();
	void Join();

private:
	bool start_;
	bool join_;
	void* arg_;
	std::thread thread_id_;
	ThreadFunc thread_func_;
};

#endif
