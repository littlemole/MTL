#pragma once

#include "Parser.h"
#include "MTL/disp/variant.h"

#import <msxml6.dll> rename_namespace(L"MSXML")

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
            if (iface.ole == false && iface.dual == false && iface.disp == false)
            {
                psi = parser_.interfaces()[0].uuid;
            }
            if (iface.ole == true || iface.dual == true || iface.disp)
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

            if (!psi.empty())
            {
                std::cout << " <file name='" << serverPS_ << "'>" << std::endl;
                std::cout << "  <comClass clsid=\"{" << psi << "}\" threadingModel=\"Both\" />" << std::endl;
                std::cout << " </file>" << std::endl;
            }


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

            if (hasTypelib && !lib.uuid.empty())
            {
                std::cout << " <file name='" << serverName_ << ".exe'>" << std::endl;
                std::cout << "  <typelib tlbid=\"{" << lib.uuid << "}\" " << std::endl;
                std::cout << "           version=\"" << lib.version << "\" " << std::endl;
                std::cout << "           helpdir=\"\" " << std::endl;
                std::cout << "           flags=\"HASDISKIMAGE\" >" << std::endl;
                std::cout << "  </typelib>" << std::endl;
                std::cout << " </file>" << std::endl;
            }

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
                    if (ps == oleaut)
                    {
                        std::cout << "    tlbid=\"{" << lib.uuid << "}\" " << std::endl;
                    }
                    std::cout << "    proxyStubClsid32=\"{" << ps << "}\" >" << std::endl;
                    std::cout << " </comInterfaceExternalProxyStub>" << std::endl;
                }
            }

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
        std::string apartment,
        std::string version,
        std::string midl,
        const std::vector<std::string>& dependencies
    )
        : parser_(parser),
        server_(server),
        arch_(arch),
        apartment_(apartment),
        version_(version),
        midl_(midl),
        dependencies_(dependencies)
    {
//        std::cout << "IsoManifest " << server << " " << arch << " " << version << std::endl;
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

        std::string v = parser_.lib().version;
        if (!v.empty())
        {
            version_ = v + ".0.0";
        }
    }

    std::wstring get_attr(MSXML::IXMLDOMNodePtr& node, const wchar_t* n)
    {
        MSXML::IXMLDOMNamedNodeMapPtr atts;
        node->get_attributes(&atts);

        MSXML::IXMLDOMNodePtr att = atts->getNamedItem(*mtl::bstr(mtl::ole_char(n)));

        if (!att)
            return L"";

        mtl::variant v;
        att->get_nodeValue(&v);

        return v.to_wstring();
    }

    HRESULT load_doc(const std::string& manifest, MSXML::IXMLDOMDocument2Ptr& xmlDoc)
    {
        HRESULT hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
        if (hr != S_OK)
        {
            //std::cout << "create parser failed" << std::endl;
            return hr;
        }

        xmlDoc->put_async(VARIANT_FALSE);
        if (xmlDoc->load(mtl::to_wstring(manifest).c_str()) != VARIANT_TRUE)
        {
            //std::cout << "load manifest " << manifest << " failed" << std::endl;
            return E_FAIL;
        }

        xmlDoc->setProperty("SelectionLanguage", "XPath");
        xmlDoc->setProperty("SelectionNamespaces", "xmlns:ms='urn:schemas-microsoft-com:asm.v1'");

        return S_OK;
    }

    void for_each(MSXML::IXMLDOMDocument2Ptr& xmlDoc, const std::string& xpath, std::function<void(MSXML::IXMLDOMNodePtr&)> fun)
    {
        MSXML::IXMLDOMNodeListPtr files = xmlDoc->selectNodes(xpath.c_str());

        long filesCount = 0;
        files->get_length(&filesCount);

        for (long j = 0; j < filesCount; j++)
        {
            MSXML::IXMLDOMNodePtr n;
            files->get_item(j, &n);

            fun(n);
        }
    }

    void parse(const std::string& exe)
    {
        std::string manifest = exe;
        std::string file = exe;
        std::string ext = ".exe";
        std::string psdll = exe;

        size_t pos = exe.find_last_of("\\");
        if (pos != std::string::npos)
        {
            file = file.substr(pos + 1);
        }

        pos = file.find_last_of(".");
        if (pos != std::string::npos)
        {
            file = file.substr(0, pos);
            psdll = file + "PS.dll";
        }

        //std::cout << file << std::endl << manifest << std::endl << ext << std::endl << psdll << std::endl;

        MSXML::IXMLDOMDocument2Ptr xmlDoc; 

        HRESULT hr;
        
        hr = load_doc(manifest,xmlDoc);
        if (hr != S_OK)
        {
            //std::cout << "create parser failed" << std::endl;
            return;
        }
        /*
        hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);
        if (hr != S_OK)
        {
            std::cout << "create parser failed" << std::endl;
            return;
        }

        xmlDoc->put_async(VARIANT_FALSE);
        if (xmlDoc->load(mtl::to_wstring(manifest).c_str()) != VARIANT_TRUE)
        {
            std::cout << "load manifest " << manifest << " failed" << std::endl;
            return;
        }

        xmlDoc->setProperty("SelectionLanguage", "XPath");
        xmlDoc->setProperty("SelectionNamespaces", "xmlns:ms='urn:schemas-microsoft-com:asm.v1'");
        */

        std::vector<std::wstring> psv;

        for_each(xmlDoc, "/ms:assembly/ms:file", [this,file,&ext,&psv](MSXML::IXMLDOMNodePtr& n) 
        {
            std::wstring fn = get_attr(n, L"name");
            if (fn == mtl::to_wstring(file) + L".dll")
            {
                ext = ".dll";
            }
            else
            {
                mtl::bstr b;
                n->get_xml(&b);
                psv.push_back(b.str());
            }
        });
        /*
        MSXML::IXMLDOMNodeListPtr files = xmlDoc->selectNodes("/ms:assembly/ms:file");

        long filesCount = 0;
        files->get_length(&filesCount);

        for (long j = 0; j < filesCount; j++)
        {
            MSXML::IXMLDOMNodePtr n;
            files->get_item(j, &n);

            std::wstring fn = get_attr(n, L"name");
            if (fn == mtl::to_wstring(file) + L".dll")
            {
                ext = ".dll";
            }
        }
        */

        std::string server = file + ext;

        if (ext == ".exe")
        {
            // local server exe
            MSXML::IXMLDOMNodeListPtr proxies = xmlDoc->selectNodes("/ms:assembly/ms:comInterfaceExternalProxyStub");

            long proxyCount = 0;
            proxies->get_length(&proxyCount);

            bool hasPs = false;
            bool wantTypelib = false;


            for (long j = 0; j < proxyCount; j++)
            {
                MSXML::IXMLDOMNodePtr n;
                proxies->get_item(j, &n);

                std::wstring psname = get_attr(n, L"name");
                std::wstring iid = get_attr(n, L"iid");
                std::wstring tlbid = get_attr(n, L"tlbid");
                std::wstring ps = get_attr(n, L"proxyStubClsid32");

                if (ps != L"{00020424-0000-0000-C000-000000000046}")
                {
                    hasPs = true;
                    //continue;
                }
                else
                {
                    wantTypelib = true;
                }


                std::cout << "  <comInterfaceExternalProxyStub iid='" << mtl::to_string(iid) << "' "
                    << "name='" << mtl::to_string(psname) << "'  "
                    << "proxyStubClsid32='" << mtl::to_string(ps) << "' ";
                if (!tlbid.empty())
                {
                    std::cout << "tlbid='" << mtl::to_string(tlbid) << "' ";
                }
// NO ACTUALLY NOT          << "threadingModel='Both' />"
                std::cout << " /> " << std::endl;
            }

            /*
            if (wantTypelib)
            {
                MSXML::IXMLDOMNodeListPtr typelibs = xmlDoc->selectNodes("/ms:assembly/ms:file/ms:typelib");

                long typelibCount = 0;
                typelibs->get_length(&typelibCount);

                for (long k = 0; k < typelibCount; k++)
                {
                    MSXML::IXMLDOMNodePtr tl;
                    typelibs->get_item(k, &tl);

                    std::wstring tlbid = get_attr(tl, L"tlbid");
                    std::wstring helpdir = get_attr(tl, L"helpdir");
                    std::wstring version = get_attr(tl, L"version");

                    std::cout << " <file name='" << server << "'> " << std::endl 
                              << "  <typelib tlbid='" << mtl::to_string(tlbid) << "' "
                              << "version='" << mtl::to_string(version) << "' "
                              << "helpdir='" << mtl::to_string(helpdir) << "' /> " << std::endl
                              << " </file>" << std::endl;
                }
            }
            */
            for (auto& i : psv)
            {
                std::wcout << i << std::endl;
            }
            /*
            for (long j = 0; j < proxyCount; j++)
            {
                MSXML::IXMLDOMNodePtr n;
                proxies->get_item(j, &n);

                std::wstring psname = get_attr(n, L"name");
                std::wstring iid = get_attr(n, L"iid");
                std::wstring ps = get_attr(n, L"proxyStubClsid32");
                std::wstring tlbid = get_attr(n, L"tlbid");

                if (!tlbid.empty())
                {
                    continue;
                }

                std::cout << "  <comInterfaceProxyStub iid='" << mtl::to_string(iid) << "' "
                    << "name='" << mtl::to_string(psname) << "'  "
                    << "proxyStubClsid32='" << mtl::to_string(ps) << "' "
                    << "threadingModel='Both' />"
                    << std::endl;
            }
            if (hasPs)
            {
                std::cout << " </file>" << std::endl;
            }
            */
        }
        else
        {
            // inproc dll
            MSXML::IXMLDOMNodeListPtr proxies = xmlDoc->selectNodes("/ms:assembly/ms:assemblyIdentity");

            long proxyCount = 0;
            proxies->get_length(&proxyCount);

            bool hasPs = false;
            for (long j = 0; j < proxyCount; j++)
            {
                MSXML::IXMLDOMNodePtr n;
                proxies->get_item(j, &n);

                std::wstring depname = get_attr(n, L"name");
                std::wstring arch = get_attr(n, L"processorArchitecture");
                std::wstring version = get_attr(n, L"version");

                std::cout << " <dependency>" << std::endl
                    << "  <dependentAssembly>" << std::endl
                    << "   <assemblyIdentity type=\"win32\" name=\"" << mtl::to_string(depname)
                    << "\" processorArchitecture=\"" << mtl::to_string(arch)
                    << "\" version=\"" << mtl::to_string(version) << "\" /> " << std::endl
                    << "  </dependentAssembly>" << std::endl
                    << " </dependency>" << std::endl;
                std::cout << "" << std::endl;
            }
        }
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
            if (iface.ole == false && iface.dual == false && iface.disp == false)
            {
                psi = parser_.interfaces()[0].uuid;
            }
            if (iface.ole == true || iface.dual == true || iface.disp == true)
            {
                hasTypelib = true;
            }
        }

        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
        std::cout << "<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">" << std::endl;
        std::cout << "" << std::endl;

        std::cout << " <assemblyIdentity type=\"win32\" ";
        std::cout << "name=\"" << serverName_  << "\" ";
        std::cout << "processorArchitecture=\"" << arch_ << "\" ";
        std::cout << "version=\"" << version_ << "\" /> " << std::endl;
        std::cout << "" << std::endl;

        if (serverType_ == ".exe")
        {
            if (hasTypelib && !lib.uuid.empty())
            {
                std::cout << " <file name=\"" << serverName_ << serverType_ << "\"> " << std::endl;
                std::cout << "  <typelib tlbid=\"{" << lib.uuid << "}\" version=\"" << lib.version << "\" helpdir=\"\" />" << std::endl;
                std::cout << " </file>" << std::endl;
                std::cout << "" << std::endl;
            }

            for (auto& iface : parser_.interfaces())
            {
                std::cout << "<comInterfaceExternalProxyStub name=\"" << iface.name << "\" "
                    << "iid=\"{" << iface.uuid << "}\" ";
                if (hasTypelib && !lib.uuid.empty() && iface.typelib)
                {
                    std::cout << "tlbid=\"{" << lib.uuid << "}\" ";
                }
                if (iface.disp || iface.dual || iface.ole)
                {
                    std::cout << "proxyStubClsid32=\"{" << oleaut << "}\" ";
                }
                else
                {
                    std::cout << "proxyStubClsid32=\"{" << psi << "}\" ";
                }
                std::cout << " /> " << std::endl;
            }
        }
        else
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
        std::cout << "" << std::endl;

        if (!psi.empty())
        {
            std::cout << " <file name='" << serverPS_ << "'>" << std::endl;
            std::cout << "  <comClass clsid=\"{" << psi << "}\" threadingModel=\"Both\" />" << std::endl;
            std::cout << " </file>" << std::endl;
        }

        std::cout << "" << std::endl;

        for (auto& d : dependencies_)
        {

            parse(d);
            /*
            std::cout << " <dependency>" << std::endl
                << "  <dependentAssembly>" << std::endl
                << "   <assemblyIdentity type=\"win32\" name=\"" << d.first << "\" processorArchitecture=\"" << arch_ << "\" version=\"" << d.second << "\" /> "
                << "  </dependentAssembly>" << std::endl
                << " </dependency>" << std::endl;
            std::cout << "" << std::endl;
            */
        }

        if (serverType_ == ".exe")
        {
            std::cout << "<!-- uncomment if you want XP style themes enabled for CommonControls library (if you link with ComCtl32.dll) -->" << std::endl
                << " <!-- <dependency>" << std::endl
                << "  <dependentAssembly>" << std::endl
                << "   <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" "
                << "processorArchitecture=\"" << arch_ << "\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\" / > " << std::endl
                << "  </dependentAssembly>" << std::endl
                << " </dependency>" << std::endl
                << "-->" << std::endl;
            std::cout << "" << std::endl;

            std::cout << "<!-- UAccess Mode. patch as desired  -->" << std::endl;
            std::cout << " <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">" << std::endl;
            std::cout << "  <security>" << std::endl;
            std::cout << "   <requestedPrivileges>" << std::endl;
            std::cout << "    <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\" />" << std::endl;
            std::cout << "   </requestedPrivileges>" << std::endl;
            std::cout << "  </security>" << std::endl;
            std::cout << " </trustInfo>" << std::endl;
        }
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
    std::vector<std::string> dependencies_;
    std::string arch_;
    std::string apartment_;
    std::string version_;
    std::string midl_;
};


