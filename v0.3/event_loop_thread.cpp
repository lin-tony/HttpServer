#include "event_loop_thread.h"

EventLoopThread::EventLoopThread()
	:event_loop_(new EventLoop()),
	 thread_(ThreadFunc, event_loop_)
{
	thread_.Start();
}

EventLoopThread::~EventLoopThread(){
	event_loop_->Quit();
	thread_.Join();
}

void* EventLoopThread::ThreadFunc(void* arg){
	EventLoop* event_loop = (EventLoop*)arg;
	event_loop->Loop();
}