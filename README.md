# HttpServer
A simple HTTP static file server, which is written on C++11 language. Use Reactor model.

# Usage
- `$ make`
- `$ ./HttpServer <port>`
- `$ make clean`

# Detail

|file|feature|
|:----:|:-------:|
|get_cf_pro.py|collect codeforces problems' solved information|
|vjudge.py|update/create contest on vjudge.net automatic|
|connect_db.py|customize db connection library implement from pymysql|
|log.py|customize log library implement from logging|
|crontab|crontab script template|
|table.sql|database structure|

- HttpServer: 主函数，MainReactor创建监听套接字并监听，当连接套接字后，选择一个SubReactor将已连接套接字注册到其EventLoop中。
- Socket: 封装对套接字的各种操作，包括创建，绑定，监听等。
- Epoll.cpp Epoll.h: 封装epoll的各种操作的类。
- EventLoop: 事件循环，循环会不断获取Epoll类返回的活动套接字并交给Handler类处理。
- Thread: 封装pthread。
- EventLoopThread: 实现EventLoop和Thread的绑定，使SubReactor运行。
- EventLoopThreadPool: 线程池。
- Parse: 用于解析客户端HTTP请求。
- Buffer: 缓冲区，在套接字和缓冲区之间读写，缓存请求和相应的内容。
- Handler: 处理HTTP请求并返回。

# IO并发模型
###
同步IO模型
### reactor模式的工作流程：
1. 主线程向epoll内核事件表内注册socket上的可读就绪事件。
2. 主线程调用epoll_wait()等待socket上有数据可读
3. 当socket上有数据可读，epoll_wait 通知主线程。主线程从socket可读事件放入处理队列。
4. 睡眠在处理队列上的某个可读工作线程被唤醒，从socket上读取数据，处理客户的请求。然后向epoll内核事件表加入写的就绪事件。
5. 主线程调用epoll_wait()等待数据可写。

### 此项目的reactor模式概述：
- MainReactor只有一个，负责响应client的连接请求，并建立TCP连接。在建立连接后用轮转方式分配到某一个SubReactor的监听队列中，
- SubReactor有多个，每个subReactor都会在一个独立线程中运行，并且维护一个独立的NIO Selector。
- 当主线程把新连接分配给了某个SubReactor，该线程此时可能正阻塞在多路选择器(epoll)的等待中，线程每隔1000ms会从epoll_wait中醒来，得到活跃事件，进行处理。

具体模型如图
![并发模型](https://github.com/lin-tony/HttpServer/blob/master/Reactor-model.png)


# 关于epoll工作模式
epoll的触发模式使用了ET模式。ET模式每次读，必须读到不能再读(出现EAGAIN)，每次写，写到不能再写(出现EAGAIN)。ET模式下事件被触发的次数比LT模式下少很多。

但是此项目只是使用Http短连接，读取请求和返回响应后便关闭了套接字，所以ET模式在此项目和LT并无差别，仅当练手。


# To do list
- <del>将LT改为ET模式</del>（完成，更新了Buffer::readFd和Epoll::addToEpoll，此前已在Socket::Accept中用了setNonBlock(设置非阻塞)）
- 增加condition和mutex（因为涉及到跨线程任务分配，需要加锁，这里的锁由某个特定线程中的loop创建，只会被该线程和主线程竞争。）
