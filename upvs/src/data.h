#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <sstream>
#include <fstream> 
#include "utils.h"
#include "upvs_data.h"

class Upvs;

class Data {
public:
std::string read_file(const std::string& filename);
void parse_response(const std::string& response, upvs_data &data);
};
extern Data test;

#endif // DATA_H