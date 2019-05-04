#include "parse.h"

Parse::Parse(const std::string request)
	:request_(request){
}

HttpRequest Parse::GetParseResult(){
	ParseLine();
	ParseRequestLine();
	ParseHeaders();
	return parse_result_;
}

void Parse::ParseLine(){
	std::string::size_type lineBegin = 0;
	std::string::size_type checkIndex = 0;

	while(checkIndex < request_.size()){
		if(request_[checkIndex] == '\r'){
			if((checkIndex + 1) == request_.size()){//因为最后一个字符是\n，所以必定错误
				std::cout << "Parse::parseLine error.  can not read complete." << std::endl;
				return;
			}else if(request_[checkIndex + 1] == '\n'){//正确的读到了完整的一行
				line_.push_back(std::string(request_, lineBegin, checkIndex - lineBegin));
				checkIndex += 2;//跳过\r\n
				lineBegin = checkIndex;
			}else{
				std::cout << "Parse::parseLine error." << std::endl;
				return;
			}
		}else{
			++checkIndex;
		}
	}
}

void Parse::ParseRequestLine(){
	std::string requestLine = line_[0];

	//first white space 指向请求行的第一个空白字符
	auto first_ws = std::find_if(requestLine.cbegin(), requestLine.cend(),
									[](char c)->bool{ return ( c==' ' || c=='\t' ); });

	if(first_ws == requestLine.cend()){
        std::cout << "Request error." << std::endl;
        return;
	}

	// 请求方法
	parse_result_.method = std::string(requestLine.cbegin(), first_ws);

	//reverse_last_ws 指向请求行最后一个空白字符
	auto reverse_last_ws = std::find_if(requestLine.crbegin(), requestLine.crend(),
									[](char c)->bool { return ( c==' ' || c=='\t' ); });

	//base() :通过reverse_iterator得到iterator
	auto last_ws = reverse_last_ws.base();
	parse_result_.version = std::string(last_ws, requestLine.cend());

	while((*first_ws == ' ' || *first_ws == '\t') && first_ws != requestLine.cend()){
		++first_ws;
	}
	--last_ws;
	while((*last_ws == ' ' || *last_ws == '\t') && last_ws != requestLine.cbegin()){
		--last_ws;
	}

	parse_result_.uri = std::string(first_ws, last_ws+1);
}

void Parse::ParseHeaders(){
	for(int i = 1; i < line_.size(); ++i){
		if(line_[i].empty()){//遇到空行则解析完毕
			return;

		//以下为http1.1做铺垫
		}else if(strncasecmp(line_[i].c_str(), "Host:", 5) == 0){//比较参数s1和s2字符串前n个字符，相同返回0
			auto iter = line_[i].cbegin() + 5;
			while(*iter == ' ' || *iter == '\t')
				++iter;
			parse_result_.host = std::string(iter, line_[i].cend());
		}else if(strncasecmp(line_[i].c_str(), "Connection:", 11) == 0){
			auto iter = line_[i].cbegin() + 11;
			while(*iter == ' ' || *iter == '\t')
				++iter;
			parse_result_.connection = std::string(iter, line_[i].cend());
		}else{
			continue;//其他先忽略
		}
	}
}