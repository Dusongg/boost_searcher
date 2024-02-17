#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "index.hpp"

namespace util {
    class file_util {
    public:
        static bool read_file(const std::string file_path, std::string *out) {
            std::ifstream reader(file_path, std::ios::in);
            if (!reader.is_open()) {
                std::cerr << "open file" << file_path << "error" << '\n';
                return false;
            }
            std::string line;
            while(std::getline(reader, line)) {
                *out += line;
            }
            reader.close();
            return true;
        }  
    };
    class string_util {
    public:
        static bool substring(const std::string& line, ns_index::doc_info* doc, const char sep) {
            boost::split(tmp, line, boost::is_any_of(sep), boost::token_compress_on);
            if (tmp.size() < 3) return false;
            doc->title = std::move(tmp[0]);
            doc->text = std::move(tmp[1]);
            doc->url = std::move(tmp[2]);
            tmp.clear();
            return true;
        }
        private:
        static std::vector<std::string> tmp;
    };
}




