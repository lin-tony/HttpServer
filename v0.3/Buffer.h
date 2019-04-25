#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <signal.h>

class Buffer{
public:
	Buffer();
	size_t ReadableBytes();
	size_t WriteableBytes();
	std::string ReadAllAsString();
	void Append(const char* data, const size_t len);
	char* GetDataIndex();
	size_t ReadFd(const int fd);
	void SendFd(const int fd);
	void ReadFileAndWriteBack(const int from_fd, const int to_fd);

private:
	void ResetBuffer();
	void MakeSpace(const size_t len);
	const char* GetBegin() const;
	char* BeginWrite();

	std::vector<char> buffer_;
	size_t readIndex_, writeIndex_;
};

#endif
