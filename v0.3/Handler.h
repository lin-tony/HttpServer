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
#include "parse.h"
#include "buffer.h"

class Handler{
public:
	Handler(const int fd);
	~Handler();
	void HandleRequest();
	const int get_connect_fd() const{
		return connect_fd_;
	}

private:
	bool ReceiveRequest();
	void SendResponse();
	void SendErrorMsg(const std::string&, const std::string&, const std::string&);
	void ParseURI();
	void GetFileType();
	
	int connect_fd_;
	bool is_close_;
	std::string file_type_;
	std::string fileName_;
	Buffer input_buffer_;
	Buffer output_buffer_;
	HttpRequest request_;
};

#endif
