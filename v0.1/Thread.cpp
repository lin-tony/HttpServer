#include <pthread.h>
#include "Thread.h"

Thread::~Thread(){
	if(started && !joined)
		//pthread_join()函数的替代函数，用于只是应用程序在线程tid终止时回收其存储空间。如果tid尚未终止，pthread_detach()不会终止该线程。
		pthread_detach(pthreadId);
}

void Thread::start(){
	started = true;
	if(pthread_create(&pthreadId, NULL, threadFunc, _arg)){
		started = false;
		std::cout << "Thread start error" << std::endl;
		return;
	}
	std::cout << "Thread: " << pthreadId << " created" << std::endl;
}

int Thread::join(){
	joined = true;
	//调用了pthread_join之后当线程函数自己返回退出时或pthread_exit时才会释放线程所占用堆栈和线程描述符（总计8K多）
	return pthread_join(pthreadId, NULL);
}
