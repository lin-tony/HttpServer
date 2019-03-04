#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
	:_loop(new EventLoop()),
	 _thread(threadFunc, _loop)
{
	_thread.start();
}

EventLoopThread::~EventLoopThread(){
	_loop->quit();
	_thread.join();
}

void* EventLoopThread::threadFunc(void* arg){
	EventLoop* loop = (EventLoop*)arg;
	loop->loop();
}