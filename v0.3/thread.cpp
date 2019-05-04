#include "thread.h"

Thread::~Thread(){
	if(start_ && !join_)
		thread_id_.detach();
	
		//pthread_detach用于只是应用程序在线程tid终止时回收其存储空间。如果tid尚未终止，pthread_detach()不会终止该线程。
		//使主线程与子线程分离，两者相互不干涉，子线程结束的同时子线程的资源自动回收
		//pthread_detach(pthreadId);//pthread_join还在这里再次使用pthread_detach确保子线程的内存必定被释放
}

void Thread::Start(){
	start_ = true;

	std::thread t(thread_func_, arg_);
	std::swap(t, thread_id_);
	//要加个~t
	/*if(pthread_create(&pthreadId, NULL, threadFunc, _arg)){
		started = false;
		std::cout << "Thread start error" << std::endl;
		return;
	}*/
	std::cout << "Thread: " << thread_id_.get_id() << " created" << std::endl;
}

void Thread::Join(){
	join_ = true;
	thread_id_.join();

	//pthread_join是一个阻塞函数，调用方会阻塞到pthread_join所指定的tid的线程结束后才被回收线程所占用堆栈和线程描述符（总计8K多）
	//子线程合入主线程，主线程会一直阻塞，直到子线程执行结束，然后回收子线程资源，并继续执行。
	//return pthread_join(pthreadId, NULL);
}

