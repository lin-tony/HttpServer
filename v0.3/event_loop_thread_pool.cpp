#include "event_loop_thread_pool.h"

EventLoopThreadPool::EventLoopThreadPool(const int threadNum)
	:threadNum_(threadNum),
	 nextId_(0){
}

bool EventLoopThreadPool::Init(){
	try{
		for(int i = 0; i < threadNum_; ++i){
			EventLoopThread* thread = new EventLoopThread();
			thread_vector_.push_back(thread);
		}
	}catch(std::bad_alloc){
		std::cout<<"EventLoopThreadPool::EventLoopThreadPool() error: bad_alloc"<<std::endl;
		this->~EventLoopThreadPool();
		return false;
	}
	return true;
}

EventLoopThreadPool::~EventLoopThreadPool(){
	for(auto &thread : thread_vector_){
		if(thread){
			delete thread;
			thread = nullptr;
		}
	}
}

EventLoopThread* EventLoopThreadPool::GetNextThread(){
	nextId_ %= threadNum_;
	return thread_vector_[nextId_++];
}