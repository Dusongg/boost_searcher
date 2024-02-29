#include "index.hpp"
#include <jsoncpp/json/json.h>
#include <algorithm>
#include <set>
namespace ns_searcher {
    class searcher {
    private:
        struct inverte_elem_dep {
            int64_t doc_id;
            int weights;
            std::set<std::string> words;
            inverte_elem_dep() : doc_id(-1), weights(0) {}
            inverte_elem_dep(int64_t id, int w, std::string&& word) : doc_id(id), weights(w) {
                words.emplace(word);
            }
            ~inverte_elem_dep() = default;
        };
    public:
        searcher() = default;
        searcher(const std::string& file_path) { init(file_path); }
        ~searcher() = default;
        
        void search(std::string& qurey, std::string *json_string) {
            using inverte_list = std::vector<ns_index::inverte_elem>;
            //将请求内容转小写
            boost::to_lower(qurey);
            //1. 分词
            std::vector<std::string> words;
            ns_util::jieba_util::cut_string(qurey, &words);
            std::cout << "words_nums: " << words.size() << std::endl;   //debug：分词的数量
            //2.根据分词查找index
            std::vector<inverte_elem_dep> search_result;     //search_result存放get_inverte_list函数返回的invert_elem
            std::unordered_map<int64_t, int> dep;        //去重

            for (auto &word : words) {
                //转小写便于后面提取摘要
                boost::to_lower(word);
                if (word == " ") continue;
                inverte_list* list = id->get_inverte_list(word);    //word->多个doc_id
                if (list == nullptr) {
                    continue;
                }
                // search_result.insert(search_result.end(), list->begin(), list->end());   
                //假设一个html文件内容为："你好/世界，你好/C++"，当搜索"世界/C++"时，同一个doc_id会被搜索出两次
                //所以需要对search的结果关于doc_id去重，同时将同一个doc_id的inverte_elem的weights相加
                for (auto& elem : *list) {
                    if (dep.contains(elem.doc_id)) {    //存在
                        search_result[dep[elem.doc_id]].words.insert(std::move(elem.word));
                        search_result[dep[elem.doc_id]].weights += elem.weights;
                    } else {
                        dep[elem.doc_id] = search_result.size();
                        inverte_elem_dep tmp;
                        tmp.doc_id = elem.doc_id;
                        tmp.weights = elem.weights;
                        tmp.words.insert(std::move(elem.word));
                        search_result.push_back(std::move(tmp)); 
                    }
                }
            }
            std::cout << search_result.size() << std::endl; //debug: 搜索出的文档数量
            //3. 排序
            std::ranges::sort(search_result, [](const inverte_elem_dep &lsh, const inverte_elem_dep &rsh) {
                return lsh.weights > rsh.weights;
            });
            //4. 构建json
            Json::Value root;
            for (auto &inverte_elem : search_result) {
                ns_index::doc_info *doc = id->get_forward_index(inverte_elem.doc_id);       //此处doc_info与parse.hpp中的doc_info不同
                if (doc == nullptr) { continue; }
                Json::Value v;
                v["title"] = doc->title;
                 //一个网页的所有内容，而实际上不需要这么多，只需要摘要, 且摘要中包含关键字word
                v["desc"] = get_desc(doc->text, inverte_elem.words); 
                v["url"] = doc->url;
                root.append(v);
            }
            Json::StyledWriter writer;
            *json_string = writer.write(root);
        }

    private:
        std::string sigle_get_desc(const std::string& text, const std::string& keyword) {
            int pos = text.find(keyword);
            int st_pos = std::max(0, pos - 50), end_pos = std::min(pos + 100, (int)text.size());
            return text.substr(st_pos, end_pos - st_pos) + "...";
        }
        std::string get_desc(std::string text, const std::set<std::string>& keywords) {
            //全部转小写匹配
            boost::to_lower(text);
            
            std::vector<int> id;
            int nxt = text.find(' ');    
            while(nxt != std::string::npos) {
                id.push_back(nxt);
                nxt = text.find(' ', nxt+1);
            }
            //text的开头和结尾
            id.insert(id.begin(), 0);
            id.push_back(text.length());
            //滑动窗口
            int mx  = 0, l = 1, r = 1, tmp = 0, mx_l = -1, mx_r = -1;
            while (r < id.size()) {
                std::string word = text.substr(id[r - 1] + 1, id[r] - id[r - 1] - 1);   
                if (keywords.contains(word)) tmp++;
                while(r > l && id[r] - id[l] > 150) {
                    if (keywords.contains(text.substr(id[l - 1] + 1, id[ l ]-id[l - 1] - 1))) tmp--;
                    l++;
                }
                if (tmp > mx) {
                    mx = tmp;
                    mx_l = l;
                    mx_r = r;
                }
                r++;
            }
            //没结果，说明查找的word在text中是与其他词链接在一起的，用原版本对单个词在text中find
            if (mx_l == -1 || mx_r == -1) return sigle_get_desc(text, *keywords.begin());
            return text.substr(id[mx_l] + 1, id[mx_r] - id[mx_l] - 1) + "...";
        }
        void init(const std::string& file_path) {
            id = ns_index::index::get_instance();
            id->build_index(file_path);
        }
        ns_index::index* id;
    };
}