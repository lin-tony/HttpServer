HttpServer: HttpServer.o Socket.o EventLoopThreadPool.o EventLoopThread.o Thread.o EventLoop.o Parse.o Handler.o Epoll.o Buffer.o
	g++ -std=c++11 -pthread -o HttpServer HttpServer.o Socket.o EventLoopThreadPool.o EventLoopThread.o Thread.o EventLoop.o Parse.o Handler.o Epoll.o Buffer.o

HttpServer.o: HttpServer.cpp Socket.h EventLoopThreadPool.h EventLoopThread.h EventLoop.h
	g++ -std=c++11 -c -pthread HttpServer.cpp EventLoopThreadPool.cpp EventLoopThread.cpp EventLoop.cpp

Socket.o: Socket.cpp Socket.h 
	g++ -std=c++11 -c -pthread Socket.cpp

EventLoopThreadPool.o: EventLoopThreadPool.cpp EventLoopThread.h 
	g++ -std=c++11 -c -pthread EventLoopThreadPool.cpp EventLoopThread.cpp

EventLoopThread.o: EventLoopThread.cpp EventLoopThread.h EventLoop.h Thread.h 
	g++ -std=c++11 -c -pthread EventLoopThread.cpp EventLoop.cpp Thread.cpp

Thread.o: Thread.cpp Thread.h 
	g++ -std=c++11 -c -pthread Thread.cpp

EventLoop.o: EventLoop.cpp EventLoop.h Epoll.h 
	g++ -std=c++11 -c EventLoop.cpp Epoll.cpp

Parse.o: Parse.cpp Parse.h 
	g++ -std=c++11 -c Parse.cpp

Handler.o: Handler.cpp Handler.h Parse.h 
	g++ -std=c++11 -c Handler.cpp Parse.cpp

Epoll.o: Handler.cpp Handler.h Parse.h Epoll.h 
	g++ -std=c++11 -c Epoll.cpp Handler.cpp Parse.cpp

Buffer.o: Buffer.cpp Buffer.h 
	g++ -std=c++11 -c Buffer.cpp

clean:
	rm *.o HttpServer
