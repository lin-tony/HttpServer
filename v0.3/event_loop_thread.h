#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <functional>
#include "thread.h"
#include "event_loop.h"

class EventLoopThread{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* get_event_loop(){
		return event_loop_;
	}

private:
	static void* ThreadFunc(void* arg);
	EventLoop* event_loop_;
	Thread thread_;
};

#endif
