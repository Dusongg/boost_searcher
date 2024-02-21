#include "searcher.hpp"

const std::string file_path = "data/raw_html/raw.txt";

int main() {
    ns_searcher::searcher s(file_path);

    std::string query;
    std::cout << "query: ";
    std::getline(std::cin, query);
    
    std::string out;
    s.search(query, &out);
    std::cout << out << std::endl;
}