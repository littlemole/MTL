#pragma once

#include "Parser.h"

class DispIds
{
public:
    DispIds(Parser& parser)
        : parser_(parser)
    {}

    void print()
    {
        std::string lib = to_upper(parser_.lib().name);

        for (auto& i : parser_.interfaces())
        {
            std::set<std::string> seen_;

            std::string name = lib + "_" + to_upper(i.name) + "_";
            for (auto& p : i.properties)
            {
                if (seen_.count(p.dispid) != 0)
                    continue;

                seen_.insert(p.dispid);
                std::cout << "#define " << name << to_upper(p.name) << " " << p.dispid << std::endl;
            }
            for (auto& m : i.methods)
            {
                if (seen_.count(m.dispid) != 0)
                    continue;

                seen_.insert(m.dispid);
                std::cout << "#define " << name << to_upper(m.name) << " " << m.dispid << std::endl;
            }
        }
        std::cout << std::endl;
    }

private:
    Parser& parser_;
};
