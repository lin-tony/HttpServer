#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <sys/syscall.h>
#include <sys/types.h>
#include <assert.h>
#include <vector>
#include <poll.h>
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
	const pid_t thread_id;
	std::vector<int> fds;
	bool isLoop;
	bool isQuit;
	Epoll* e;
};

#endif