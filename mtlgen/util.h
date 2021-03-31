#pragma once


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <algorithm>

#include <memory>
#include <stdexcept>

/////////////////////////////////////////////
// helper functions
/////////////////////////////////////////////

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
    int size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::string to_upper(const std::string& s);

std::string trim(const std::string s);

std::string unbracket(const std::string& s);

std::string escape(const std::string& str);

std::string new_uuid();

std::string pathToSelf();