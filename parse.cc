#include "parse.hpp"


int main () {
    //将 路径 + html文件名 保存到vector中，方便对文件进行读取
    std::vector<std::string> files_list;
    if (!enum_file(src_path, &files_list)) {
        std::cerr << "enum file name error" << '\n';
        return 1;
    }

    //读取文件内容，并解析
    std::vector<doc_info> results;
    if (!parse_html(files_list, &results)) { 
        std::cerr << "parse html error" << '\n';
        return 2;
    }

    //解析完成将文件内容写到data/raw_html/raw.txt中，按照'\3'对每个文件内容进行分割
    if (!save_html(results)) {
        std::cerr << "save html error" << '\n';
        return 3;
    }
}
