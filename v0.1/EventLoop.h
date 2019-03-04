#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <sys/syscall.h>
#include <sys/types.h>
#include <vector>
#include <poll.h>
#include <mutex>
#include <signal.h>
#include "Epoll.h"

class Handler;

class EventLoop{
public:
	EventLoop();
	void loop();
	void quit();
	void addToLoop(const int fd);

private:
	void addToLoop();
	bool isLooping();
	pid_t thread_id;
	std::vector<int> fds;
	bool isLoop;
	bool isQuit;
	Epoll* e;
	std::mutex mtx;
};

#endif
