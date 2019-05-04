#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <netinet/tcp.h>

class Socket{
public:
	//开启SO_REUSERADDR
	static void SetReuseAddr(const int fd, bool on);

	//开启TCP_NODELAY，禁用Nagle算法,允许小包发送
	static void SetTcpNoDelay(const int fd, bool on);

	//O_NONBLOCK，非阻塞模式
	static void SetNonBlock(const int fd);

	//socket基本操作
	static int CreateSocket();
	static void Bind(const int fd, const struct sockaddr_in &addr);
	static void Listen(const int fd);
	static int Accept(const int fd, struct sockaddr_in *addr);
	static void Close(const int fd);
};

#endif //SOCKET_H
