#include "Socket.h"

void Socket::setReuseAddr(const int fd, bool on){
	int flag = on ? 1 : 0;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

void Socket::setTcpNoDelay(const int fd, bool on){
	int flag = on ? 1 : 0;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
}

int Socket::CreateSocket(){
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd < 0){
		std::cout << "Socket::CreateSocket error:" << strerror(errno) << std::endl;
		exit(1);
	}
	return fd;
}

void Socket::Bind(const int fd, const struct sockaddr_in &addr){
	if(bind(fd, (struct sockaddr*)(&addr), sizeof(addr)) < 0){
		std::cout << "Socket::Bind error:" << strerror(errno) << std::endl;
		exit(1);
	}
}

void Socket::Listen(const int fd){
	if(listen(fd, 5) < 0){
		std::cout << "Socket::Listen error:" << strerror(errno) << std::endl;
		exit(1);
	}
}

int Socket::Accept(const int fd, struct sockaddr_in *addr){
	socklen_t addrLen = sizeof(*addr);
	int connFd = accept(fd, (struct sockaddr*)&addr, &addrLen);
	std::cout << "Accept: " << connFd<< std::endl;
	setTcpNoDelay(connFd, true);
	setNonBlock(connFd);

	if(connFd < 0){
		switch(errno){
			//非致命错误，忽略
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EMFILE:
				break;
			//致命错误
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOMEM:
				std::cout << "Socket::Accept error:" << strerror(errno) << std::endl;
				exit(1);
				break;
			default:
				std::cout << "Socket::Accept error:" << strerror(errno) << std::endl;
				exit(1);
				break;
		}
	}
	return connFd;
}

void Socket::setNonBlock(const int fd){
	int flag;
	if((flag = fcntl(fd, F_GETFL, 0)) < 0){
		std::cout << "Socket::setNonBlock error: " <<strerror(errno) << std::endl;
        exit(1);
	}
	flag |= O_NONBLOCK;
	flag |= FD_CLOEXEC;
	if(fcntl(fd, F_SETFL, flag) < 0){
		std::cout << "Socket::setNonBlock error: " <<strerror(errno) << std::endl;
        exit(1);
	}
}
