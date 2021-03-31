#pragma once

#include "util.h"

/////////////////////////////////////////////
// Parser and Lexer enums
/////////////////////////////////////////////

enum class TT {
    DIRECTIVE,
    TEXT,
    IDENTIFIER,
    LINEBREAK,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    CURLY_BRACKET_OPEN,
    CURLY_BRACKET_CLOSE,
    SQUARE_BRACKET_OPEN,
    SQUARE_BRACKET_CLOSE,
    SEMICOLON,
    COLON,
    COMMA,
    COMMENT_START,
    COMMENT_END
};

enum class PS {
    PARSING_MIDL,
    PARSING_ANNOTATION,
    PARSING_ATTRIBUTE,
    PARSING_INTERFACE,
    PARSING_TYPELIB,
    PARSING_COCLASS,
    PARSING_METHODS,
    PARSING_METHOD,
    PARSING_PROPERTIES
};

/////////////////////////////////////////////
// Parser Entites to collect info parsed
// from midl
/////////////////////////////////////////////

class LexerToken
{
public:
    std::string value;
    TT type;
};


struct Attribute
{
    std::string key;
    std::string value;
};

struct Method
{
    std::string name;
    std::string resultType;
    std::string dispid;
    std::string desc;
    bool put = false;
    bool get = false;

    std::vector<Attribute> attributes;

    void apply()
    {
        for (auto& a : attributes)
        {
            if (a.key == "id")
            {
                dispid = a.value;
            }
            if (a.key == "helpstring")
            {
                desc = a.value;
            }
            if (a.key == "propput")
            {
                put = true;
            }
            if (a.key == "propget")
            {
                get = true;
            }
        }
    }
};

struct Interface
{
    std::string name;
    std::string uuid;
    std::string desc;
    bool dual = false;
    bool ole = false;
    bool typelib = false;
    bool disp = false;

    std::vector<Attribute> attributes;
    std::vector<Method> methods;
    std::vector<Method> properties;

    void apply(bool parsing_typelib)
    {
        for (auto& a : attributes)
        {
            if (a.key == "uuid")
            {
                uuid = unbracket(a.value);
            }
            if (a.key == "dual")
            {
                dual = true;
            }
            if (a.key == "oleautomation")
            {
                ole = true;
            }
            if (a.key == "helpstring")
            {
                desc = a.value;
            }
            if (parsing_typelib)
            {
                typelib = true;
            }
        }
    }
};

struct Library
{
    std::string name;
    std::string uuid;
    std::string desc;
    std::string version;

    std::string major() const
    {
        size_t pos = version.find(".");
        if (pos == std::string::npos)
        {
            return version;
        }
        return version.substr(0, pos);
    }

    std::string minor() const
    {
        size_t pos = version.find(".");
        if (pos == std::string::npos)
        {
            return "0";
        }
        return version.substr(pos + 1);
    }

    std::vector<Attribute> attributes;

    void apply()
    {
        for (auto& a : attributes)
        {
            if (a.key == "uuid")
            {
                uuid = unbracket(a.value);
            }
            if (a.key == "helpstring")
            {
                desc = a.value;
            }
            if (a.key == "version")
            {
                version = a.value;
            }
        }
    }
};

struct CoClass
{
    std::string name;
    std::string uuid;
    std::string desc;

    std::vector<Attribute> attributes;
    std::vector<Interface> interfaces;

    void apply()
    {
        for (auto& a : attributes)
        {
            if (a.key == "uuid")
            {
                uuid = unbracket(a.value);
            }
            if (a.key == "helpstring")
            {
                desc = a.value;
            }
        }
    }
};


/////////////////////////////////////////////
// rule driven Parser
//
// parses the stream of LexerTokens
// and builds info of
// -interfaces (vtable,dual and dispatch)
// -coclasses
// -typelib
/////////////////////////////////////////////


class Parser
{
public:

    Parser()
    {
        push(PS::PARSING_MIDL);
    }

    struct Rule {
        TT type;
        std::string condition;
        std::function<void(const LexerToken&)> invoke;
    };

