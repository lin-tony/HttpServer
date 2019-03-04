#include "EventLoop.h"

EventLoop::EventLoop()
	:isLoop(false),
	 thread_id(static_cast<pid_t>(syscall(SYS_gettid))),
	 isQuit(false),
	 e(new Epoll(this)){

}

void EventLoop::loop(){
	isLoop = true;
	isQuit = false;

	while(!isQuit){
		std::cout << thread_id<<"-----------Looping" << std::endl;
		addToLoop();
		std::vector<Handler*> activeEvents;
		activeEvents.clear();
		
		e->epoll(activeEvents);
		for(std::vector<Handler*>::iterator i = activeEvents.begin();
			i != activeEvents.end(); ++i){
			std::cout << "-----------Handler Request!" << std::endl;
			(*i)->handle();
			e->removeFd((*i)->connFd());
			delete* i;
		}
	}
	isLoop = false;
}

void EventLoop::quit(){
	isQuit = true;
}

void EventLoop::addToLoop(const int fd){
	std::cout << "Add " << fd << " to Loop." << std::endl;
	fds.push_back(fd);
}

void EventLoop::addToLoop(){
	if(fds.empty()){
		std::cout << "fds empty." << std::endl;
	}else{
		for(int i = 0; i < fds.size(); ++i){
			e->addToEpoll(fds[i]);
		}
		fds.clear();
		std::cout << "Add all fd into Loop" << std::endl;
	}
}
