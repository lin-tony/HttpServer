#ifndef EVENTLOOPTHREADLOOP_H
#define EVENTLOOPTHREADLOOP_H

#include <vector>
#include "EventLoopThread.h"

class EventLoop;

class EventLoopThreadPool{
public:
	EventLoopThreadPool(const int threadNum);
	~EventLoopThreadPool();
	EventLoopThread* getNextThread();

private:
	int _threadNum;
	int _nextId;
	EventLoop* _baseLoop;
	std::vector<EventLoopThread*> _threads;
};

#endif