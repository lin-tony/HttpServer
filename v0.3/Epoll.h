#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "handler.h"

class EventLoop;//前向声明

class Epoll{
public:
	//explicit 防止不应该允许的经过转换构造函数进行的隐式转换
	explicit Epoll()
		:epoll_fd_(epoll_create(8)),//创建一个多路复用的实例，参数在linux2.6之后已被忽略，但仍需要设置
		 event_epoll_vector_(8){
		if(epoll_fd_ < 0){
			std::cout << "Epoll::epoll_create1() error: " << ::strerror(errno) << std::endl;
		}
	}
	~Epoll(){
		close(epoll_fd_);
	}
	//调用epoll_wait，回调则给Event类的HandEvent处理
	void EpollWaitToGetActive(std::vector<Handler*> &events);
	void RemoveFd(const int fd);
	void AddToEpollQueue(const int fd);

private:
	int epoll_fd_;
	std::vector<struct epoll_event> event_epoll_vector_;
};

#endif
