#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "util.hpp"
// #include "parse.hpp"

namespace ns_index {
    using inverte_list = std::vector<inverte_elem>;
    struct doc_info {
        std::string title;
        std::string text;
        std::string url;
        uint64_t doc_id;
    };
    struct inverte_elem {
        uint64_t doc_id;
        std::string word;
        int weights;        // 暂时由word在标题中出现的次数和正文中出现的次数决定
    };

    class index {
    public:
        index() = default;
        ~index() = default;
        //更具id找到文档内容
        doc_info* get_forward_index(uint64_t doc_id) {
            if (doc_id >= forward_index.size()) {
                std::cerr << "doc_id out of range!" << '\n';
                return nullptr;
            }
            return &forward_index[doc_id];
        }
        //根据关键词找到倒排内容
        inverte_list* get_inverte_list(const std::string &keyword) {
            auto it = inverte_index.find(keyword);
            if (it == inverte_index.end()) {
                std::cerr << keyword << "have no inverte_list" << '\n';
                return nullptr;
            }
            return &(it->second);
        }

        //由data/raw.html/raw.txt文档内容构建倒排正排
        bool build_index(const std::string &file) {
            std::ifstream reader(file, std::ios::in | std::ios::binary);
            if (!reader.is_open()) {
                std::cerr << file << "open failed" << '\n';
                return false;
            }
            std::string line;
            while(std::getline(reader, line)) {
                doc_info* ret = build_forward_index(line);
                if (ret == nullptr) {
                    std::cerr << "build" << line << "error";    //for debug
                    continue;
                }
                build_inverte_list(*ret);
            }
        }
    private:
        doc_info* build_forward_index(const std::string& line) {
            doc_info doc;
            char sep = '\3';
            if (util::string_util::substring(line, &doc, sep)) { return nullptr; }
            doc.doc_id = forward_index.size();
            forward_index.push_back(std::move(doc));
            return &forward_index.back();
        }
        bool build_inverte_list(const doc_info& odc) {
             //jieba分词
            
             //词频统计

             //填入inverte_index
        }

    private:
        std::vector<doc_info> forward_index;    //正排索引
        std::unordered_map<std::string, inverte_list> inverte_index;   //倒排索引
    };
}