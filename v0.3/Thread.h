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
		:started(false),
		 joined(false),
		 _arg(arg),
		 threadFunc(func){
	}

	~Thread();
	void start();
	void join();

private:
	bool started;
	bool joined;
	void* _arg;
	std::thread threadId;
	ThreadFunc threadFunc;
};

#endif
