#include <iostream>
#include <cstdlib>
#include "Socket.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

int main(int argc, char** argv){
	if(argc != 2){
		std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return 0;
	}

	int port = std::atoi(argv[1]);

	int listenFd = Socket::CreateSocket();
	Socket::setReuseAddr(listenFd, true);
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);
	Socket::Bind(listenFd, servAddr);
	Socket::Listen(listenFd);

	EventLoopThreadPool* threadPool = new EventLoopThreadPool(5);
	while(true){
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		memset(&clientAddr, 0, clientAddrLen);
		int connFd = Socket::Accept(listenFd, &clientAddr);

		EventLoopThread* thread = threadPool->getNextThread();
		EventLoop* loop = thread->getLoop();
		loop->addToLoop(connFd);
	}

	Socket::Close(listenFd);
	delete threadPool;

	return 0;
}
