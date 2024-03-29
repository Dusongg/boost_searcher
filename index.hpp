#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include "util.hpp"
#include <cstdio>
#include <chrono>

namespace ns_index {
    struct doc_info {
        std::string title;
        std::string text;
        std::string url;
        uint64_t doc_id;
    };
    struct inverte_elem {
        uint64_t doc_id;
        std::string word;       //用于摘要
        int weights;        // 暂时由word在标题中出现的次数和正文中出现的次数决定
    };
    using inverte_list = std::vector<inverte_elem>;

    struct word_frequency {
        word_frequency() : title_cnt(0), text_cnt(0) {}
        int title_cnt;
        int text_cnt;
    };

    class index {
    private:
        index() = default;
        index(const index&) = delete;
        index& operator=(const index&) = delete;
        static index* instance;

    public:
        ~index() = default;
        static index* get_instance() {
            if (instance == nullptr) {
                std::lock_guard<std::mutex> lock(init_mutex);
                if (instance == nullptr) {
                    instance = new index;
                }
            }
            return instance;
        }
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
                std::cerr << keyword << " have no inverte_list" << '\n';
                return nullptr;
            }
            return &(it->second);
        }

        //由data/raw.html/raw.txt文档内容构建倒排正排
        #define TOL_HTML 8586
        bool build_index(const std::string &file) {
            std::ifstream reader(file, std::ios::in | std::ios::binary);
            if (!reader.is_open()) {
                std::cerr << file << "open failed" << '\n';
                return false;
            }
            std::string line;
            //打印建立索引进度条
            std::cout << "Indexing" << '\n';
            int cnt = 0;
            char schedule[102]{'\0'};
            char cursor[4]{'|', '/', '-', '\\'};
            auto start_time = std::chrono::steady_clock::now();

            while(std::getline(reader, line)) {
                cnt++;
                printf("[%-100s][%%%d][%c]\r", schedule, cnt * 100 / TOL_HTML, cursor[cnt%4]);

                doc_info* ret = build_forward_index(line);
                if (ret == nullptr) {
                    std::cerr << "build error";    //for debug
                    continue;
                }
                build_inverte_list(*ret);

                schedule[cnt * 100 / TOL_HTML] = '#';
                // std::cout << std::format("已建立{}条索引!\n", ++cnt);
            }
            auto end_time = std::chrono::steady_clock::now();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
            std::cout << "\nIndex creation completed, " << seconds << " s "<< '\n';
            return true;
        }
    private:
        doc_info* build_forward_index(const std::string& line) {           
            doc_info doc;
            std::string sep = "\3";
            std::vector<std::string> twu;
            if (!ns_util::string_util::substring(line, &twu, sep)) { return nullptr; }
            doc.title = twu[0];
            doc.text = twu[1];
            doc.url = twu[2];
            doc.doc_id = forward_index.size();
            forward_index.push_back(std::move(doc));
            return &forward_index.back();
        }

        //一句话->分成多个词->建立词到doc_id以及weights的映射
        bool build_inverte_list(const doc_info& doc) {
             //jieba分词
            std::vector<std::string> title_words, text_words;
            ns_util::jieba_util::cut_string(doc.title, &title_words);
            ns_util::jieba_util::cut_string(doc.text, &text_words);
             //词频统计 
            std::unordered_map<std::string, word_frequency> word_cnt;
            
            for (auto& word : title_words) {
                boost::to_lower(word);
                word_cnt[word].title_cnt++;
            } 
            for (auto& word : text_words) {
                boost::to_lower(word);
                word_cnt[word].text_cnt++;
            }
            //填入inverte_index
            for (auto& [word, cnt] : word_cnt) {
                inverte_elem e;
                e.doc_id = doc.doc_id;
                e.word = word;
                e.weights = 5 * cnt.title_cnt + 1 * cnt.text_cnt;
                inverte_index[word].push_back(std::move(e));
            } 
            return true;
        }

    private:
        std::vector<doc_info> forward_index;    //正排索引
        std::unordered_map<std::string, inverte_list> inverte_index;   //倒排索引
        static std::mutex init_mutex;
    };
    index* index::instance = nullptr;
    std::mutex index::init_mutex;
}