    void apply_rules(const std::vector<Rule>& rules, const LexerToken& token)
    {
        for (auto& rule : rules)
        {
            if (token.type == rule.type)
            {
                std::string cond = rule.condition;
                if (cond == "*" || token.value == cond)
                {
                    rule.invoke(token);
                }
            }
        }
    }

    void parse_properties(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::SEMICOLON, "*", [this](const LexerToken& token)
                {
                    currentProperty_.apply();
                    currentInterface_.properties.push_back(currentProperty_);
                    currentProperty_ = Method{};
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (token.value == "methods")
                    {
                        pop();
                    }
                    else if (currentProperty_.resultType.empty())
                    {
                        currentProperty_.attributes = attributes_;
                        currentProperty_.resultType = token.value;
                    }
                    else if (currentProperty_.name.empty())
                    {
                        currentProperty_.name = token.value;
                    }
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    attributes_.clear();
                    push(PS::PARSING_ANNOTATION);
                }
            }
        };

        apply_rules(rules, token);
    }


    void parse_method(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::SEMICOLON, "*", [this](const LexerToken& token)
                {
                    currentMethod_.apply();
                    currentInterface_.methods.push_back(currentMethod_);
                    currentMethod_ = Method{};
                    pop();
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (currentMethod_.name.empty())
                    {
                        currentMethod_.name = token.value;
                    }
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    attributes_.clear();
                    push(PS::PARSING_ANNOTATION);
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_methods(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::CURLY_BRACKET_CLOSE, "*", [this](const LexerToken& token)
                {
                    interfaces_.push_back(currentInterface_);

                    pop();
                    pop(); // jump out 2 levels!
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    attributes_.clear();
                    push(PS::PARSING_ANNOTATION);
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (currentInterface_.disp == true)
                    {
                        if (token.value == "properties")
                        {
                            push(PS::PARSING_PROPERTIES);
                            return;
                        }
                        else if (token.value == "methods")
                        {
                            return;
                        }
                    }
                    currentMethod_ = Method{};
                    currentMethod_.resultType = token.value;
                    currentMethod_.attributes = attributes_;
                    push(PS::PARSING_METHOD);
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_midl(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::IDENTIFIER, "interface", [this](const LexerToken& token)
                {
                    currentInterface_ = Interface{};
                    currentInterface_.attributes = attributes_;

                    push(PS::PARSING_INTERFACE);
                }
            },
            {
                TT::IDENTIFIER, "dispinterface", [this](const LexerToken& token)
                {
                    currentInterface_ = Interface{};
                    currentInterface_.disp = true;
                    currentInterface_.ole = true;
                    currentInterface_.attributes = attributes_;

                    push(PS::PARSING_INTERFACE);
                }
            },
            {
                TT::IDENTIFIER, "library", [this](const LexerToken& token)
                {
                    currentLibrary_ = Library{};
                    currentLibrary_.attributes = attributes_;

                    push(PS::PARSING_TYPELIB);
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    attributes_.clear();
                    currentAttribute_ = Attribute{};

                    push(PS::PARSING_ANNOTATION);
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_typelib(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::CURLY_BRACKET_CLOSE, "*", [this](const LexerToken& token)
                {
                    currentLibrary_.apply();

                    pop();
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    attributes_.clear();
                    currentAttribute_ = Attribute{};

                    push(PS::PARSING_ANNOTATION);
                }
            },
            {
                TT::IDENTIFIER, "interface", [this](const LexerToken& token)
                {
                    currentInterface_ = Interface{};
                    currentInterface_.attributes = attributes_;

                    push(PS::PARSING_INTERFACE);
                }
            },
            {
                TT::IDENTIFIER, "dispinterface", [this](const LexerToken& token)
                {
                    currentInterface_ = Interface{};
                    currentInterface_.disp = true;
                    currentInterface_.ole = true;
                    currentInterface_.attributes = attributes_;

                    push(PS::PARSING_INTERFACE);
                }
            },
            {
                TT::IDENTIFIER, "coclass", [this](const LexerToken& token)
                {
                    currentCoClass_ = CoClass{};
                    currentCoClass_.attributes = attributes_;

                    push(PS::PARSING_COCLASS);
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (currentLibrary_.name.empty())
                    {
                        currentLibrary_.name = token.value;
                    }
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_coclass(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::CURLY_BRACKET_CLOSE, "*", [this](const LexerToken& token)
                {
                    currentCoClass_.apply();

                    coClasses_.push_back(currentCoClass_);

                    pop();
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                }
            },
            {
                TT::SQUARE_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    currentAttribute_ = Attribute{};
                    attributes_.clear();
                    push(PS::PARSING_ANNOTATION);
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (currentCoClass_.name.empty())
                    {
                        currentCoClass_.name = token.value;
                    }
                    else
                    {
                        std::string v = token.value;
                        if (v != "interface" && v != "dispinterface")
                        {
                            bool found = false;
                            for (auto& i : interfaces_)
                            {
                                if (i.name == v)
                                {
                                    i.typelib = true;
                                    currentCoClass_.interfaces.push_back(i);
                                    found = true;
                                }
                            }
                            if (!found)
                            {
                                Interface disp;
                                disp.name = v;
                                disp.disp = true;
                                disp.ole = true;
                                currentCoClass_.interfaces.push_back(disp);
                            }
                        }
                    }
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_interface(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::CURLY_BRACKET_CLOSE, "*", [this](const LexerToken& token)
                {
                    currentInterface_.attributes = attributes_;
                    currentInterface_.apply(parsing_typelib());

                    interfaces_.push_back(currentInterface_);

                    pop();
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    if (currentInterface_.name.empty())
                    {
                        currentInterface_.name = token.value;
                    }
                }
            },
            {
                TT::CURLY_BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    currentInterface_.attributes = attributes_;
                    currentInterface_.apply(parsing_typelib());

                    push(PS::PARSING_METHODS);
                }
            },
        };

        apply_rules(rules, token);
    }

    void parse_annotation(const LexerToken& token)
    {
        std::vector<Rule> rules
        {
            {
                TT::SQUARE_BRACKET_CLOSE, "*", [this](const LexerToken& token)
                {
                    attributes_.push_back(currentAttribute_);

                    pop();
                }
            },
            {
                TT::COMMA, "*", [this](const LexerToken& token)
                {
                    attributes_.push_back(currentAttribute_);
                    currentAttribute_ = Attribute{};
                    currentAttribute_.key = token.value;
                }
            },
            {
                TT::IDENTIFIER, "*", [this](const LexerToken& token)
                {
                    currentAttribute_ = Attribute{};
                    currentAttribute_.key = token.value;
                }
            },
            {
                TT::BRACKET_OPEN, "*", [this](const LexerToken& token)
                {
                    currentAttributeValue_ = "";

                    push(PS::PARSING_ATTRIBUTE);
                }
            }
        };

        apply_rules(rules, token);
    }

    void parse_attribute(const LexerToken& token)
    {
        TT type = token.type;

        if (type == TT::BRACKET_CLOSE)
        {
            pop();
        }

        if (type == TT::IDENTIFIER || type == TT::TEXT)
        {
            currentAttributeValue_ = token.value;
            currentAttribute_.value = currentAttributeValue_;
        }
    }

    void feed(const std::vector<LexerToken>& tokens)
    {
        static std::map<PS, std::function<void(Parser*, const LexerToken&)>> map
        {
            { PS::PARSING_MIDL,          &Parser::parse_midl        },
            { PS::PARSING_TYPELIB,       &Parser::parse_typelib     },
            { PS::PARSING_COCLASS,       &Parser::parse_coclass     },
            { PS::PARSING_INTERFACE,     &Parser::parse_interface   },
            { PS::PARSING_ANNOTATION,    &Parser::parse_annotation  },
            { PS::PARSING_ATTRIBUTE,     &Parser::parse_attribute   },
            { PS::PARSING_METHODS,       &Parser::parse_methods     },
            { PS::PARSING_METHOD,        &Parser::parse_method      },
            { PS::PARSING_PROPERTIES,    &Parser::parse_properties  }
        };

        for (auto it = tokens.begin(); it != tokens.end(); it++)
        {
            map[state_](this, *it);
        }
    }

    void push(PS state)
    {
        state_ = state;
        states_.push_back(state);
    }

    void pop()
    {
        states_.pop_back();
        state_ = states_.back();
    }

    bool parsing_typelib()
    {
        for (auto& s : states_)
        {
            if (s == PS::PARSING_TYPELIB)
                return true;
        }
        return false;
    }

    void print()
    {
        for (auto& i : interfaces_)
        {
            std::cout << std::endl;
            std::cout << "interface: " << i.name << " ";
            if (i.dual)
                std::cout << "tdual ";
            if (i.ole)
                std::cout << "oleautomation ";
            if (i.typelib)
                std::cout << "[typelib] ";

            std::cout << i.uuid << std::endl;
            std::cout << "\t" << i.desc << std::endl;

            for (auto& m : i.properties)
            {
                std::cout << "\t(" << m.dispid << ") " << m.resultType << " " << m.name << std::endl;
            }

            for (auto& m : i.methods)
            {
                std::cout << "\t(" << m.dispid << ") " << m.resultType << " " << m.name << std::endl;
            }
        }
        for (auto& c : coClasses_)
        {
            std::cout << std::endl;
            std::cout << "coclass: " << c.name << " " << c.uuid << std::endl;
            std::cout << "\t" << c.desc << std::endl;
        }
        std::cout << std::endl;
        std::cout << "typelib: " << currentLibrary_.name << " " << currentLibrary_.version << " " << currentLibrary_.uuid << std::endl;
        std::cout << "\t" << currentLibrary_.desc << std::endl;
    }

    Library& lib()
    {
        return currentLibrary_;
    }

    std::vector<Interface>& interfaces()
    {
        return interfaces_;
    }

    std::vector<CoClass>& coClasses()
    {
        return coClasses_;
    }

private:
    PS state_ = PS::PARSING_MIDL;
    std::vector<PS> states_;

    Library currentLibrary_;
    CoClass currentCoClass_;
    Method currentMethod_;
    Method currentProperty_;
    std::vector<CoClass> coClasses_;

    std::string currentAttributeValue_;
    Attribute currentAttribute_;
    std::vector<Attribute> attributes_;
    Interface currentInterface_;
    std::vector<Interface> interfaces_;
};

/////////////////////////////////////////////
// Lexer
//
// takes tokens from Tokenizer and
// identifies them as a type of the TT enum
// special handling to strip C-style /* */
// comments
/////////////////////////////////////////////

class Lexer
{
public:

    void feed(const std::vector<std::string> tokens)
    {
        for (auto it = tokens.begin(); it != tokens.end(); it++)
        {
            char c = (*it)[0];
            if (c == '/')
            {
                // check for start of comment
                if ((*it).size() > 1)
                {
                    if ((*it)[1] == '*')
                    {
                        insideComment_ = true;
                    }
                }
                continue;
            }
            if (insideComment_)
            {
                if (c == '*')
                {
                    // check for end of comment
                    if ((*it).size() > 1)
                    {
                        if ((*it)[1] == '/')
                        {
                            insideComment_ = false;
                        }
                    }
                    continue;
                }
            }

            static std::map<const char, TT> map
            {
                { '#',  TT::DIRECTIVE },
                { '\n', TT::LINEBREAK },
                { '\r', TT::LINEBREAK },
                { ',',  TT::COMMA },
                { '"',  TT::TEXT },
                { ';',  TT::SEMICOLON },
                { ':',  TT::COLON },
                { '(',  TT::BRACKET_OPEN },
                { ')',  TT::BRACKET_CLOSE },
                { '[',  TT::SQUARE_BRACKET_OPEN },
                { ']',  TT::SQUARE_BRACKET_CLOSE },
                { '{',  TT::CURLY_BRACKET_OPEN },
                { '}',  TT::CURLY_BRACKET_CLOSE }
            };

            if (map.count(c) != 0)
            {
                LexerToken token{ *it, map[c] };
                lexerTokens_.push_back(token);
            }
            else
            {
                LexerToken token{ *it,TT::IDENTIFIER };
                if (!token.value.empty())
                {
                    lexerTokens_.push_back(token);
                }
            }
        }
    }

    void print()
    {
        for (auto& token : lexerTokens_)
        {
            switch (token.type)
            {
            case TT::DIRECTIVE:
            {
                std::cout << "DIRECTIVE : " << token.value << std::endl;
                break;
            }
            case TT::TEXT:
            {
                std::cout << "TEXT : " << token.value << std::endl;
                break;
            }
            case TT::LINEBREAK:
            {
                std::cout << "LINEBREAK : " << token.value << std::endl;
                break;
            }
            case TT::BRACKET_OPEN:
            {
                std::cout << "BRACKET_OPEN : " << token.value << std::endl;
                break;
            }
            case TT::BRACKET_CLOSE:
            {
                std::cout << "BRACKET_CLOSE : " << token.value << std::endl;
                break;
            }
            case TT::CURLY_BRACKET_OPEN:
            {
                std::cout << "CURLY_BRACKET_OPEN : " << token.value << std::endl;
                break;
            }
            case TT::CURLY_BRACKET_CLOSE:
            {
                std::cout << "CURLY_BRACKET_CLOSE : " << token.value << std::endl;
                break;
            }
            case TT::SQUARE_BRACKET_OPEN:
            {
                std::cout << "SQUARE_BRACKET_OPEN : " << token.value << std::endl;
                break;
            }
            case TT::SQUARE_BRACKET_CLOSE:
            {
                std::cout << "SQUARE_BRACKET_CLOSE : " << token.value << std::endl;
                break;
            }
            case TT::SEMICOLON:
            {
                std::cout << "SEMICOLON : " << token.value << std::endl;
                break;
            }
            case TT::COLON:
            {
                std::cout << "COLON : " << token.value << std::endl;
                break;
            }
            case TT::COMMA:
            {
                std::cout << "COMMA : " << token.value << std::endl;
                break;
            }
            case TT::IDENTIFIER:
            {
                std::cout << "IDENTIFIER : " << token.value << std::endl;
                break;
            }
            }
        }
    }

    std::vector<LexerToken> tokens()
    {
        return lexerTokens_;
    }

private:
    bool insideComment_ = false;
    std::vector<LexerToken> lexerTokens_;
};

/////////////////////////////////////////////
// Tokenizer
//
// splits input text into tokens
// special handling to split 
// "string literals"
/////////////////////////////////////////////

class Tokenizer
{
public:

    void feed(const std::string& line)
    {
        static std::string delims = "# \t\r\n{[()]};:,\"";

        size_t start = 0;
        size_t pos = line.find_first_of(delims, start);

        while (start < line.size() && pos != std::string::npos)
        {
            if (pos != 0)
            {
                // push identifier
                tokens_.push_back(line.substr(start, pos - start));

                // special handling of string literals
                if (line[pos] == '"')
                {
                    size_t end = line.find_first_of('"', pos + 1);
                    if (end == std::string::npos)
                    {
                        std::cout << "sybtax error unterminated string" << std::endl;
                        exit(1);
                    }

                    // push the string literal
                    tokens_.push_back(line.substr(pos, end - pos + 1));

                    // next iteration
                    start = end + 1;
                    pos = line.find_first_of(delims, start);
                    continue;
                }
            }

            // push token if not whitespace
            std::string t = line.substr(pos, 1);
            if (t != " " && t != "\t")
            {
                tokens_.push_back(line.substr(pos, 1));
            }

            // next iteration
            start = pos + 1;
            pos = line.find_first_of(delims, start);
            continue;
        }

        // push any last identifiers
        if (start < line.size())
        {
            tokens_.push_back(line.substr(start));
        }
    }

    void print()
    {
        for (auto& token : tokens_)
        {
            std::cout << token << std::endl;
        }
    }

    std::vector<std::string> tokens()
    {
        return tokens_;
    }

private:
    std::vector<std::string> tokens_;
};
