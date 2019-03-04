#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

class Buffer{
public:
	Buffer();
	size_t readableBytes();
	size_t writeableBytes();
	std::string readAllAsString();
	void append(const char* data, const size_t len);
	char* getDataIndex();
	size_t readFd(const int fd);
	void sendFd(const int fd);

private:
	void resetBuffer();
	void makeSpace(const size_t len);
	const char* getBegin() const;
	char* beginWrite();

	std::vector<char> _buffer;
	size_t _readIndex, _writeIndex;
};

#endif