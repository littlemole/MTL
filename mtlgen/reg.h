#pragma once

#include "Parser.h"


class Reg
{
public:
    Reg(Parser& parser,
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

    void print()
    {
        std::cout << "Windows Registry Editor Version 5.00" << std::endl << std::endl;

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
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
            std::cout << "; TYPELIB" << std::endl;
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\TypeLib\\{" << lib.uuid << "}]" << std::endl;
            std::cout << "@=\"" << lib.name << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\TypeLib\\{" << lib.uuid << "}\\" << lib.version << "]" << std::endl;
            std::cout << "@=\"" << lib.desc << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\TypeLib\\{" << lib.uuid << "}\\" << lib.version << "\\0\\" << arch_ << "]" << std::endl;
            std::cout << "@=\"" << server_ << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\TypeLib\\{" << lib.uuid << "}\\" << lib.version << "\\FLAGS]" << std::endl;
            std::cout << "@=\"0\"" << std::endl;

            std::cout << std::endl;
        }

        for (auto& coClass : parser_.coClasses())
        {
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
            std::cout << "; COCLASS " << coClass.name << std::endl << std::endl;
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl << std::endl;


            std::string progid = lib.name + "." + coClass.name + "." + lib.version;
            std::string versionless_progid = lib.name + "." + coClass.name;

            std::cout << "; COCLASS " << coClass.name << " CLSID entries" << std::endl << std::endl;

            // CLSID entires
            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}]" << std::endl;
            std::cout << "@=\"" << coClass.name << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}\\" << serverType_ << "]" << std::endl;
            std::cout << "@=\"" << server_ << "\"" << std::endl;
            std::cout << "\"ThreadingModel\"=\"" << apartment_ << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}\\Version]" << std::endl;
            std::cout << "@=\"" << lib.version << "\"" << std::endl;

            if (hasTypelib)
            {
                std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}\\Typelib]" << std::endl;
                std::cout << "@=\"{" << lib.uuid << "}\"" << std::endl;
            }
            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}\\ProgID]" << std::endl;
            std::cout << "@=\"" << progid << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}\\VersionIndependentProgID]" << std::endl;
            std::cout << "@=\"" << versionless_progid << "\"" << std::endl;

            // proid entries

            std::cout << std::endl << "; COCLASS " << coClass.name << " PROGID entries" << std::endl << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\" << progid << "\\CLSID]" << std::endl;
            std::cout << "@=\"{" << coClass.uuid << "}\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\" << versionless_progid << "\\CurVer]" << std::endl;
            std::cout << "@=\"" << lib.version << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\" << versionless_progid << "\\CLSID]" << std::endl;
            std::cout << "@=\"{" << coClass.uuid << "}\"" << std::endl;

            std::cout << std::endl;
        }



        for (auto& iface : parser_.interfaces())
        {

            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
            std::cout << "; INTERFACE " << iface.name << " entries" << std::endl;
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\Interface\\{" << iface.uuid << "}]" << std::endl;
            std::cout << "@=\"" << iface.name << "\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\Interface\\{" << iface.uuid << "}\\ProxyStubClsid32]" << std::endl;
            if (!psi.empty())
            {
                std::cout << "@=\"{" << psi << "}\"" << std::endl;
            }
            else
            {
                std::cout << "@=\"{" << oleaut << "}\"" << std::endl;
            }

            std::cout << std::endl;
        }

        if (!psi.empty())
        {
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
            std::cout << "; PROXYSTUB " << serverName_ << " entries" << std::endl;
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << psi << "}]" << std::endl;
            std::cout << "@=\"" << serverName_ << "PS\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << psi << "}\\InprocServer32]" << std::endl;
            std::cout << "@=\"" << serverPS_ << "\"" << std::endl;
            std::cout << "\"ThreadingModel\"=\"Both\"" << std::endl;

            std::cout << "[HKEY_CLASSES_ROOT\\CLSID\\{" << psi << "}\\Version]" << std::endl;
            std::cout << "@=\"" << lib.version << "\"" << std::endl;
        }

        std::cout << "" << std::endl;
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

class Unreg
{
public:
    Unreg(Parser& parser) : parser_(parser)
    {};

    void print()
    {
        std::cout << "Windows Registry Editor Version 5.00" << std::endl << std::endl;

        Library& lib = parser_.lib();

        std::cout << "[-HKEY_CLASSES_ROOT\\TypeLib\\{" << lib.uuid << "}]" << std::endl;

        for (auto& coClass : parser_.coClasses())
        {
            std::string progid = lib.name + "." + coClass.name + "." + lib.version;
            std::string versionless_progid = lib.name + "." + coClass.name;

            std::cout << "[-HKEY_CLASSES_ROOT\\CLSID\\{" << coClass.uuid << "}]" << std::endl;
            std::cout << "[-HKEY_CLASSES_ROOT\\" << progid << "]" << std::endl;
            std::cout << "[-HKEY_CLASSES_ROOT\\" << versionless_progid << "]" << std::endl;
        }

        for (auto& iface : parser_.interfaces())
        {
            std::cout << "[-HKEY_CLASSES_ROOT\\Interface\\{" << iface.uuid << "}]" << std::endl;
        }

        std::cout << "" << std::endl;
    }

private:
    Parser& parser_;
};
