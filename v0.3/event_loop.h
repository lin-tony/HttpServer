#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <sys/syscall.h>
#include <sys/types.h>
#include <vector>
#include <poll.h>
#include <mutex>
#include <signal.h>
#include "epoll.h"

class Handler;

class EventLoop{
public:
	EventLoop();
	void Loop();
	void Quit();
	void AddToFdVector(const int fd);

private:
	void AddToEpoll();
	pid_t thread_id_;
	std::vector<int> fd_vector_;
	bool is_quit_;
	Epoll* epoll_;
	std::mutex mutex_;
};

#endif
