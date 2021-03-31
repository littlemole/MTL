#pragma once

#include "Parser.h"



class SelfReg
{
public:
    SelfReg(Parser& parser,
        std::string server,
        std::string arch,
        std::string apartment)
        : parser_(parser),
        server_(server),
        arch_(arch),
        apartment_(apartment)
    {
        serverType_ = "InprocServer32";
        if (server.substr(server.size() - 4, 4) == ".exe")
        {
            serverType_ = "LocalServer32";
        }
        serverName_ = server.substr(0, server.size() - 4);
        serverPS_ = serverName_ + "PS.dll";

        size_t pos = serverName_.find_last_of("\\");
        if (pos != std::string::npos)
        {
            serverName_ = serverName_.substr(pos + 1);
        }
    }

    struct Entry
    {
        std::string key;
        std::string name;
        std::string value;
        int type = REG_SZ;
    };

    void print()
    {
        std::vector<Entry> entries;

        Library& lib = parser_.lib();

        std::string psi = "";

        std::string oleaut = "00020424-0000-0000-C000-000000000046";

        bool hasTypelib = true;

        for (auto& iface : parser_.interfaces())
        {
            if (iface.ole == false && iface.dual == false)
            {
                psi = parser_.interfaces()[0].uuid;
            }
            if (iface.ole == true || iface.dual == true)
            {
                hasTypelib = true;
            }
        }


        if (hasTypelib)
        {
            entries.push_back({
                string_format("TypeLib\\{%s}",lib.uuid.c_str()) ,
                "", lib.name
                });

            entries.push_back({
                string_format("TypeLib\\{%s}\\%s", lib.uuid.c_str() , lib.version.c_str()) ,
                "", lib.desc
                });

            entries.push_back({
                string_format("TypeLib\\{%s}\\%s\\0", lib.uuid.c_str() , lib.version.c_str()) ,
                "", ""
                });

            entries.push_back({
                string_format("TypeLib\\{%s}\\%s\\0\\%s", lib.uuid.c_str() , lib.version.c_str(), arch_.c_str()) ,
                "", "<!-- server -->"
                });

            entries.push_back({
                string_format("TypeLib\\{%s}\\%s\\FLAGS", lib.uuid.c_str() , lib.version.c_str()),
                "", "0"
                });


            std::cout << std::endl;
        }

        for (auto& coClass : parser_.coClasses())
        {
            std::string progid = lib.name + "." + coClass.name + "." + lib.version;
            std::string versionless_progid = lib.name + "." + coClass.name;

            // CLSID entires

            entries.push_back({
                string_format("CLSID\\{%s}", coClass.uuid.c_str()),
                "", coClass.name
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\%s", coClass.uuid.c_str(), serverType_.c_str()),
                "", "<!-- server -->"
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\%s", coClass.uuid.c_str(), serverType_.c_str()),
                "ThreadingModel", apartment_
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\Version", coClass.uuid.c_str()),
                "", lib.version
                });


            if (hasTypelib)
            {
                entries.push_back({
                    string_format("CLSID\\{%s}\\Typelib", coClass.uuid.c_str()),
                    "", string_format("{%s}", lib.uuid.c_str())
                    });
            }

            entries.push_back({
                string_format("CLSID\\{%s}\\ProgID", coClass.uuid.c_str()),
                "", progid
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\VersionIndependentProgID", coClass.uuid.c_str()),
                "", versionless_progid
                });


            // proid entries

            entries.push_back({
                string_format("%s\\CLSID", progid.c_str()),
                "", string_format("{%s}", coClass.uuid.c_str())
                });

            entries.push_back({
                string_format("%s\\CLSID", versionless_progid.c_str()),
                "", string_format("{%s}", coClass.uuid.c_str())
                });

            entries.push_back({
                string_format("%s\\CurVer", versionless_progid.c_str()),
                "",  lib.version
                });
        }



        for (auto& iface : parser_.interfaces())
        {
            entries.push_back({
                string_format("Interface\\{%s}", iface.uuid.c_str()),
                "",  iface.name
                });


            if (!psi.empty())
            {
                entries.push_back({
                    string_format("Interface\\{%s}\\ProxyStubClsid32", iface.uuid.c_str()),
                    "",  string_format("{%s}", psi.c_str())
                    });
            }
            else
            {
                entries.push_back({
                    string_format("Interface\\{%s}\\ProxyStubClsid32", iface.uuid.c_str()),
                    "",  string_format("{%s}", oleaut.c_str())
                    });
            }

            std::cout << std::endl;
        }


        if (!psi.empty())
        {

            entries.push_back({
                string_format("CLSID\\{%s}", psi.c_str()),
                "",  string_format("%sPS", serverName_.c_str())
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\InprocServer32", psi.c_str()),
                "",  serverPS_
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\InprocServer32", psi.c_str()),
                "ThreadingModel",  "Both"
                });

            entries.push_back({
                string_format("CLSID\\{%s}\\Version", psi.c_str()),
                "",  lib.version
                });
        }


        std::cout << "#include <windows.h>" << std::endl;
        std::cout << "#include <string>" << std::endl;
        std::cout << "#include <vector>" << std::endl;
        std::cout << "#include \"MTL/module.h\"" << std::endl;
        std::cout << std::endl;

        std::cout << "struct Entry " << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "    std::string key;" << std::endl;
        std::cout << "    std::string name;" << std::endl;
        std::cout << "    std::string value;" << std::endl;
        std::cout << "};" << std::endl << std::endl;
        std::cout << "static std::vector<Entry> entries {" << std::endl;

        for (int i = 0; i < entries.size(); i++)
        {
            std::cout << "{\"" << escape(entries[i].key) << "\", "
                << "\"" << escape(entries[i].name) << "\", "
                << "\"" << escape(entries[i].value) << "\"}"
                << std::endl;

            if (i != entries.size() - 1)
            {
                std::cout << ",";
            }
            std::cout << std::endl;
        }

        std::cout << "};" << std::endl;

        std::cout << "HRESULT register_module()" << std::endl;
        std::cout << "{" << std::endl;

        std::cout << "    HKEY hkClasses = NULL; " << std::endl;
        std::cout << "    if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_CURRENT_USER, (LPWSTR)L\"Software\\\\Classes\", 0, KEY_ALL_ACCESS, &hkClasses))" << std::endl;
        std::cout << "    {" << std::endl;
        std::cout << "        throw \"Software\\\\Classes\";" << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << std::endl;

        std::cout << "    for (auto& entry : entries)" << std::endl;
        std::cout << "    {" << std::endl;
        std::cout << "        HKEY hkSub = NULL;" << std::endl;
        std::cout << "        if (ERROR_SUCCESS != RegCreateKeyExA(hkClasses, (LPSTR)entry.key.c_str(), 0, 0,0, KEY_ALL_ACCESS,0, &hkSub,0))" << std::endl;
        std::cout << "        {" << std::endl;
        std::cout << "            throw entry.key;" << std::endl;
        std::cout << "        }" << std::endl;
        std::cout << std::endl;

        std::cout << "        if (entry.value == \"<!-- server -->\")" << std::endl;
        std::cout << "        {" << std::endl;

        std::cout << "            char self[MAX_PATH];" << std::endl;
        std::cout << "            size_t len = 0;" << std::endl;
        std::cout << "            wcstombs_s( &len, self, MTL::pathToSelf().c_str(),_TRUNCATE);" << std::endl;

        std::cout << "            if (ERROR_SUCCESS != RegSetValueExA(hkSub, (LPSTR)entry.name.c_str(), 0, REG_SZ, (const BYTE*)(self), (DWORD)(len) * sizeof(char)))" << std::endl;
        std::cout << "            {" << std::endl;
        std::cout << "                throw self;" << std::endl;
        std::cout << "            }" << std::endl;
        std::cout << "        }" << std::endl;
        std::cout << "        else {" << std::endl;
        std::cout << "            if (ERROR_SUCCESS != RegSetValueExA(hkSub, (LPSTR)entry.name.c_str(), 0, REG_SZ, (const BYTE*)(entry.value.c_str()), (DWORD)(entry.value.size() + 1) * sizeof(char)))" << std::endl;
        std::cout << "            {" << std::endl;
        std::cout << "                throw entry.value;" << std::endl;
        std::cout << "            }" << std::endl;
        std::cout << "        }" << std::endl;
        std::cout << "        ::RegCloseKey(hkSub);" << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << "    ::RegCloseKey(hkClasses);" << std::endl;;
        std::cout << "    return S_OK;" << std::endl;
        std::cout << "}" << std::endl;

        std::cout << std::endl;

        std::cout << "HRESULT unregister_module()" << std::endl;
        std::cout << "{" << std::endl;

        std::cout << "    HKEY hkClasses = NULL; " << std::endl;
        std::cout << "    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, (LPWSTR)L\"Software\\\\Classes\", 0, KEY_ALL_ACCESS, &hkClasses))" << std::endl;
        std::cout << "    {" << std::endl;
        std::cout << "        throw \"Software\\\\Classes\";" << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << std::endl;

        std::cout << "    for (int i = (int)entries.size()-1; i >=0; i--)" << std::endl;
        std::cout << "    {" << std::endl;
        std::cout << "        ::RegDeleteKeyA( hkClasses, (LPSTR)entries[i].key.c_str() ); " << std::endl;
        std::cout << "    }" << std::endl;
        std::cout << "    ::RegCloseKey(hkClasses);" << std::endl;;
        std::cout << "    return S_OK;" << std::endl;
        std::cout << "}" << std::endl;
    }

private:
    Parser& parser_;
    std::string server_;
    std::string serverPS_;
    std::string serverType_;
    std::string serverName_;
    std::string arch_;
    std::string apartment_;
};
