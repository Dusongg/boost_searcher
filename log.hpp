#pragma once
 
#include <iostream>
#include <string>
#include <ctime>

#define NORMAL 1
#define WAENING 2
#define DEBUG 3
#define FATAL 4

#define LOB(LEVEL, MESSAGE) log(#LEVEL, #MESSAGE, __FILE__, __LINE__)

void log(std::string level, std::string message, std::string file, int line) {
    std::cout << '[' << level << ']'<< '[' << time(nullptr) << ']'<< '[' << message << ']'<< '[' << file << ']'<< ':' << '[' << line << ']'<< '\n';
}


//$ nohup ./http_server > log.txt 2>&1 &   将stdout与stderr都重定向到log.txt文件中