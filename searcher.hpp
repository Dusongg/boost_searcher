#include "index.hpp"
#include <jsoncpp/json/json.h>
#include <algorithm>

namespace ns_searcher {
    class searcher {
    public:
        searcher() = default;
        searcher(const std::string& file_path) { init(file_path); }
        ~searcher() = default;

        void search(const std::string& qurey, std::string *json_string) {
            using inverte_list = std::vector<ns_index::inverte_elem>;
            //1. 分词
            std::vector<std::string> words;
            ns_util::jieba_util::cut_string(qurey, &words);
            //2.根据分词查找index
            inverte_list inverte_elems;
            for (auto &word : words) {
                boost::to_lower(word);
                // std::unordered_map<uint64_t, int> id_weights;       //将iverte_elems中相同doc_id的weights相加
                inverte_list* list = id->get_inverte_list(word);
                if (list == nullptr) {
                    continue;
                }
                inverte_elems.insert(inverte_elems.end(), list->begin(), list->end());
            }

            //3. 排序
            std::ranges::sort(inverte_elems, [](const ns_index::inverte_elem &lsh, const ns_index::inverte_elem &rsh) {
                return lsh.weights > rsh.weights;
            });
            //4. 构建json
            Json::Value root;
            for (auto &inverte_elem : inverte_elems) {
                ns_index::doc_info *doc = id->get_forward_index(inverte_elem.doc_id);       //此处doc_info与parse.hpp中的doc_info不同
                if (doc == nullptr) { continue; }
                Json::Value v;
                v["title"] = doc->title;
                v["desc"] = get_desc(doc->text, inverte_elem.word);  //一个网页的所有内容，而实际上不需要这么多，只需要摘要, 且摘要中包含关键字word
                v["url"] = doc->url;
                root.append(v);
            }
            Json::StyledWriter writer;
            *json_string = writer.write(root);

        }
 

    private:
        std::string get_desc(const std::string& text, const std::string& keyword) {
            auto it = std::search(text.begin(), text.end(), keyword.begin(), keyword.end(), [](char x, char y) {
                return std::tolower(x) == std::tolower(y);
            });
            int pos = std::distance(text.begin(), it);
            int st_pos = std::max(0, pos - 50), end_pos = std::min(pos + 100, (int)text.size());

            return text.substr(st_pos, end_pos - st_pos);
        }
        void init(const std::string& file_path) {
            id = ns_index::index::get_instance();
            id->build_index(file_path);
        }
        ns_index::index* id;
    };
}