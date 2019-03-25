#include "EventLoop.h"

EventLoop::EventLoop()
	:isQuit(false),
	 e(new Epoll(this)){

}

void EventLoop::loop(){
	isQuit = false;
	thread_id = static_cast<pid_t>(syscall(SYS_gettid));
	while(!isQuit){
		std::cout << "--------Thread: " << thread_id << " Looping" << std::endl;
		addToLoop();
		std::vector<Handler*> activeEvents;
		activeEvents.clear();
		
		e->epoll(activeEvents);
		for(std::vector<Handler*>::iterator i = activeEvents.begin();
			i != activeEvents.end(); ++i){
			std::cout << "-----------Handle Request!" << std::endl;
			(*i)->handle();
			e->removeFd((*i)->connFd());
			delete* i;
		}
	}
}

void EventLoop::quit(){
	isQuit = true;
}

void EventLoop::addToLoop(const int fd){
	std::cout << "Thread: " << thread_id << " add socket: " << fd << " to Loop." << std::endl;
	//使用lock_guard则相对安全，它是基于作用域的，能够自解锁，当该对象创建时，它会像m.lock()一样获得互斥锁，当生命周期结束时，它会自动析构(unlock)，不会因为某个线程异常退出而影响其他线程
	std::lock_guard<std::mutex> lockGuard(mtx);
	fds.push_back(fd);
}

void EventLoop::addToLoop(){
	std::lock_guard<std::mutex> lockGuard(mtx);
	if(fds.empty()){
		std::cout <<  "Thread: " << thread_id << " fds empty." << std::endl;
	}else{
		for(int i = 0; i < fds.size(); ++i){
			e->addToEpoll(fds[i]);
		}
		fds.clear();
		std::cout <<  "Thread: " << thread_id << " add all fd into Loop" << std::endl;
	}
}
