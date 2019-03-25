#include "Buffer.h"

Buffer::Buffer(): _readIndex(0), _writeIndex(0) {}

size_t Buffer::readableBytes(){
	return _writeIndex - _readIndex;
}

size_t Buffer::writeableBytes(){
	return _buffer.size() - _writeIndex;
}

std::string Buffer::readAllAsString(){
	std::string str(&*_buffer.begin() + _readIndex, readableBytes());
	resetBuffer();
	return str;
}

void Buffer::append(const char* data, const size_t len){
	makeSpace(len);
	std::copy(data, data+len, beginWrite());
	_writeIndex += len;
}

char* Buffer::getDataIndex(){
	return static_cast<char*>(&*_buffer.begin()) + _readIndex;
}

void Buffer::resetBuffer(){
	_readIndex = _writeIndex = 0;
	_buffer.clear();
	_buffer.shrink_to_fit();
}

void Buffer::makeSpace(const size_t len){
	if(writeableBytes() < len){
		_buffer.resize(_writeIndex + len);
	}
}

char* Buffer::beginWrite(){
	return &*_buffer.begin() + _writeIndex;
}

const char* Buffer::getBegin() const{
	return &*_buffer.begin();
}

size_t Buffer::readFd(const int fd){

	char supportBuf[65535];
	char* ptr = supportBuf;
	int nleft = 65535;
	int readCount, nread;
	//ET模式，必须全部读完
	while (nleft > 0){
		nread = read(fd, ptr, nleft);
		//std::cout << "````````````````nread="<<nread<<std::endl;
        if (nread < 0){
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN){//读取完成！
                return readCount;
            }
            else{
                return 0;
            }  
        }
        else if (nread == 0)
            break;

        nleft -= nread;
        readCount += nread;
		append(ptr, nread);//只将新增的写入

        ptr += nread;
    }
	return readCount;
}

void Buffer::sendFd(const int fd){
	size_t bytesSent;
	size_t readableCount = readableBytes();
	char* ptr = getDataIndex();
	while(readableCount){
		if((bytesSent = write(fd,ptr,readableCount)) < 0){
			if(bytesSent < 0 && errno == EINTR)
				bytesSent = 0;
			else
				return;
		}
		readableCount -= bytesSent;
		ptr += bytesSent;
	}
	resetBuffer();
}
