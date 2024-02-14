#include <iostream>
#include <string>
#include <fstream>

namespace util {
    class file_util {
    public:
        static bool read_file(const std::string file_path, std::string *out) {
            std::ifstream in(file_path, std::ios::in);
            if (!in.is_open()) {
                std::cerr << "open file" << file_path << "error" << '\n';
                return false;
            }
            std::string line;
            while(std::getline(in, line)) {
                *out += line;
            }
            in.close();
            return true;
        }  
    };
}




