#include "Parse.h"

Parse::Parse(const std::string request){
	this->_request = request;
}

HttpRequest Parse::getParseResult(){
	parseLine();
	parseRequestLine();
	parseHeaders();
	return _parseResult;
}

void Parse::parseLine(){
	std::string::size_type lineBegin = 0;
	std::string::size_type checkIndex = 0;

	while(checkIndex < _request.size()){
		if(_request[checkIndex] == '\r'){
			if((checkIndex + 1) == _request.size()){//因为最后一个字符是\n，所以必定错误
				std::cout << "Parse::parseLine error.  can not read complete." << std::endl;
				return;
			}else if(_request[checkIndex + 1] == '\n'){//正确的读到了完整的一行
				_line.push_back(std::string(_request, lineBegin, checkIndex - lineBegin));
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

void Parse::parseRequestLine(){
	std::string requestLine = _line[0];

	//first white space 指向请求行的第一个空白字符
	auto first_ws = std::find_if(requestLine.cbegin(), requestLine.cend(),
									[](char c)->bool{ return ( c==' ' || c=='\t' ); });

	if(first_ws == requestLine.cend()){
        std::cout << "Request error." << std::endl;
        return;
	}

	// 请求方法
	_parseResult.method = std::string(requestLine.cbegin(), first_ws);

	//reverse_last_ws 指向请求行最后一个空白字符
	auto reverse_last_ws = std::find_if(requestLine.crbegin(), requestLine.crend(),
									[](char c)->bool { return ( c==' ' || c=='\t' ); });

	//base() :通过reverse_iterator得到iterator
	auto last_ws = reverse_last_ws.base();
	_parseResult.version = std::string(last_ws, requestLine.cend());

	while((*first_ws == ' ' || *first_ws == '\t') && first_ws != requestLine.cend()){
		++first_ws;
	}
	--last_ws;
	while((*last_ws == ' ' || *last_ws == '\t') && last_ws != requestLine.cbegin()){
		--last_ws;
	}

	_parseResult.uri = std::string(first_ws, last_ws+1);
}

void Parse::parseHeaders(){
	for(int i = 1; i < _line.size(); ++i){
		if(_line[i].empty()){//遇到空行则解析完毕
			return;

		//以下为http1.1做铺垫
		}else if(strncasecmp(_line[i].c_str(), "Host:", 5) == 0){//比较参数s1和s2字符串前n个字符，相同返回0
			auto iter = _line[i].cbegin() + 5;
			while(*iter == ' ' || *iter == '\t')
				++iter;
			_parseResult.host = std::string(iter, _line[i].cend());
		}else if(strncasecmp(_line[i].c_str(), "Connection:", 11) == 0){
			auto iter = _line[i].cbegin() + 11;
			while(*iter == ' ' || *iter == '\t')
				++iter;
			_parseResult.connection = std::string(iter, _line[i].cend());
		}else{
			continue;//其他先忽略
		}
	}
}