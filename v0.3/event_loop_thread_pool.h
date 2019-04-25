#ifndef EVENTLOOPTHREADLOOP_H
#define EVENTLOOPTHREADLOOP_H

#include <vector>
#include "event_loop_thread.h"

class EventLoopThreadPool{
public:
	EventLoopThreadPool(const int threadNum);
	~EventLoopThreadPool();
	EventLoopThread* GetNextThread();

private:
	int threadNum_;
	int nextId_;
	std::vector<EventLoopThread*> thread_vector_;
};

#endif
