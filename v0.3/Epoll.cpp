#include "epoll.h"

void Epoll::EpollWaitToGetActive(std::vector<Handler*> &activeEvents){
	int numEvents = epoll_wait(epoll_fd_, event_epoll_vector_.data(), 
							static_cast<int>(event_epoll_vector_.size()), 1);

	if(numEvents < 0){
		if(errno != EINTR){
			std::cout << "Epoll::epoll() error: " <<strerror(errno) << std::endl;
			exit(1);
		}
	}else if(numEvents > 0){
		for(int i = 0; i < numEvents; i++){
			Handler* h = new Handler(event_epoll_vector_[i].data.fd);
			activeEvents.push_back(h);
		}
	}
}

void Epoll::RemoveFd(const int fd){
	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL);
}

void Epoll::AddToEpollQueue(const int fd){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;//使用了ET模式，注意加入监听队列的套接字必须为非阻塞socket（Socket::setnonblock），否则读取完毕不会有EAGAIN错误让循环退出。
	if(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0){
		std::cout << "Epoll::addToEpoll error: " << strerror(errno) << std::endl;
	}
	std::cout << "Add " << fd << " to Epoll..." << std::endl;
}
