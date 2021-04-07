
#include "mtlgen.h"


/////////////////////////////////////////////
// usage
/////////////////////////////////////////////

int usage()
{
    std::cout << "Usage: mdiparser show|debug|unreg|disp>|def <midl>" << std::endl;
    std::cout << "       mdiparser reg|manifest|selfreg <arch> <apartment> <server> <midl>" << std::endl;
    std::cout << "       mdiparser wix <arch> <apartment> <type> <server> <midl>" << std::endl;
    std::cout << "       mdiparser isolate <arch> <apartment> <version> <server> <midl> <dependency1.manifest> [dep2.manifest ...]" << std::endl;
    std::cout << "       mdiparser package <subproj1> [subproj2 ...]" << std::endl;
    std::cout << "       mdiparser init|setup" << std::endl;
    std::cout << std::endl;
    std::cout << "where:" << std::endl;
    std::cout << "       <midl> : midl filename, ie <Project>.idl" << std::endl;
    std::cout << "       <arch> : target architecture x64|x86. Win32 also works for x86" << std::endl;
    std::cout << "       <apartment> : com apartment type Apartment|Both|Free " << std::endl;
    std::cout << "       <type> : testration type, reg|iso << std::endl;" << std::endl;
    std::cout << "       <server> : server file name, like <Project>.dll. for reg command FULL path to file" << std::endl;
    std::cout << "       <version> : 4 digit version like 1.0.0.0 for regsitry-free com assembly identity" << std::endl;
    return 1;
}

Parser* parse(Parser& parser, bool debug, const std::string& midl)
{
    Tokenizer tokenizer;
    Lexer lexer;

    Parser* p = nullptr;

    if (!midl.empty())
    {
        std::fstream fs;
        fs.open(midl);
        while (fs)
        {
            std::string line;
            std::getline(fs, line);
            line = trim(line);

            // strip comments
            size_t pos = line.find("//");
            if (pos != std::string::npos)
            {
                line = line.substr(0, pos);
            }

            if (debug)
                std::cout << line << std::endl;

            if (!line.empty())
            {
                tokenizer.feed(line);
            }
        }
        fs.close();


        if (debug)
            tokenizer.print();

        lexer.feed(tokenizer.tokens());

        if (debug)
            lexer.print();

        parser.feed(lexer.tokens());

        p = &parser;
    }
    return p;
}




/////////////////////////////////////////////
// main
/////////////////////////////////////////////

int main(int argc, char** argv)
{
    // handle cli arguments
    if (argc < 2)
    {
        return usage();
    }

    std::string cmd = argv[1];
    if (cmd == "init")
    {
        std::string apartment = "Apartment";
        if (argc > 2)
        {
            apartment = argv[2];
        }
        init(apartment);
        return 0;
    }

    if (cmd == "setup")
    {
        init_package();
        return 0;
    }


    if (argc < 3)
    {
        return usage();
    }

    std::string midl = argv[2];
    std::string server;
    std::string arch = "win64";
    std::string apartment = "Apartment";
    std::string version;
    std::string package_type;
    std::vector<std::string> dependencies;
    std::vector<std::string> projects;

    if (cmd == "package")
    {
        if (argc < 3)
        {
            return usage();
        }

        for (size_t i = 2; i < argc; i++)
        {
            projects.push_back(std::string(argv[i]));
        }

        midl = "";
    }

    if (cmd == "isolate" )
    {
        if (argc < 7)
        {
            return usage();
        }
        std::string a = argv[2];
        if (a == "win32" || a == "x86")
        {
            arch = "win32";
        }

        apartment = argv[3];
        version = argv[4];
        server = argv[5];
        midl = argv[6];

        for (int i = 7; i < argc; i++)
        {
            std::string d = trim(argv[i]);
            dependencies.push_back(d);
        }
    }

    if (cmd == "reg" || cmd == "manifest" || cmd == "selfreg")
    {
        if (argc < 5)
        {
            return usage();
        }

        std::string a = argv[2];
        if (a == "win32" || a == "x86")
        {
            arch = "win32";
        }

        apartment = argv[3];
        server = argv[4];
        if (argc > 5)
        {
            midl = argv[5];
        }
        else {
            midl = "";
        }
    }

    if (cmd == "wix" )
    {
        if (argc < 6)
        {
            return usage();
        }

        std::string a = argv[2];
        if (a == "win32" || a == "x86")
        {
            arch = "win32";
        }

        apartment = argv[3];
        package_type = argv[4];
        server = argv[5];
        if (argc > 6)
        {
            midl = argv[6];
        }
        else {
            midl = "";
        }
    }

    ::CoInitialize(NULL);

    // PARSE the midl file, if any
    Parser parser;
    Parser* p = parse(parser, cmd == "debug", midl);

    // generate output
    if (cmd == "package")
    {
        Packer packer(projects);
        packer.print();
    }

    if (cmd == "debug" || cmd == "show")
        parser.print();


    if (cmd == "def")
    {
        ModuleDef md(parser, midl);
        md.print();
    }

    if (cmd == "unreg")
    {
        Unreg unreg(parser);
        unreg.print();
    }

    if (cmd == "reg" && p)
    {
        Reg reg(parser, server, arch, apartment);
        reg.print();
    }

    if (cmd == "selfreg" && p)
    {
        SelfReg reg(parser, server, arch, apartment);
        reg.print();
    }

    if (cmd == "wix")
    {
        Wix wix(p, server, arch, apartment, package_type);
        wix.print();
    }

    if (cmd == "disp")
    {
        DispIds disp(parser);
        disp.print();
    }

    if (cmd == "manifest" && p)
    {
        Manifest man(parser, server, arch, apartment, midl);
        man.print();
    }

    if (cmd == "isolate")
    {
        IsoManifest man(parser, server, arch, apartment, version,midl, dependencies);
        man.print();
    }

    ::CoUninitialize();
    return 0;
}
