#include "searcher.hpp"
#include "./cpp-httplib-0.7.15/httplib.h"

const std::string file_path = "data/raw_html/raw.txt";
const std::string root_html = "./wwwroot";



//todo:
//1.热词统计：优先队列
//2.网站登录注册，mysql
int main() {
    ns_searcher::searcher sc(file_path);
    httplib::Server svr;
    svr.set_base_dir(root_html.c_str());
    svr.Get("/s", [&sc](const httplib::Request &req, httplib::Response &rsp) -> int {  
        if (!req.has_param("word")) {
            rsp.set_content("输入关键字", "text/plain; charset=utf-8");
            return 1;
        }
        //对关键字进行搜索
        std::string word = req.get_param_value("word");
        std::cout << "用户搜索： " << word << '\n';
        std::string json_string;
        sc.search(word, &json_string);
        rsp.set_content(json_string, "application/json");
        return 0;
    });
    svr.listen("0.0.0.0", 8888);
    return 0;
}
