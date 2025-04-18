#include <string>
#include <iostream>
class Utils {
    public:
    std::string extract_tag(std::string& buffer, const std::string& tag);
};
extern Utils util;