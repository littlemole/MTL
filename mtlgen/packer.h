#pragma once

#include "util.h"

class Packer
{
public:
    Packer(const std::string& project, const std::string& m, const std::vector<std::string>& v)
        : deps_(v), project_(project), manufacturer_(m)
    {
    }

    void print()
    {

        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
        std::cout << "<Project DefaultTargets='Build' ToolsVersion='4.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>" << std::endl;

        std::cout << " <Import Project='..\\mtl.props'/>" << std::endl;
        std::cout << std::endl;
        std::cout << " <PropertyGroup Label='Configuration'>" << std::endl;
        std::cout << std::endl;
        std::cout << "  <projectName>" << project_ << "</projectName>" << std::endl;
        std::cout << "  <manufacturer>" << manufacturer_ << "</manufacturer>" << std::endl;
        std::string uGuid = new_uuid();
        std::cout << "  <upgradeUuid>" << uGuid << "</upgradeUuid>" << std::endl;
        std::string vGuid = new_uuid();
        std::cout << "  <vendorUuid>" << vGuid << "</vendorUuid>" << std::endl;
        std::cout << std::endl;
        std::cout << "  <Configuration Condition=\"'$(Configuration)'==''\">Debug</Configuration>" << std::endl;
        std::cout << "  <Platform Condition=\"'$(Platform)'==''\">x64</Platform>" << std::endl;
        std::cout << " </PropertyGroup>" << std::endl;
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
        std::cout << "</Project>" << std::endl;
        std::cout << std::endl;
    }

private:
    std::vector<std::string> deps_;
    std::string project_;
    std::string manufacturer_;
};
