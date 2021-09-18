#pragma once

#include "mtl/sdk.h"
#include "mtl/punk.h"
#include "mtl/win32/uni.h"

#include <pathcch.h>

#include <sstream>
#include <iostream>
 
namespace mtl {

 
class path
{
public:

    path() {};
    path(const std::wstring& p):path_(canonicalize(p)) {}
    ~path() {};

    path(const path& rhs)
        : path_(canonicalize(rhs.path_))
    {}

    path absolute()
    {
        DWORD retval = 0;
        wbuff buf(MAX_PATH);
        retval = ::GetFullPathNameW(path_.c_str(), (DWORD)buf.size(), buf, 0);
        if (retval == 0)
        {
            return std::wstring(L"");
        }
        else
        {
            return path(buf.toString());
        }
    }

    bool is_absolute()
    {
        if (path_.empty())
            return false;

        if (is_unc())
            return true;

        if (is_net_drive())
            return true;

        if (has_root())
            return true;

        return false;
    }

    static std::wstring canonicalize( std::wstring p)
    {
        std::wcout << L">>>>>>>>>>>>>> " << p << std::endl;
        if (p.empty()) return L"";

        if (p == L"." || p == L".\\" )
        {
            wbuff buf(MAX_PATH);
            DWORD len = ::GetCurrentDirectoryW(MAX_PATH, buf);
            return buf.toString(len);
        }
        if (p.size()>2 && p[0] == L'.' && p[1] !='.')
        {
            wbuff buf(MAX_PATH);
            DWORD len = ::GetCurrentDirectoryW(MAX_PATH, buf);
            p = std::wstring(buf, len) + std::wstring( L"\\") + p.substr(1);
        }
        if (p.size() > 2 && p[0] == L'.' && p[1] == '.')
        {
            wbuff buf(MAX_PATH);
            DWORD len = ::GetCurrentDirectoryW(MAX_PATH, buf);
            std::wstring cwd = std::wstring(buf, len);
            p = path(cwd).parent_dir().str() + std::wstring(L"\\") + p.substr(1);
        }

        std::wostringstream oss;
        wchar_t last = ' ';
        for(size_t i = 0; i < p.size(); i++)
        {
            if(p[i] == L'/')
            {
                if (i > 1)
                {
                    if (last != '\\')
                    {
                        oss << L'\\';
                        last = '\\';
                        continue;
                    }
                }
            }

            oss << p[i];
            last = p[i];
        }

        std::wstring tmp = oss.str();

        wbuff buf(tmp.size() * 2);
        HR hr = ::PathCchCanonicalize(buf, buf.size(), tmp.c_str());

        return buf.toString();
    }

    /*
    Path operator=(const Path& rhs) 
    {
        if(this == &rhs)
        {
            return *this;
        }

        path_ = std::wstring(rhs.path_);
        return *this;
    }
    */

    std::wstring str() const
    {
        return path_;
    }

    std::wstring operator*() const
    {
        return str();
    }

	std::wstring ext() const
    {
        std::wstring p = *remove_backslash();
	    size_t pos = p.find_last_of( L".");
	    if ( (pos != std::wstring::npos) && (pos < p.size()-1) )
		    return p.substr(pos);
	    return L"";        
    }

    std::wstring filename() const
    {
        std::wstring p = *remove_backslash();
        size_t pos = p.find_last_of(L"/\\");
        if ( (pos != std::wstring::npos) && (pos < p.size()-2) )
            p = p.substr(pos+1);
        if ( (pos = p.find_first_of(L"?#")) != std::string::npos )
        {
            return p.substr(0,pos);
        }
        return p;        
    }

	const path get_path() const
    {
        std::wstring p = *remove_backslash();
        size_t pos = p.find_last_of(L"/\\");
        if ( (pos != std::wstring::npos) && (pos < p.size()-2) )
            return p.substr(0,pos);
        return p;        
    }

	const path strip_root() const
    {
        size_t pos = path_.find_first_of(L"/\\");
        std::wstring path = path_;
        if ( (pos != std::wstring::npos) && (pos != path_.size()) )
        {
            size_t p   = path_.find_first_not_of(L"/\\",pos);
            if ( (p != std::wstring::npos) && (p != path_.size()) )
                pos = p;
            path = path_.substr(pos);
        }
        return path;	
    }

