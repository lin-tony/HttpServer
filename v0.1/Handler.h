#ifndef HANDLER_H
#define HANDLER_H

#include <string>
#include <algorithm>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Parse.h"
#include "Buffer.h"

class Handler{
public:
	Handler(const int fd);
	~Handler();
	void handle();
	const int connFd() const{
		return _connFd;
	}

private:
	bool receiveRequest();
	void sendResponse();
	void sendErrorMsg(const std::string&, const std::string&, const std::string&);
	void parseURI();
	void getFileType();
	
	int _connFd;
	bool _isClosed;
	std::string _fileType;
	std::string _fileName;
	Buffer _inputBuffer;
	Buffer _outputBuffer;
	HttpRequest _request;
};

#endif