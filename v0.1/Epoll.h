#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "Handler.h"

class EventLoop;//前向声明

class Epoll{
public:
	//explicit 防止不应该允许的经过转换构造函数进行的隐式转换
	explicit Epoll(EventLoop* loop)
		:ownerLoop(loop),
		//创建一个多路复用的实例, EPOLL_CLOEXEC表示生成的epoll fd具有“执行后关闭”特性
		epollFd(epoll_create1(EPOLL_CLOEXEC)),
		eventEpoll(8){
		if(epollFd < 0){
			std::cout << "Epoll::epoll_create1() error: " << ::strerror(errno) << std::endl;
		}
	}
	~Epoll(){
		close(epollFd);
	}
	//调用epoll_wait，回调则给Event类的HandEvent处理
	void epoll(std::vector<Handler*> &events);
	void removeFd(const int fd);
	void addToEpoll(const int fd);

private:
	EventLoop* ownerLoop;
	int epollFd;
	std::vector<struct epoll_event> eventEpoll;
};

#endif
