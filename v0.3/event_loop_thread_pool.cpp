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
	for(int i = 0; i < thread_vector_.size(); ++i){
		if(thread_vector_[i]){
			delete thread_vector_[i];
			thread_vector_[i] = nullptr;
		}
	}
}

EventLoopThread* EventLoopThreadPool::GetNextThread(){
	nextId_ %= threadNum_;
	return thread_vector_[nextId_++];
}