    const path strip_to_root() const
    {
        std::wstring path = path_;
        size_t pos = path.find(L":\\\\");
        if ( pos == 1 )
        {
            path = path.substr(0,3);
            return path;
        }

        pos = path.find(L":\\");
        if ( pos == 1 )
        {
            path = path.substr(0,3);
            return path;
        }
        pos = path.find(L"\\\\");
        if ( pos == 0 )
        {
            size_t p = path.find(L"\\",3);
            if ( p != std::wstring::npos )
            {
                path = path.substr(0,p);
                return path;
            }
        }
        pos = path.find(L"://");
        if ( pos != std::wstring::npos )
        {
            size_t p = path.find(L"/",pos+3);
            if ( pos != std::wstring::npos )
            {
                path = path.substr(0,p);
                return path;
            }
        }
        if ( path.size() > 1 )
        {
            pos = path.find_first_of(L"/\\",2);
            if ( pos != std::wstring::npos )
            {
                path = path.substr(0,pos);
                return path;
            }
        }
        return path;
    }

    const path add_backslash() const
    {
        std::wstring p(path_);
        if ( p.size() > 0 )
            if ( p[p.size()-1] != L'\\' )
                p += L"\\";
        return p;        
    }
    
    const path remove_backslash() const
    {
        std::wstring p(path_);
        if ( p.size() > 0 )
            if ( p[p.size()-1] == L'\\' )
                p.erase(p.size()-1,1);
        return p;        
    }

    const path add_extension(const std::wstring& extension) const
    {
        std::wstring p(path_);
        if ( ext() == L"" )
            p += extension;
        return p; 
    }

    const path rename_extension(const std::wstring& extension) const
    {
        std::wstring p(path_);
        if ( ext() == L"" )
            p = p.substr(0,p.size()-ext().size());
        else
            p += L".";
        p += extension;
        return p;
    }

    const path append(const std::wstring& ap) const
    {
        std::wstring ret = *add_backslash();
        std::wstring a(ap);
        if ( a[0] == L'\\' )
            a = a.substr(1);

        ret += a;
        return ret; 
    }

    bool exists() const
    {
        if ( path_.empty() )
                return false;

        DWORD attributes = ::GetFileAttributesW(wpath().c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES);
    }

	bool is_dir() const
    {
        if ( path_.empty() )
            return false;

        if ( is_unc() ) 
        {
            std::wstring tmp = path_.substr(2);
            size_t p = tmp.find_first_of( L"/\\" );
            if ( p == std::wstring::npos ) 
            {
                // top level network share
                return true;
            }
        }

        DWORD attributes = ::GetFileAttributesW(wpath().c_str());
        if ( attributes == INVALID_FILE_ATTRIBUTES )
        {
            DWORD e = ::GetLastError();
            if (  e == ERROR_BAD_NETPATH )
                return true;
            return false;
        }
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    bool has_root() const
    {
        std::wstring p = *strip_to_root();
        if ( p == path_ )
            return true;
        return false;
    }
	
    bool is_unc() const
    {
        return path_.substr(0,2) == L"\\\\";
    }

	bool is_net_drive() const 
    {
        return DRIVE_REMOTE == ::GetDriveTypeW(path_.c_str());
    }

	const path parent_dir() const
    {
        std::wstring ret = path_;

        if ( path_.size() == 0 )
            return ret;

        if ( ret[ret.size()-1] == L'\\' )
            ret.erase(ret.size()-1);

        size_t pos = ret.find_last_of( '\\' );
        if ( pos != std::wstring::npos )
        {
            ret = ret.substr(0,pos);
        }
        else ret = L"";

        return ret;
    }

	std::wstring service_name() const
    {
        if ( is_unc() )
        {
            return path_.substr(0,path_.find( L'/',3));
        }

        std::wstring remotename;
        path p = strip_to_root().remove_backslash();

        if ( p.is_net_drive() )
        {	
            DWORD len = 0;
            ::WNetGetConnectionW(p.str().c_str(), 0, &len);
            if ( len )
            {
                wbuff buf(len);
                if (::WNetGetConnectionW(p.str().c_str(), buf, &len) )
                {
                    remotename = buf.toString(len);
                }
            }
        }
        return remotename;        
    }
	
    std::wstring wpath() const
    {
        std::wstring ws;

        if ( is_unc() )
            ws = L"\\\\?\\UNC\\" + path_.substr(2);
        else
            ws = L"\\\\?\\" + path_;

        return ws;        
    }

private:
    const std::wstring path_;
};

}