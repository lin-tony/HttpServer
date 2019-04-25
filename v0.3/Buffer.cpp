#include "buffer.h"

Buffer::Buffer(): readIndex_(0), writeIndex_(0) {
	//捕获SIGPIPE信号, 给它设置SIG_IGN信号处理函数：对端已经关闭.errno置为SIGPIPE
	signal(SIGPIPE, SIG_IGN);
}

size_t Buffer::ReadableBytes(){
	return writeIndex_ - readIndex_;
}

size_t Buffer::WriteableBytes(){
	return buffer_.size() - writeIndex_;
}

std::string Buffer::ReadAllAsString(){
	std::string str(&*buffer_.begin() + readIndex_, ReadableBytes());
	ResetBuffer();
	return str;
}

void Buffer::Append(const char* data, const size_t len){
	MakeSpace(len);
	std::copy(data, data+len, BeginWrite());
	writeIndex_ += len;
}

char* Buffer::GetDataIndex(){
	return static_cast<char*>(&*buffer_.begin()) + readIndex_;
}

void Buffer::ResetBuffer(){
	readIndex_ = writeIndex_ = 0;
	buffer_.clear();
	buffer_.shrink_to_fit();
}

void Buffer::MakeSpace(const size_t len){
	if(WriteableBytes() < len){
		buffer_.resize(writeIndex_ + len);
	}
}

char* Buffer::BeginWrite(){
	return &*buffer_.begin() + writeIndex_;
}

const char* Buffer::GetBegin() const{
	return &*buffer_.begin();
}

size_t Buffer::ReadFd(const int fd){

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
		Append(ptr, nread);//只将新增的写入

        ptr += nread;
    }
	return readCount;
}

void Buffer::SendFd(const int fd){
	size_t bytesSent;
	size_t readableCount = ReadableBytes();
	char* ptr = GetDataIndex();
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
	ResetBuffer();
}

void Buffer::ReadFileAndWriteBack(const int fromFd, const int toFd){
	int nread, nsend;

	/*//本来还想根据文件大小计算，得知read文件尾直接返回0便不需要了
	struct stat stbuf;
	if ((fstat(fromFd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) {
		  return;
	}
	off_t file_size;
    file_size = stbuf.st_size;
	*/
	char supportBuf[655350];
	char* ptr;
	while(true){
		ptr = supportBuf;
		//读文件
		nread = read(fromFd, ptr, 655350);
		//std::cout<<" nread bytes:"<<nread <<std::endl;
		if (nread < 0){
            if (errno == EINTR){
                nread = 0;
            } else{
                break;
            }  
        }
        if (nread == 0)//读取完成
            break;

		//写进套接字
        int readableCount = nread;
        while(readableCount){
			if((nsend = write(toFd,ptr,readableCount)) < 0){
				if(errno == EINTR){
					nsend = 0;
				} else if(errno == EAGAIN){
					//缓冲区满了，所以阻塞20ms，再看能不能写
					struct timeval delay;
					delay.tv_sec = 0;
					delay.tv_usec = 20 * 1000; // 20 ms
					select(0, NULL, NULL, NULL, &delay);
				} else
					return;
			}
			readableCount -= nsend;
			ptr += nsend;
			//std::cout<<" send bytes:"<<nsend <<std::endl;
		}
		
	}
}