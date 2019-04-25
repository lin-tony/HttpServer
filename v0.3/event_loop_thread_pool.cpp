#include "event_loop_thread_pool.h"

EventLoopThreadPool::EventLoopThreadPool(const int threadNum)
	:threadNum_(threadNum),
	 nextId_(0)
{
	for(int i = 0; i < threadNum; ++i){
		EventLoopThread* thread = new EventLoopThread();
		thread_vector_.push_back(thread);
	}
}

EventLoopThreadPool::~EventLoopThreadPool(){
	for(int i = 0; i < thread_vector_.size(); ++i){
		delete thread_vector_[i];
	}
}

EventLoopThread* EventLoopThreadPool::GetNextThread(){
	nextId_ %= threadNum_;
	return thread_vector_[nextId_++];
}