#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include "util.hpp"

const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";

typedef struct docInfo {
    std::string title;
    std::string text;
    std::string url;
}docInfo_t;



bool parse_title(const std::string& content, std::string *title) {
    std::size_t begin = content.find("<title>") + std::string("<title>").size();
    if (begin == std::string::npos) { return false; }
    std::size_t end = content.find("</title>");
    if (end == std::string::npos) { return false; }
    if (begin > end) { return false; }

    *title = content.substr(begin, end - begin);
    return true;
}
bool parse_text(const std::string& content, std::string *text) {
    enum state {
        LABLE,
        TEXT
    };
    enum state st = LABLE;
    for (char c : content) {
        if (st == LABLE) {
            if (c == '>') { st = TEXT; }
        } 
        else if (st == TEXT) {
            if (c == '<') { st = LABLE; }
            else { 
                if (c == '\n') { c == ' '; }
                *text += c; 
            }
        }
    }
    return true;

}

bool enum_file(const std::string& src_path, std::vector<std::string> *files_list) {
    namespace fs = boost::filesystem;
    fs::path root_path(src_path);   //根路径
    if (!fs::exists(root_path)) {
        std::cerr << src_path << "not exists" << '\n';
        return false;
    }

    fs::recursive_directory_iterator end;
    for (fs::recursive_directory_iterator it(root_path); it != end; it++) {   //递归
        if (!fs::is_regular_file(*it)) continue;    //跳过其他类型文件
        if (it->path().extension() == ".html") {        //筛选文件后缀为html的文件
            // std::cout << "debug:" <<  it->path().string() << std::endl;  
            files_list->push_back(it->path().string());
        }
    }
    return true;

    
}

bool parse_html(std::vector<std::string>& files_list, std::vector<docInfo_t> *results) {
    for (auto& file : files_list) {
        //1.读文件，提取带标签的html内容
        std::string doc_content;
        if (!util::file_util::read_file(file, &doc_content)) continue;

        docInfo_t doc_info;
        //2.提取标题
        if (!parse_title(doc_content, &doc_info.title)) continue;

        //3.提取正文
        if (!parse_text(doc_content, &doc_info.text)) continue;
        
        //4.构建url
        doc_info.url = "https://www.boost.org/doc/libs/1_84_0/doc/html/" + file.substr(src_path.length());

        std::cout << "title " << doc_info.title << "text: " << doc_info.text << "url: " << doc_info.url << '\n';

        results->emplace_back(std::move(doc_info));

    }
    return true;
}

bool save_html(const std::vector<docInfo_t>& results, const std::string& output) {
    return true;
}