http_server: http_server.o socket.o event_loop_thread_pool.o event_loop_thread.o thread.o event_loop.o parse.o handler.o epoll.o buffer.o
	g++ -std=c++11 -pthread -o http_server http_server.o socket.o event_loop_thread_pool.o event_loop_thread.o thread.o event_loop.o parse.o handler.o epoll.o buffer.o

http_server.o: http_server.cpp socket.h event_loop_thread_pool.h event_loop_thread.h event_loop.h
	g++ -std=c++11 -c -pthread http_server.cpp event_loop_thread_pool.cpp event_loop_thread.cpp event_loop.cpp

socket.o: socket.cpp socket.h 
	g++ -std=c++11 -c -pthread socket.cpp

event_loop_thread_pool.o: event_loop_thread_pool.cpp event_loop_thread.h 
	g++ -std=c++11 -c -pthread event_loop_thread_pool.cpp event_loop_thread.cpp

event_loop_thread.o: event_loop_thread.cpp event_loop_thread.h event_loop.h thread.h 
	g++ -std=c++11 -c -pthread event_loop_thread.cpp event_loop.cpp thread.cpp

thread.o: thread.cpp thread.h 
	g++ -std=c++11 -c -pthread thread.cpp

event_loop.o: event_loop.cpp event_loop.h epoll.h 
	g++ -std=c++11 -c event_loop.cpp epoll.cpp

parse.o: parse.cpp parse.h 
	g++ -std=c++11 -c parse.cpp

handler.o: handler.cpp handler.h parse.h 
	g++ -std=c++11 -c handler.cpp parse.cpp

epoll.o: handler.cpp handler.h parse.h epoll.h 
	g++ -std=c++11 -c epoll.cpp handler.cpp parse.cpp

buffer.o: buffer.cpp buffer.h 
	g++ -std=c++11 -c buffer.cpp

clean:
	rm *.o http_server
