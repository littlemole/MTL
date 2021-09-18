#pragma once

#include "util.h"

inline void init(const std::string& apartment)
{

    char buf[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, buf);

    std::string cwd(buf);
    std::string projectDir = cwd;
    std::string mtl_xml = projectDir + "\\mtl.xml";

    std::string self = pathToSelf();
    std::string mtl_dir;
    size_t pos = self.find("\\tools\\mtl\\");
    if (pos != std::string::npos)
    {
        mtl_dir = self.substr(0, pos) + "\\share\\mtlgen\\msbuild";
    }
    else
    {
        pos = self.find("\\x64\\");
        if (pos != std::string::npos)
        {
            mtl_dir = self.substr(0, pos) + "\\mtlgen\\msbuild";
        }
    }

    std::ostringstream oss;
    oss << "xcopy \"" << mtl_dir << "\\root\\" << "\" \".\" /s /e /y /q > NUL";
    std::string cmd = oss.str();

        std::cout << cmd << std::endl;

    system(cmd.c_str());

    //    ::Sleep(2000);

    std::ofstream ofs;
    ofs.open(mtl_xml);

    if (ofs)
    {
        ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
        ofs << "<Project DefaultTargets='Build' ToolsVersion='4.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>" << std::endl;

        ofs << " <PropertyGroup Label='Configuration'>" << std::endl;

        ofs << "" << std::endl;
        ofs << "  <!-- COM Apartment type to use for codegen -->" << std::endl;
        ofs << "  <apartment>" << apartment << "</apartment>" << std::endl;
        ofs << "" << std::endl;
        ofs << "  <!-- project name to use, default is derived from projectname in vcxproj. -->" << std::endl;
        ofs << "  <projectName Condition=\"$(projectName)==''\"></projectName>" << std::endl;

        ofs << "" << std::endl;
        ofs << "  <!-- manufacturer string. defaults to projectName. -->" << std::endl;
        ofs << "  <manufacturer Condition=\"$(manufacturer)==''\"></manufacturer>" << std::endl;

        ofs << "  <!-- UUIDs for MSI packaging, if you want to override defaults. -->" << std::endl;
        ofs << "  <upgradeUuid Condition=\"$(upgradeUuid)==''\"></upgradeUuid>" << std::endl;
        ofs << "  <vendorUuid Condition=\"$(vendorUuid)==''\">*</vendorUuid>" << std::endl;

        ofs << std::endl;
        ofs << "  <!-- detected path to project binary output dir root, usually the msbuild solution folder. -->" << std::endl;
        ofs << "  <installPrefix Condition=\"$(installPrefix)==''\">..</installPrefix>" << std::endl;
        ofs << std::endl;

        ofs << "  <!-- detected path to mtlgen.exe -->" << std::endl;
        ofs << "  <mtlgen Condition = \"$(mtlgen)==''\">" << self <<  "</mtlgen>" << std::endl;
        ofs << "" << std::endl;

        ofs << " </PropertyGroup>" << std::endl;
        ofs << std::endl;

        ofs << " <!-- import real mtl.xml - all msbuild targets can be found here. -->" << std::endl;
        ofs << " <Import Project='" << mtl_dir << "\\mtl.xml'/>" << std::endl;

        ofs << std::endl;
        ofs << " </Project>" << std::endl;
    }
    ofs.close();
}

inline void init_package()
{

    char buf[MAX_PATH];
    ::GetCurrentDirectoryA(MAX_PATH, buf);

    std::string cwd(buf);
    std::string projectDir = cwd;
    std::string mtl_xml = projectDir + "\\package.xml";

    std::string projectName = projectDir;
    std::string projectParentName;
    size_t pos = projectDir.find_last_of("\\");

    if (pos != std::string::npos)
    {
        projectName = projectDir.substr(pos + 1);
        projectParentName = projectDir.substr(0, pos);
        pos = projectParentName.find_last_of("\\");
        if (pos != std::string::npos)
        {
            projectParentName = projectParentName.substr(pos + 1);
        }
    }
    std::string manufacturer = projectName;

    pos = projectDir.find_last_of("\\");
    if (pos != std::string::npos)
    {
        projectName = projectDir.substr(pos + 1);
    }


    std::string self = pathToSelf();
    std::string mtl_dir;
    pos = self.find("\\bin\\");
    if (pos != std::string::npos)
    {
        mtl_dir = self.substr(0, pos) + "\\share\\mtl\\msbuild";
    }
    else
    {
        // todo : add vcpkg version !!
    }

    std::ostringstream oss;
    oss << "xcopy \"" << mtl_dir << "\\root\\" << "\" \".\" /s /e /y /q > NUL";
    std::string cmd = oss.str();

    //    std::cout << cmd << std::endl;

    system(cmd.c_str());

    //    ::Sleep(2000);

    std::ofstream ofs;
    ofs.open(mtl_xml);

    if (ofs)
    {
        ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
        ofs << "<Project DefaultTargets='Build' ToolsVersion='4.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>" << std::endl;

        ofs << " <PropertyGroup Label='Configuration'>" << std::endl;
        ofs << "  <projectName Condition=\"$(projectName)==''\">" << projectName << "</projectName>" << std::endl;
        ofs << "  <manufacturer Condition=\"$(manufacturer)==''\">" << projectParentName << "</manufacturer>" << std::endl;
        std::string uGuid = new_uuid();
        ofs << "  <upgradeUuid Condition=\"$(upgradeUuid)==''\">" << uGuid << "</upgradeUuid>" << std::endl;
        std::string vGuid = new_uuid();
        ofs << "  <vendorUuid Condition=\"$(vendorUuid)==''\">" << vGuid << "</vendorUuid>" << std::endl;

        ofs << std::endl;
        ofs << "  <installPrefix Condition=\"$(installPrefix)==''\">..</installPrefix>" << std::endl;
        ofs << std::endl;

        ofs << " </PropertyGroup>" << std::endl;
        ofs << std::endl;

        ofs << " <Import Project='" << mtl_dir << "\\package.xml'/>" << std::endl;

        ofs << std::endl;
        ofs << " </Project>" << std::endl;
    }
    ofs.close();
}
