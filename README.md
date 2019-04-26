# HttpServer
A simple HTTP static file server, which is written on C++11 language. Use Reactor model.

# 索引
- [Usage](#Usage)
- [Detail](#Detail)
- [Reactor模式概述](#Reactor模式概述)
- [版本历史](#版本历史)
- [压力测试](#压力测试)


# Technical points

1. 使用Reactor模式，满足高并发需求
2. 使用Epoll边沿触发的IO多路复用技术，非阻塞IO，减少事件触发次数
3. 使用线程池实现多线程模块，充分利用多核CPU，避免线程频繁创建销毁的开销 
4. 主线程只负责accept请求，返回后轮转分发给线程池中子线程处理，锁的争用只会出现在主线程和某一线程中


# Usage
- `$ make`
- `$ ./HttpServer [-t thread_numbers] [-p port]`
- `$ make clean`

# Detail

|file|feature|
|:----:|:-------:|
|HttpServer | 主函数，MainReactor创建监听套接字并监听，当连接套接字后，选择一个SubReactor将已连接套接字注册到其EventLoop中|
|Socket | 封装对套接字的各种操作，包括创建，绑定，监听等|
|Epoll | 封装epoll的各种操作的类|
|EventLoop | 事件循环，循环会不断获取Epoll类返回的活动套接字并交给Handler类处理|
|Thread | 封装pthread，控制处理线程的开始与结束|
|EventLoopThread | 实现EventLoop和Thread的绑定，维护SubReactor的运行|
|EventLoopThreadPool | EventLoopThread的线程池|
|Parse | 用于解析客户端HTTP请求|
|Buffer | 缓冲区，缓存请求和响应的内容，将请求的文件放入缓冲区，将缓冲区的内容发送到对应套接字|
|Handler| 控制处理HTTP请求并返回响应|
|Mutex| 互斥锁|


# Reactor模式概述：
- MainReactor只有一个，负责响应client的连接请求，并建立TCP连接。在建立连接后用轮转方式分配到某一个SubReactor的监听队列中，
- SubReactor有多个，每个subReactor都会在一个独立线程中运行，并且维护一个独立的NIO Selector。
- 当主线程把新连接分配给了某个SubReactor，该线程此时可能正阻塞在多路选择器(epoll)的等待中，线程最多每隔1000ms会从epoll_wait中醒来，得到所有活跃事件，进行处理。

具体模型如图
![并发模型](https://github.com/lin-tony/HttpServer/blob/master/Reactor-model.png)



# 关于epoll工作模式
epoll的触发模式使用了ET模式。ET模式每次读，必须读到不能再读(出现EAGAIN)，每次写，写到不能再写(出现EAGAIN)。ET模式下事件被触发的次数比LT模式下少很多。

此项目只是使用Http短连接，读取请求和返回响应后便关闭了套接字，不会有第二次触发，所以ET模式在此项目和LT并无差别，仅当练手。


# 版本历史
C++实现的高性能Http服务器。可解析响应get、head请求，可处理静态资源。


## v0.1
第一版是看了很多Github上的Http服务器，结合自己的理解写出来的。模型结构如下：
- 使用Epoll边沿触发的IO多路复用技术，非阻塞IO，减少事件触发次数
- 使用Reactor模式，满足高并发需求
- 使用线程池实现多线程模块，减少系统资源开销
- 解析了HTTP的get请求，还可处理静态资源。
- 在将套接字加入epoll监听队列的操作上加了互斥锁

## v0.2
主要是运用C++11的知识
- 将子线程全部由pthread改为std::thread
- 在互斥锁上用了std::lock_guard增加稳健性，线程获得锁并崩溃后会自动释放锁，然而崩溃后整个进程会结束
- 已将epoll_wait()改为1ms阻塞，经测试在1核空转的情况下开200线程占用50%cpu，user和system各占一半

## v0.3
- 增加程序的健壮性，改进了Buffer类，增加 捕获SIGPIPE 和 write缓冲区满时，阻塞等待write释放 的功能。
- 测试了程序性能
- 根据Google C++命名规范修改所有命名，文档https://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/ ，参考了https://www.cnblogs.com/chensheng-zhou/p/5127415.html  
- 增加异常处理机制



# 压力测试

## 使用postman
结果发现postman的请求是串行的，但是至少测试得出的结果是服务器可以长时间正常运作。


## 使用http_load
__测试环境__
- OS：Ubuntu 16.04
- 内存：2G
- CPU：I5-7300HQ
- 远程访问，云主机1核，Http服务器子线程数量1，带宽1Mbps

从下面的输出可以知道：
运行了5695个请求，最大的并发进程数是18，总计传输的数据是2.26581e+06 bytes，运行了60.0072秒
每一连接平均传输的数据量397.86 bytes
每秒的响应请求为94.9053个，每秒传递的数据为37 759.1 bytes
每连接的平均响应时间是43.0704 毫秒，最大的响应时间3039.73 毫秒，最小的响应时间18.302 毫秒
有两个连接丢失了
```shell
$ http_load -s 60 -r 1000 url.txt 

5695 fetches, 18 max parallel, 2.26581e+06 bytes, in 60.0072 seconds
397.86 mean bytes/connection
94.9053 fetches/sec, 37759.1 bytes/sec
msecs/connect: 43.0704 mean, 3039.73 max, 18.302 min
msecs/first-response: 25.9838 mean, 317.248 max, 17.91 min
2 bad byte counts
HTTP response codes:
  code 200 -- 5693
```

__总结__
- 这个工具在时间到的时候居然会发几个头部直接是空行的请求，也找出了我程序的Bug，因为我按照接收正常请求去解析，结果导致了内存溢出，进程直接崩溃。
- 发现accept返回的fd是从1自增的，总体来说基本性能还是OK的，但是还没到高并发的测试要求，因为测试工具不是太给力（因为只要大量的请求没有返回，进程会缓冲区溢出）。
- 测试的虚拟机使用了1核，最大并发线程数和请求速率其实受到了客户机的CPU处理上限和服务器带宽的限制。

总体上还是没有达到我想要的效果，下次试试别的工具。（发现居然带宽超过了1Mbps，也不知道是误差还是啥问题呢。。）

参考https://www.cnblogs.com/cqingt/p/6043126.html


## 使用webbench

理想的测试环境是两台计算机，带宽无限，然而这种情况应该只有本地测试才能勉强达到，否则用以太网都会被限制100Mbps，所以最终选择了本地单机测试，缺点就是所有CPU要是被占满了就会相互影响。

__测试环境__
- OS：Ubuntu 16.04
- 内存：2G
- CPU：I5-7300HQ
- 虚拟机，4核，Http服务器子线程数8.

从下面的输出可以知道：
测试进程并发进程数是3000，跑了20秒，
平均每秒钟响应请求数28794个，每秒钟传输数据量13878611字节，
返回数：575885次返回成功，0失败。
```shell
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://localhost:8088/
3000 clients, running 20 sec.

Speed=1727655 pages/min, 13878611 bytes/sec.
Requests: 575885 susceed, 0 failed.
```

__总结__
- 所担心的占满所有CPU的情况并没有发生，极限大概是占用180%CPU
- `ulimit -n`设置的最多似乎是1e7，结果直接跑炸了，所以这次用`sysctl -w fs.file-max=31234567`解决文件描述符上限问题
- 测试进程时间到的时候会直接关闭所有socket，导致server的write直接中断，因为之前没用捕获SIGPIPE，所以程序会直接崩溃，已修复。

这次的测试很顺利，通过top可以看到没有内存溢出，服务器承载的压力也非常大了，剩下的大方向就是升级到Http1.1了。

参考：https://blog.csdn.net/jk110333/article/details/9190687


# C++11元素
std::mutex
std::lock_guard
std::thread
auto
lambda


# To do list
- <del>将LT改为ET模式</del>（完成，更新了Buffer::readFd和Epoll::addToEpoll，此前已在Socket::Accept中用了setNonBlock(设置非阻塞)）
- <del>增加condition和mutex</del>（更新EventLoop类，在主线程将连接好的套接字加入到子线程等待队列中时加锁 和 子线程将等待队列中的套接字加入到监听队列中）
- <del>挂上云服务器</del>
- C++11线程池改造(使用了std::thread，还没完工)
- 处理head请求
- 压力测试
- 将Http改为长连接，以测试ET模式的性能
- 增加基于连接状态的socket调度，主动释放超时连接
- 静态文件路径检测
- 增加其他安全措施
- 解决大文件的解析



__未能解决的Bug__

135K的图片在本地可以完美解析，但是放到1Mbps的云服务器上再访问永远只能显示一半，
用WINHEX查看文件，发现乱序了，不同次请求的乱序不一样，然而明显socket封装了底层的tcp流传输啊，我只有write可以调用。
