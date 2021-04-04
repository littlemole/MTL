#pragma once

#include "util.h"

class Packer
{
public:
    Packer( const std::vector<std::string>& v)
        : deps_(v)
    {
    }

    void print()
    {

        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
        std::cout << "<Project DefaultTargets='Build' ToolsVersion='4.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>" << std::endl;

        std::cout << std::endl;
        std::cout << " <ItemGroup>" << std::endl;
        for (auto& d : deps_)
        {
            std::cout << "  <wixobjs Include=\"" << d << "\" />" << std::endl;
        }
        std::cout << " </ItemGroup>" << std::endl;
        std::cout << std::endl;

        std::cout << " <ItemGroup>" << std::endl;
        for (auto& d : deps_)
        {
            std::cout << "  <resourceFiles Include=\"" << d << "\\deploy\\**\\*.*\" />" << std::endl;
        }
        std::cout << " </ItemGroup>" << std::endl;
        std::cout << std::endl;
        std::cout << "</Project>" << std::endl;
        std::cout << std::endl;
    }

private:
    std::vector<std::string> deps_;
};
