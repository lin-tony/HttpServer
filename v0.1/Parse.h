#ifndef PARSE_H
#define PARSE_H

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <cstring>

struct HttpRequest{
	std::string method; //方法
	std::string uri; //uri
	std::string version; // http版本
	std::string host; //请求 主机名
	std::string connection; //Conntect首部
};

class Parse{
public:
	Parse(const std::string request);
	HttpRequest getParseResult();
private:
	void parseLine();
	void parseRequestLine();
	void parseHeaders();

	std::string _request;
	std::vector<std::string> _line;
	HttpRequest _parseResult;
};


#endif