#pragma once

#include "Parser.h"


class ModuleDef
{
public:
    ModuleDef(Parser& parser, std::string midl)
        : parser_(parser)
    {
        module_ = midl.substr(0, midl.size() - 4);
    }

    void print()
    {
        std::cout << "LIBRARY	\"" << module_ << "\"" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "EXPORTS" << std::endl;
        std::cout << "     DllCanUnloadNow      PRIVATE" << std::endl;
        std::cout << "     DllGetClassObject    PRIVATE" << std::endl;
        std::cout << "     DllRegisterServer    PRIVATE" << std::endl;
        std::cout << "     DllUnregisterServer  PRIVATE" << std::endl;
        std::cout << "     DllMain              PRIVATE" << std::endl;
        std::cout << "" << std::endl;

    }

private:
    Parser& parser_;
    std::string module_;
};

