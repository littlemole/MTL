#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace mtl
{
    inline std::vector<std::string> split(const std::string& s, char delim, std::vector<std::string>& elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty()) {
                elems.push_back(item);
            }
        }
        return elems;
    }

    inline std::vector<std::string> split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }


    inline std::vector<std::string> split(const std::string& s, std::string delim)
    {
        std::vector<std::string> elems;

        size_t last = 0;
        size_t pos = s.find(delim, last);
        while (pos != std::string::npos)
        {
            if (pos != 0)
            {
                std::string tmp = s.substr(last, pos - last);

                if (!tmp.empty())
                {
                    elems.push_back(tmp);
                }
            }
            last = pos + delim.size();
            pos = s.find(delim, last);
        }

        if (last < s.size())
        {
            elems.push_back(s.substr(last));
        }
        return elems;
    }

    // wstring

    inline std::vector<std::wstring> split(const std::wstring& s, wchar_t delim, std::vector<std::wstring>& elems)
    {
        std::wstringstream ss(s);
        std::wstring item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty()) {
                elems.push_back(item);
            }
        }
        return elems;
    }

    inline std::vector<std::wstring> split(const std::wstring& s, wchar_t delim)
    {
        std::vector<std::wstring> elems;
        split(s, delim, elems);
        return elems;
    }


    inline std::vector<std::wstring> split(const std::wstring& s, std::wstring delim)
    {
        std::vector<std::wstring> elems;

        size_t last = 0;
        size_t pos = s.find(delim, last);
        while (pos != std::string::npos)
        {
            if (pos != 0)
            {
                std::wstring tmp = s.substr(last, pos - last);

                if (!tmp.empty())
                {
                    elems.push_back(tmp);
                }
            }
            last = pos + delim.size();
            pos = s.find(delim, last);
        }

        if (last < s.size())
        {
            elems.push_back(s.substr(last));
        }
        return elems;
    }

    inline std::wstring url_unescape(const std::wstring& str)
    {
        DWORD unescaped = 2047;
        mtl::wbuff buf(2048);
        HRESULT hr = ::UrlUnescape((wchar_t*)str.c_str(), buf, &unescaped, 0);

        return buf.toString();
    }
}