#include <iostream>
#include <fstream>
#include <regex>
#pragma once

using namespace std;


class FileManager{
public:
    static bool file_existance(const string& path);
    static bool file_not_created(const string& path);
    static bool is_forbidden_file_name(const string& fileName);
    static bool is_valid_filename(const string& path);
};