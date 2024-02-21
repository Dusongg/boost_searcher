#include "../cpp-httplib-0.7.15/httplib.h"
#include <string>

const std::string root_path = "../wwwroot";

int main() {
    httplib::Server svr;
    svr.set_base_dir(root_path.c_str());

    svr.Get("/dusong", [](const httplib::Request &req, httplib::Response &rep) {
        rep.set_content("么么么", "text/plain; charset=utf-8");
    });
    svr.listen("0.0.0.0", 8888);
    return 0;
} 