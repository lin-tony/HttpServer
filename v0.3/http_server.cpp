#include <iostream>
#include <cstdlib>
#include "socket.h"
#include "event_loop.h"
#include "event_loop_thread.h"
#include "event_loop_thread_pool.h"

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

	int listen_fd = Socket::CreateSocket();
	Socket::SetReuseAddr(listen_fd, true);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//随机的话就htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	Socket::Bind(listen_fd, serv_addr);
	Socket::Listen(listen_fd);
	std::cout << "********Server listen on address: 127.0.0.1, on port: " << port << "********" << std::endl;
	
	EventLoopThreadPool* thread_pool = new EventLoopThreadPool(threadNum);
	while(true){
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		memset(&client_addr, 0, client_addr_len);
		int connect_fd = Socket::Accept(listen_fd, &client_addr);

		EventLoopThread* event_loop_thread = thread_pool->GetNextThread();
		EventLoop* eventloop = event_loop_thread->get_event_loop();
		eventloop->AddToFdVector(connect_fd);
	}

	Socket::Close(listen_fd);
	delete thread_pool;

	return 0;
}
