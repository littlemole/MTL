
#include "mtlgen.h"


/////////////////////////////////////////////
// usage
/////////////////////////////////////////////

int usage()
{
    std::cout << "Usage: mdiparser <show|debug|unreg|disp>|def <midl>" << std::endl;
    std::cout << "       mdiparser <reg|wix|manifest|selfreg> <arch> <apartment> <server> <midl>" << std::endl;
    std::cout << "       mdiparser <isolate> <arch> <version> <server> <dependency1:version> [dep2:ver ...]" << std::endl;
    std::cout << "       mdiparser <package> <projectname> <manufactuer> <subproj1> [subproj2 ...]" << std::endl;
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


void init()
{
    std::string self = pathToSelf();
    std::string mtl_dir;
    size_t pos = self.find("\\bin\\");
    if (pos != std::string::npos)
    {
        mtl_dir = self.substr(0, pos ) + "\\share\\mtl\\msbuild";
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

    std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
    std::cout << "<Project DefaultTargets='Build' ToolsVersion='4.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>" << std::endl;

    std::cout << " <Import Project='" << mtl_dir << "\\mtl.xml'/>" << std::endl;
    std::cout << " </Project>" << std::endl;

    /*
    std::ostringstream oss2;
    oss2 << "xcopy \"" << mtl_dir << "\\msbuild\\root\\mtl.xml" << "\" \".\" /s /e /y";
    cmd = oss2.str();

    system(cmd.c_str());

    std::ostringstream oss3;
    oss3 << "xcopy \"" << mtl_dir << "\\msbuild\\root\\package.props" << "\" \".\" /s /e /y";
    cmd = oss3.str();

    system(cmd.c_str());
    */
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
        init();
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
    std::vector<std::pair<std::string, std::string>> dependencies;
    std::vector<std::string> projects;

    if (cmd == "package")
    {
        if (argc < 5)
        {
            return usage();
        }

        for (size_t i = 4; i < argc; i++)
        {
            projects.push_back(std::string(argv[i]));
        }

        midl = "";
    }

    if (cmd == "isolate")
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
        version = argv[3];
        server = argv[4];


        for (int i = 5; i < argc; i++)
        {
            std::string d = trim(argv[i]);
            size_t pos = d.find(":");
            if (pos == std::string::npos)
            {
                return usage();
            }
            std::string n = d.substr(0, pos);
            std::string v = d.substr(pos + 1);
            dependencies.push_back(std::make_pair(n, v));
        }
    }

    if (cmd == "reg" || cmd == "wix" || cmd == "manifest" || cmd == "selfreg")
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

    // PARSE the midl file, if any
    Parser parser;
    Parser* p = parse(parser, cmd == "debug", midl);

    // generate output
    if (cmd == "package")
    {
        Packer packer(argv[2], argv[3], projects);
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
        Wix wix(p, server, arch, apartment);
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
        IsoManifest man(parser, server, arch, version, dependencies);
        man.print();
    }

    return 0;
}
