#include "Handler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <cstdlib>

Handler::Handler(const int fd)
:_connFd(fd),
_isClosed(false){
}

Handler::~Handler(){
	if(!_isClosed){
		close(_connFd);
	}
}

void Handler::handle(){
	if(!receiveRequest()){
		close(_connFd);
		_isClosed = true;
		return;
	}
	if(_request.method != "GET"){
		//std::cout << "501 Not Implemented" << std::endl;
		sendErrorMsg("501", "Not Implemented","Not implemented, Sorry");
		return;
	}
	parseURI();

	//查看文件是否存在
	struct stat fileInfo;
	if(stat(_fileName.c_str(), &fileInfo) < 0){
		//std::cout << "404 Not Found" << std::endl;
		sendErrorMsg("404", "Not Found", "Not Found, Sorry");
		return;
	}

	//检查文件 是否是一个常规文件  是否可读
	if(!(S_ISREG(fileInfo.st_mode)) || !(S_IRUSR & fileInfo.st_mode)){
		//std::cout << "403 Forbidden" << std::endl;
        sendErrorMsg("403", "Forbidden", "Couldn't read this file");
        return;
	}

	getFileType();
	std::string msg = "HTTP/1.0 200 OK\r\n";
	msg += "Server: Web Server\r\n";
	msg += "Content-type:" + _fileType + "\r\n";
	

	int fileFd = open(_fileName.c_str(), O_RDONLY, 0);

	// //本来还想根据文件大小计算，得知read文件尾直接返回0便不需要了
	struct stat stbuf;
	if ((fstat(fileFd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
		  return;
	}
	off_t file_size;
    file_size = stbuf.st_size;
    std::string s = std::to_string((int)file_size);
    msg += "Content-Length: " + s + "\r\n";
    msg += "\r\n";
    _outputBuffer.append(msg.c_str(), msg.size());

	//先把响应头发回去，再传文件
	_outputBuffer.sendFd(_connFd);
	_outputBuffer.readFileAndWrite(fileFd, _connFd);

	close(_connFd);
	_isClosed = true;
}

bool Handler::receiveRequest(){
	if(_inputBuffer.readFd(_connFd) == 0){
		return false;
	}

	std::string request = _inputBuffer.readAllAsString();
	std::cout << "------------Get Request----------" << std::endl;
	std::cout << request << std::endl;
	std::cout << "------------End of Request-------" << std::endl;

	if(request.find("\r\n\r\n") == std::string::npos)//直接丢掉没有正常结束的请求
		return false;

	Parse p(request);
	_request = p.getParseResult();
	return true;
}

void Handler::sendErrorMsg(const std::string &errorNum,
						const std::string &shortMsg,
						const std::string &longMsg){
	std::string msg = "HTTP /1.0" + errorNum + " " + shortMsg + "\r\n";
	msg += "Content-type: text/html\r\n";
	msg += "<html><title>Error</title>";
	msg += "<body>"+errorNum+": "+shortMsg+"\r\n";
	msg += "<p>"+longMsg+"</p></body></html>\r\n\r\n";

	_outputBuffer.append(msg.c_str(), msg.size());
	_outputBuffer.sendFd(_connFd);
	close(_connFd);
	_isClosed = true;
}

void Handler::parseURI(){
	_fileName = ".";
	if(_request.uri == "/")
		_fileName += "/index.html";
	else
		_fileName += _request.uri;
}

void Handler::getFileType(){
	const char* name = _fileName.c_str();
	if(strstr(name, ".html")){
		_fileType = "text/html";
	}else if(strstr(name, ".jpg")){
		_fileType = "image/jpg";
	}else if(strstr(name, ".png")){
		_fileType = "image/png";
	}else{
		_fileType = "text/plain";
	}
}
