#pragma once

#include "Parser.h"


class Manifest
{
public:
    Manifest(Parser& parser,
        std::string server,
        std::string arch,
        std::string apartment,
        std::string midl)
        : parser_(parser),
        server_(server),
        arch_(arch),
        apartment_(apartment),
        midl_(midl)
    {
        serverType_ = ".dll";
        if (server.substr(server.size() - 4, 4) == ".exe")
        {
            serverType_ = ".exe";
        }
        serverName_ = server.substr(0, server.size() - 4);
        serverPS_ = serverName_ + "PS.dll";

        size_t pos = serverName_.find_last_of("\\");
        if (pos != std::string::npos)
        {
            serverName_ = serverName_.substr(pos + 1);
        }

        if (arch_ == "x64") arch_ = "amd64";
        if (arch_ == "win64") arch_ = "amd64";
        if (arch_ == "Win32") arch_ = "x86";

    }

    void print()
    {
        std::set<std::string> seen;
        Library& lib = parser_.lib();

        std::string psi = "";

        std::string oleaut = "00020424-0000-0000-C000-000000000046";

        bool hasTypelib = true; // now always true

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

        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;

        std::cout << "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">" << std::endl;
        // processorArchitecture="X64"

        std::string version = lib.version + ".0.0";
        if (serverType_ == ".exe")
        {
            version = midl_;
        }

        std::cout << " <assemblyIdentity type=\"win32\" ";
        std::cout << "   name=\"" << serverName_ << "\" ";
        std::cout << "   processorArchitecture=\"" << arch_ << "\" ";
        std::cout << "   version=\"" << version << "\" /> " << std::endl;

        if (serverType_ == ".dll")
        {

            std::cout << " <file name=\"" << serverName_ << serverType_ << "\" hashalg=\"SHA1\">" << std::endl;

            for (auto& coClass : parser_.coClasses())
            {
                std::string progid = lib.name + "." + coClass.name + "." + lib.version;
                std::string versionless_progid = lib.name + "." + coClass.name;

                std::cout << "  <comClass clsid=\"{" << coClass.uuid << "}\" " << std::endl;
                if (hasTypelib)
                {
                    std::cout << "         tlbid=\"{" << lib.uuid << "}\" " << std::endl;
                }

                std::cout << "         threadingModel=\"" << apartment_ << "\" " << std::endl;
                std::cout << "         progid=\"" << versionless_progid << "\" " << std::endl;
                std::cout << "         description=\"\" >" << std::endl;
                std::cout << "  </comClass>" << std::endl;
            }

            if (hasTypelib)
            {
                std::cout << "  <typelib tlbid=\"{" << lib.uuid << "}\" " << std::endl;
                std::cout << "           version=\"" << lib.version << "\" " << std::endl;
                std::cout << "           helpdir=\"\" " << std::endl;
                std::cout << "           flags=\"HASDISKIMAGE\" >" << std::endl;
                std::cout << "  </typelib>" << std::endl;
            }

            std::cout << " </file>" << std::endl;

            for (auto& coClass : parser_.coClasses())
            {
                // interfaces
                for (auto& iface : coClass.interfaces)
                {
                    if (seen.count(iface.name) != 0)
                    {
                        continue;
                    }

                    seen.insert(iface.name);

                    std::string ps = psi;
                    if (iface.disp || iface.ole || iface.dual)
                    {
                        ps = oleaut;
                    }

                    std::cout << " <comInterfaceExternalProxyStub name=\"" << iface.name << "\" " << std::endl;
                    std::cout << "    iid=\"{" << iface.uuid << "}\" " << std::endl;
                    if (hasTypelib)
                    {
                        std::cout << "    tlbid=\"{" << lib.uuid << "}\" " << std::endl;
                    }
                    std::cout << "    proxyStubClsid32=\"{" << ps << "}\" >" << std::endl;
                    std::cout << " </comInterfaceExternalProxyStub>" << std::endl;
                }
            }
        }
        else
        {
            std::cout << "<!-- repeat this for any dependency you have -->" << std::endl;
            std::cout << "<!--" << std::endl
                << " <dependency>" << std::endl
                << "  <dependentAssembly>" << std::endl
                << "   <assemblyIdentity type=\"win32\" name=\"<[your dependend dll filename without extension]>\" version=\"<[dll version 4 digits]>\" /> "
                << "  </dependentAssembly>" << std::endl
                << " </dependency>" << std::endl
                << "-->" << std::endl;

            std::cout << "<!-- uncomment if you want XP style themes enabled for CommonControls library (if you link with ComCtl32.dll) -->" << std::endl
                << " <dependency>" << std::endl
                << "  <dependentAssembly>" << std::endl
                << "   <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" "
                << "processorArchitecture=\"" << arch_ << "\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\" / > " << std::endl
                << "  </dependentAssembly>" << std::endl
                << " </dependency>" << std::endl
                << "-->" << std::endl;

            std::cout << "<!-- UAccess Mode. patch as desired  -->" << std::endl;
            std::cout << " <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">" << std::endl;
            std::cout << "  <security>" << std::endl;
            std::cout << "   <requestedPrivileges>" << std::endl;
            std::cout << "    <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\" />" << std::endl;
            std::cout << "   </requestedPrivileges>" << std::endl;
            std::cout << "  </security>" << std::endl;
            std::cout << " </trustInfo>" << std::endl;

            std::cout << "" << std::endl;
        }
        std::cout << "</assembly>" << std::endl;
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
    std::string midl_;
};

class IsoManifest
{
public:
    IsoManifest(Parser& parser,
        std::string server,
        std::string arch,
        std::string version,
        const std::vector<std::pair<std::string, std::string>>& dependencies
    )
        : parser_(parser),
        server_(server),
        arch_(arch),
        version_(version),
        dependencies_(dependencies)
    {
        serverType_ = ".dll";
        if (server.substr(server.size() - 4, 4) == ".exe")
        {
            serverType_ = ".exe";
        }
        serverName_ = server.substr(0, server.size() - 4);
        serverPS_ = serverName_ + "PS.dll";

        size_t pos = serverName_.find_last_of("\\");
        if (pos != std::string::npos)
        {
            serverName_ = serverName_.substr(pos + 1);
        }

        if (arch_ == "x64") arch_ = "amd64";
        if (arch_ == "win64") arch_ = "amd64";
        if (arch_ == "Win32") arch_ = "x86";

    }

