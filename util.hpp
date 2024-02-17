#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "jieba_include/Jieba.hpp"

namespace ns_util {
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
        static bool substring(const std::string& line, std::vector<std::string>* output, const std::string& sep) {
            boost::split(*output, line, boost::is_any_of(sep), boost::token_compress_on);
            if ((*output).size() < 3) return false;
            return true;
        }
    };

    const char* const DICT_PATH = "./dict/jieba.dict.utf8";
    const char* const HMM_PATH = "./dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
    const char* const IDF_PATH = "./dict/idf.utf8";
    const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

    class jieba_util {
    public:
        static void cut_string(const std::string &in, std::vector<std::string> *out) {
            jieba.CutForSearch(in, *out);
        }

    private:
        static cppjieba::Jieba jieba;
    };
    cppjieba::Jieba jieba_util::jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
}




