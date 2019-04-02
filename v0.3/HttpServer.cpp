#include <iostream>
#include <cstdlib>
#include "Socket.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

int main(int argc, char** argv){
	if(argc > 5){
		std::cout << "Usage: " << argv[0] << " [-t thread_numbers] [-p port]" << std::endl;
        return 0;
	}

	int threadNum = 8;
	int port = 8088;
	int opt;
	const char* str = "t:p:";
	while((opt = getopt(argc, argv, str)) != -1){
		switch(opt){
			case 't':{
				threadNum = atoi(optarg);
				break;
			}
			case 'p':{
				port = atoi(optarg);
				break;
			}
			default: 
				break;
		}
	}

	int listenFd = Socket::CreateSocket();
	Socket::setReuseAddr(listenFd, true);
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//随机的话就htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);
	Socket::Bind(listenFd, servAddr);
	Socket::Listen(listenFd);
	std::cout << "********Server listen on address: 127.0.0.1, on port: " << port << "********" << std::endl;
	
	EventLoopThreadPool* threadPool = new EventLoopThreadPool(threadNum);
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