    void print()
    {
        std::set<std::string> seen;
        Library& lib = parser_.lib();

        std::string psi = "";

        std::string oleaut = "00020424-0000-0000-C000-000000000046";

        bool hasTypelib = true; // now always true

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

        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;

        std::cout << "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">" << std::endl;

        std::cout << " <assemblyIdentity type=\"win32\" ";
        std::cout << "   name=\"" << serverName_ << "\" ";
        std::cout << "   processorArchitecture=\"" << arch_ << "\" ";
        std::cout << "   version=\"" << version_ << "\" /> " << std::endl;

        for (auto& d : dependencies_)
        {
            std::cout << " <dependency>" << std::endl
                << "  <dependentAssembly>" << std::endl
                << "   <assemblyIdentity type=\"win32\" name=\"" << d.first << "\" processorArchitecture=\"" << arch_ << "\" version=\"" << d.second << "\" /> "
                << "  </dependentAssembly>" << std::endl
                << " </dependency>" << std::endl;
        }

        std::cout << "<!-- uncomment if you want XP style themes enabled for CommonControls library (if you link with ComCtl32.dll) -->" << std::endl
            << " <!-- <dependency>" << std::endl
            << "  <dependentAssembly>" << std::endl
            << "   <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" "
            << "processorArchitecture=\"" << arch_ << "\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\" / > " << std::endl
            << "  </dependentAssembly>" << std::endl
            << " </dependency>" << std::endl
            << "-->" << std::endl;

        std::cout << "<!-- UAccess Mode. patch as desired  -->" << std::endl;
        std::cout << " <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">" << std::endl;
        std::cout << "  <security>" << std::endl;
        std::cout << "   <requestedPrivileges>" << std::endl;
        std::cout << "    <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\" />" << std::endl;
        std::cout << "   </requestedPrivileges>" << std::endl;
        std::cout << "  </security>" << std::endl;
        std::cout << " </trustInfo>" << std::endl;

        std::cout << "" << std::endl;

        std::cout << "</assembly>" << std::endl;
        std::cout << "" << std::endl;
    }

private:
    Parser& parser_;
    std::string server_;
    std::string serverPS_;
    std::string serverType_;
    std::string serverName_;
    std::vector<std::pair<std::string, std::string>> dependencies_;
    std::string arch_;
    std::string version_;
};


