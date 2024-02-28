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
#include <memory>
#include "ThreadPool.hpp"

namespace ns_index {
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
    using inverte_list = std::vector<inverte_elem>;

    struct word_frequency {
        word_frequency() : title_cnt(0), text_cnt(0) {}
        int title_cnt;
        int text_cnt;
    };

    //声明，用于线程调用
    bool task(int, const std::string&);

    class index {
        friend bool task(int, const std::string&);
        friend doc_info* build_forward_index(const std::string&);
        friend bool build_inverte_list(const doc_info&);
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
            //初始化线程池
            ctpl::thread_pool tp(3);
            while(std::getline(reader, line)) {
                tp.push(task, line);
            }
            return true;
        }

    private:
        static uint64_t cnt_complete;
        static std::vector<doc_info> forward_index;    //正排索引
        static std::unordered_map<std::string, inverte_list> inverte_index;   //倒排索引
        static std::mutex init_mutex;
        static std::mutex forward_index_mutex;
        static std::mutex inverte_index_mutex;
        static std::mutex cnt_mutex;

    };
    //类外初始化静态成员
    uint64_t index::cnt_complete = 0;
    index* index::instance = nullptr;
    std::mutex index::init_mutex;
    std::mutex index::forward_index_mutex;
    std::mutex index::inverte_index_mutex;
    std::mutex index::cnt_mutex;

    std::vector<doc_info> index::forward_index; 
    std::unordered_map<std::string, inverte_list> index::inverte_index; 
        
    doc_info* build_forward_index(const std::string& line) {
        doc_info doc;
        std::string sep = "\3";
        std::vector<std::string> twu;
        if (!ns_util::string_util::substring(line, &twu, sep)) { return nullptr; }
        doc.title = twu[0];
        doc.text = twu[1];
        doc.url = twu[2];

        std::lock_guard<std::mutex> lock(index::forward_index_mutex);
        doc.doc_id = index::forward_index.size();
        index::forward_index.push_back(std::move(doc));
        return &index::forward_index.back();   
    }
    bool build_inverte_list(const doc_info& doc) {
        //jieba分词
        std::unique_ptr<std::vector<std::string>> title_words(new std::vector<std::string>);
        std::unique_ptr<std::vector<std::string>> text_words(new std::vector<std::string>);
        // std::vector<std::string> *title_words = new std::vector<std::string>();
        // std::vector<std::string> *text_words = new std::vector<std::string>();
        cppjieba::Jieba jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH);
        // jieba.CutForSearch(doc.title, *title_words);
        // jieba.CutForSearch(doc.text, *text_words);

        for (int i = 0; i < 100; i++) {
            title_words->push_back(std::to_string(i));
        }
        for (int i = 0; i < 100; i++) {
            text_words->push_back(std::to_string(i));
        }
        //词频统计 
        std::unique_ptr<std::unordered_map<std::string, word_frequency>> word_cnt(new std::unordered_map<std::string, word_frequency>);
        // std::unordered_map<std::string, word_frequency>* word_cnt = new std::unordered_map<std::string, word_frequency>();
        
        for (auto& word : *title_words) {
            boost::to_lower(word);
            (*word_cnt)[word].title_cnt++;
        } 
        for (auto& word : *text_words) {
            boost::to_lower(word);
            (*word_cnt)[word].text_cnt++;
        }
        //填入inverte_index
        for (auto& [word, cnt] : *word_cnt) {
            inverte_elem e;
            e.doc_id = doc.doc_id;
            e.word = word;
            e.weights = 5 * cnt.title_cnt + 1 * cnt.text_cnt;
            index::inverte_index_mutex.lock();
            index::inverte_index[word].push_back(std::move(e));
            index::inverte_index_mutex.unlock();
        } 
        return true;
    }
    char cursor[4]{'|', '/', '-', '\\'};
    char schedule[102]{'\0'};

    bool task(int id, const std::string& line) {
        doc_info* ret = build_forward_index(line);
        if (ret == nullptr) {
            std::cerr << "build error";   
            return false; 
        }
        build_inverte_list(*ret);·
        index::cnt_mutex.lock();
        schedule[index::cnt_complete * 100 / TOL_HTML] = '#';
        index::cnt_complete++;
        printf("[%-100s][%%%d][%c]\r", schedule, index::cnt_complete * 100 / TOL_HTML, cursor[index::cnt_complete%4]);
        index::cnt_mutex.unlock();
        return true;
    }
}
