#pragma once

#include "Parser.h"


class Wix
{
public:
    Wix(Parser* parser,
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

        if (apartment_ == "Apartment")
        {
            apartment_ = "apartment";
        }

        if (apartment_ == "Both")
        {
            apartment_ = "both";
        }
    }

    void print()
    {
        const Library& lib = parser_ ? parser_->lib() : Library{ serverName_, "", "", "1.0" };

        std::string psi = "";

        std::string oleaut = "00020424-0000-0000-C000-000000000046";

        bool hasTypelib = true; // now always true

        if (parser_)
            for (auto& iface : parser_->interfaces())
            {
                if (iface.ole == false && iface.dual == false)
                {
                    psi = parser_->interfaces()[0].uuid;
                }
                if (iface.ole == true || iface.dual == true)
                {
                    hasTypelib = true;
                }
            }

        std::cout << "<?xml version='1.0' encoding='UTF-8' ?>" << std::endl;
        std::cout << "<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>" << std::endl;
        std::cout << " <Fragment Id='FragmentCom" << serverName_ << "'>" << std::endl;
        std::cout << "  <DirectoryRef Id='COMPATH'>" << std::endl;


        std::string server_guid = new_uuid();
        std::cout << "   <Component Id=\"" << serverName_ << "Server\" Guid=\"{" << server_guid << "}\">" << std::endl;

        std::string server_short = serverName_ + ".exe";
        if (serverType_ == "InprocServer32")
        {
            server_short = serverName_ + ".dll";
        }

        std::cout << "    <File Id=\"" << serverName_
            << "Server\" KeyPath=\"yes\" "
            << "Source=\"$(var.SourceDir)\\"
            << server_short << "\">" << std::endl;

        if (parser_)
        {
            std::cout << "     <TypeLib Id=\"" << lib.uuid << "\" ";
            std::cout << "Language=\"0\" ";
            std::cout << "MajorVersion=\"" << lib.major() << "\" ";
            std::cout << "MinorVersion=\"" << lib.minor() << "\" >" << std::endl;


            for (auto& coClass : parser_->coClasses())
            {
                std::string progid = lib.name + "." + coClass.name + "." + lib.version;
                std::string versionless_progid = lib.name + "." + coClass.name;

                std::cout << "      <Class Id=\"" << coClass.uuid << "\" ";
                std::cout << "Context=\"" << serverType_ << "\" ";

                if (!coClass.desc.empty())
                {
                    std::cout << "Description=\"" << coClass.desc << "\" ";
                }
                std::cout << "Programmable=\"yes\" ";

                std::cout << "ThreadingModel=\"" << apartment_ << "\" ";
                std::cout << "Version=\"" << lib.version << "\">" << std::endl;

                // progids
                std::cout << "       <ProgId Id=\"" << progid << "\">" << std::endl;
                std::cout << "        <ProgId Id=\"" << versionless_progid << "\" />" << std::endl;
                std::cout << "       </ProgId>" << std::endl;

                // interfaces
                for (auto& iface : coClass.interfaces)
                {
                    std::string proxystub = psi;
                    if (iface.disp || iface.ole || iface.dual || psi.empty())
                    {
                        proxystub = oleaut;
                    }

                    std::cout << "       <Interface Id=\""
                        << iface.uuid << "\" "
                        << "Name=\"" << iface.name << "\" "
                        << "ProxyStubClassId32=\"" << proxystub << "\" />"
                        << std::endl;
                }
                std::cout << "      </Class>" << std::endl;

            }

            std::cout << "     </TypeLib>" << std::endl;
        }

        std::cout << "    </File>" << std::endl;
        std::cout << "   </Component>" << std::endl;

        if (!psi.empty())
        {

            std::string ps_guid = new_uuid();
            std::cout << "   <Component Id=\"" << serverName_ << "ServerPS\" Guid=\"{" << ps_guid << "}\">" << std::endl;

            std::string server_short = serverName_ + "PS.dll";

            std::cout << "    <File Id=\"" << serverName_
                << "ServerPS\" KeyPath=\"yes\" "
                << "Source=\"$(var.SourceDir)\\"
                << server_short << "\">" << std::endl;

            std::cout << "     <Class Id=\"" << psi << "\" ";
            std::cout << "Context=\"InprocServer32\" ";
            std::cout << "Description=\"" << serverName_ << "PS\" ";
            std::cout << "ThreadingModel=\"both\" ";
            std::cout << "Version=\"" << lib.version << "\" />" << std::endl;

            std::cout << "    </File>" << std::endl;
            std::cout << "   </Component>" << std::endl;
        }



        std::cout << "  </DirectoryRef>" << std::endl;

        std::cout << "  <ComponentGroup Id=\"Com" << serverName_ << "\">" << std::endl;
        std::cout << "   <ComponentRef Id=\"" << serverName_ << "Server\" />" << std::endl;

        if (!psi.empty())
        {
            std::cout << "   <ComponentRef Id=\"" << serverName_ << "ServerPS\" />" << std::endl;
        }

        std::cout << "  </ComponentGroup>" << std::endl;

        std::cout << " </Fragment>" << std::endl;
        std::cout << "</Wix>" << std::endl;
        std::cout << "" << std::endl;
    }

private:
    Parser* parser_ = nullptr;
    std::string server_;
    std::string serverPS_;
    std::string serverType_;
    std::string serverName_;
    std::string arch_;
    std::string apartment_;
};
