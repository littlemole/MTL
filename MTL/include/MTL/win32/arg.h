#pragma once

#include "MTL/sdk.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>

namespace MTL {

    class Options
    {
    public:

        Options(const std::vector<std::wstring>& opts)
            : available_(opts)
        {
            int argc = __argc;
            wchar_t** argv = __wargv;

            path_ = argv[0];

            for (int i = 1; i < argc; i++)
            {
                parse_arg(argv[i]);
            }
        }

        void parse_arg(std::wstring arg)
        {
            bool flag = false;
            while (arg.size())
            {
                if (arg[0] != L'-' && arg[0] != L'/')
                {
                    break;
                }
                flag = true;
                arg = arg.substr(1);
            }

            if (!arg.size())
            {
                return;
            }

            if (flag)
            {
                size_t pos = arg.find_first_of(L"=:");
                if (pos == std::wstring::npos)
                {
                    options_[arg] = L"";
                    return;
                }
                options_[arg.substr(0, pos)] = arg.substr(pos + 1);
                return;
            }

            args_.push_back(arg);
        }

        bool has(const std::wstring& key)
        {
            return options_.count(key) > 0;
        }

        std::wstring value(const std::wstring& key)
        {
            if (!has(key))
            {
                return L"";
            }
            return options_[key];
        }

        std::wstring operator[](const std::wstring& key)
        {
            return value(key);
        }

        std::wstring path()
        {
            return path_;
        }

        std::vector<std::wstring> keys()
        {
            std::vector<std::wstring> result;
            for (auto& option : options_)
            {
                result.push_back(option.first);
            }
            return result;
        }

        std::vector<std::wstring> args()
        {
            return args_;
        }

    private:
        std::wstring path_;
        std::vector<std::wstring> available_;
        std::map<std::wstring, std::wstring> options_;
        std::vector<std::wstring> args_;
    };

}

