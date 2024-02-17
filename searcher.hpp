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
            for (auto &e : inverte_elems) {
                ns_index::doc_info *doc = id->get_forward_index(e.doc_id);
                if (doc == nullptr) { continue; }
                Json::Value v;
                v["title"] = doc->title;
                v["text"] = doc->text;
                v["url"] = doc->url;
                root.append(v);
            }
            Json::StyledWriter writer;
            *json_string = writer.write(root);

        }


    private:
        void init(const std::string& file_path) {
            id = ns_index::index::get_instance();
            id->build_index(file_path);
        }
        ns_index::index* id;
    };
}