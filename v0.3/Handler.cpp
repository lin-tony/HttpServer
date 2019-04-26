#include "handler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <cstdlib>

Handler::Handler(const int fd)
	:connect_fd_(fd),
	 is_close_(false){
}

Handler::~Handler(){
	if(!is_close_){
		close(connect_fd_);
	}
}

void Handler::HandleRequest(){
	if(!ReceiveRequest()){
		close(connect_fd_);
		is_close_ = true;
		return;
	}
	if(request_.method != "GET"){
		//std::cout << "501 Not Implemented" << std::endl;
		SendErrorMsg("501", "Not Implemented","Not implemented, Sorry");
		return;
	}
	ParseURI();
	//查看文件是否存在
	struct stat file_info;
	if(stat(fileName_.c_str(), &file_info) < 0){
		//std::cout << "404 Not Found" << std::endl;
		SendErrorMsg("404", "Not Found", "Not Found, Sorry");
		return;
	}

	//检查文件 是否是一个常规文件  是否可读
	if(!(S_ISREG(file_info.st_mode)) || !(S_IRUSR & file_info.st_mode)){
		//std::cout << "403 Forbidden" << std::endl;
        SendErrorMsg("403", "Forbidden", "Couldn't read this file");
        return;
	}

	GetFileType();
	std::string msg = "HTTP/1.0 200 OK\r\n";
	msg += "Server: Web Server\r\n";
	msg += "Content-type:" + file_type_ + "\r\n";
	

	int file_fd = open(fileName_.c_str(), O_RDONLY, 0);

	// //本来还想根据文件大小计算，得知read文件尾直接返回0便不需要了
	struct stat stat_buf;
	if ((fstat(file_fd, &stat_buf) != 0) || (!S_ISREG(stat_buf.st_mode))) {
		  return;
	}
	off_t file_size;
    file_size = stat_buf.st_size;
    std::string s = std::to_string((int)file_size);
    msg += "Content-Length: " + s + "\r\n";


    msg += "\r\n";
    output_buffer_.Append(msg.c_str(), msg.size());

	//先把响应头发回去，再传文件
	output_buffer_.SendFd(connect_fd_);
	output_buffer_.ReadFileAndWriteBack(file_fd, connect_fd_);

	close(connect_fd_);
	is_close_ = true;
}

bool Handler::ReceiveRequest(){
	if(input_buffer_.ReadFd(connect_fd_) == 0){
		return false;
	}

	std::string request = input_buffer_.ReadAllAsString();
	std::cout << "------------Get Request----------" << std::endl;
	std::cout << request << std::endl;
	std::cout << "------------End of Request-------" << std::endl;

	if(request.find("\r\n\r\n") == std::string::npos)//直接丢掉没有正常结束的请求
		return false;

	Parse parse(request);
	request_ = parse.GetParseResult();
	return true;
}

void Handler::SendErrorMsg(const std::string &errorNum,
						const std::string &shortMsg,
						const std::string &longMsg){
	std::string msg = "HTTP /1.0" + errorNum + " " + shortMsg + "\r\n";
	msg += "Content-type: text/html\r\n\r\n";
	msg += "<html><title>Error</title>";
	msg += "<body>"+errorNum+": "+shortMsg+"\r\n";
	msg += "<p>"+longMsg+"</p></body></html>\r\n";

	output_buffer_.Append(msg.c_str(), msg.size());
	output_buffer_.SendFd(connect_fd_);
	close(connect_fd_);
	is_close_ = true;
}

void Handler::ParseURI(){
	fileName_ = ".";
	if(request_.uri == "/")
		fileName_ += "/index.html";
	else{
		fileName_ += request_.uri;
		if(request_.uri[request_.uri.size()-1] == '/')
			fileName_ += "index.html";
	}

	//非常暴力地去掉了版本。。
	int pos = fileName_.find("?v=");
	if(pos != -1){
		fileName_ = fileName_.substr(0,pos);
	}
}

void Handler::GetFileType(){
	const char* name = fileName_.c_str();
	if(strstr(name, ".html")){
		file_type_ = "text/html";
	}else if(strstr(name, ".jpg")){
		file_type_ = "image/jpg";
	}else if(strstr(name, ".png")){
		file_type_ = "image/png";
	}else if(strstr(name, ".js")){
		file_type_ = "application/javascript";
	}else if(strstr(name, ".css")){
		file_type_ = "text/css";
	}else{
		file_type_ = "text/plain";
	}
}