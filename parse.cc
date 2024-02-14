#include "parse.hpp"


int main () {
    //将 路径 + html文件名 保存到vector中，方便对文件进行读取
    std::vector<std::string> files_list;
    if (!enum_file(src_path, &files_list)) {
        std::cerr << "enum file name error" << '\n';
        return 1;
    }

    //读取文件内容，并解析
    std::vector<docInfo_t> results;
    if (!parse_html(files_list, &results)) { 
        std::cerr << "parse html error" << '\n';
        return 2;
    }

    //解析完成将文件内容写道output中，按照'\3'对每个文件内容进行分割
    if (!save_html(results, output)) {
        std::cerr << "save html error" << '\n';
        return 3;
    }
}
