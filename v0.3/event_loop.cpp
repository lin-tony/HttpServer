#include "event_loop.h"

EventLoop::EventLoop()
	:is_quit_(false),
	 epoll_(new Epoll()){
}

void EventLoop::Loop(){
	is_quit_ = false;
	thread_id_ = static_cast<pid_t>(syscall(SYS_gettid));
	while(!is_quit_){
		//std::cout << "--------Thread: " << thread_id_ << " Looping" << std::endl;
		AddToEpoll();
		std::vector<Handler*> active_events;
		active_events.clear();
		
		epoll_->EpollWaitToGetActive(active_events);
		for(std::vector<Handler*>::iterator it = active_events.begin(); it != active_events.end(); ++it){
			//std::cout << "-----------Handle Request!" << std::endl;
			(*it)->HandleRequest();
			epoll_->RemoveFd((*it)->get_connect_fd());
			delete* it;
		}
	}
}

void EventLoop::Quit(){
	is_quit_ = true;
}

void EventLoop::AddToFdVector(const int fd){
	std::cout << "Thread: " << thread_id_ << " add socket: " << fd << " to Loop." << std::endl;
	//使用lock_guard则相对安全，它是基于作用域的，能够自解锁，当该对象创建时，它会像m.lock()一样获得互斥锁，当生命周期结束时，它会自动析构(unlock)，不会因为某个线程异常退出而影响其他线程
	std::lock_guard<std::mutex> lockGuard(mutex_);
	fd_vector_.push_back(fd);
}

void EventLoop::AddToEpoll(){
	std::lock_guard<std::mutex> lockGuard(mutex_);
	if(fd_vector_.empty()){
		//std::cout <<  "Thread: " << thread_id_ << " fd_vector_ empty." << std::endl;
	}else{
		for(int i = 0; i < fd_vector_.size(); ++i){
			epoll_->AddToEpollQueue(fd_vector_[i]);
		}
		fd_vector_.clear();
		std::cout <<  "Thread: " << thread_id_ << " add all fd into Loop" << std::endl;
	}
}
