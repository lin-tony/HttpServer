#include "Epoll.h"

void Epoll::epoll(std::vector<Handler*> &activeEvents){
	int numEvents = epoll_wait(epollFd, eventEpoll.data(), 
							static_cast<int>(eventEpoll.size()), 1000);

	if(numEvents < 0){
		if(errno != EINTR){
			std::cout << "Epoll::epoll() error: " <<strerror(errno) << std::endl;
			exit(1);
		}
	}else if(numEvents > 0){
		for(int i = 0; i < numEvents; i++){
			Handler* h = new Handler(eventEpoll[i].data.fd);
			activeEvents.push_back(h);
		}
	}
}

void Epoll::removeFd(const int fd){
	epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

void Epoll::addToEpoll(const int fd){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;//使用了ET模式，注意注入的套接字必须为非阻塞socket，否则读取完毕不会有EAGAIN错误让循环退出。
	if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0){
		std::cout << "Epoll::addToEpoll error: " << strerror(errno) << std::endl;
	}
	std::cout << "Add " << fd << " to Epoll..." << std::endl;
}
