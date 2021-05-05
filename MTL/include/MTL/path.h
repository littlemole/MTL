#pragma once

#include <windows.h>
#include <string>
#include "MTL/uni.h"
 
namespace MTL {

 
class Path
{
public:

    Path();
    Path(const std::wstring& p):path_(p) {}
    ~Path();

    Path(const Path& rhs)
        : path_(rhs.path_)
    {}

    const Path operator=(const Path& rhs) const
    {
        if(this == &rhs)
        {
            return Path(path_);
        }

        return Path(rhs.path_);
    }

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
        std::wstring p = *removeBackSlash();
	    size_t pos = p.find_last_of( L".");
	    if ( (pos != std::wstring::npos) && (pos < p.size()-1) )
		    return p.substr(pos);
	    return L"";        
    }

    std::wstring filename() const
    {
        std::wstring p = *removeBackSlash();
        size_t pos = p.find_last_of(L"/\\");
        if ( (pos != std::wstring::npos) && (pos < p.size()-2) )
            p = p.substr(pos+1);
        if ( (pos = p.find_first_of(L"?#")) != std::string::npos )
        {
            return p.substr(0,pos);
        }
        return p;        
    }

	const Path path() const
    {
        std::wstring p = *removeBackSlash();
        size_t pos = p.find_last_of(L"/\\");
        if ( (pos != std::wstring::npos) && (pos < p.size()-2) )
            return p.substr(0,pos);
        return p;        
    }

	const Path stripRoot() const
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

    const Path stripToRoot() const
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

    const Path addBackSlash() const
    {
        std::wstring p(path_);
        if ( p.size() > 0 )
            if ( p[p.size()-1] != _T('\\') )
                p += _T("\\");
        return p;        
    }
    
    const Path removeBackSlash() const
    {
        std::wstring p(path_);
        if ( p.size() > 0 )
            if ( p[p.size()-1] == L'\\' )
                p.erase(p.size()-1,1);
        return p;        
    }

    const Path addExtension(const std::wstring& extension) const
    {
        std::wstring p(path_);
        if ( ext() == L"" )
            p += extension;
        return p; 
    }

    const Path renameExtension(const std::wstring& extension) const
    {
        std::wstring p(path_);
        if ( ext() == L"" )
            p = p.substr(0,p.size()-ext().size());
        else
            p += L".";
        p += extension;
        return p;
    }

    const Path append(const std::wstring& ap) const
    {
        std::wstring ret = *addBackSlash();
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

	bool isDir() const
    {
        if ( path_.empty() )
            return false;

        if ( isUNC() ) 
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

    bool isRoot() const
    {
        std::wstring p = *stripToRoot();
        if ( p == path_ )
            return true;
        return false;
    }
	
    bool isUNC() const
    {
        return path_.substr(0,2) == L"\\\\";
    }

	bool isNetDrive() const 
    {
        return DRIVE_REMOTE == ::GetDriveTypeW(path_.c_str());
    }

	const Path parentDir() const 
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

	std::wstring serviceName() const
    {
        if ( isUNC() )
        {
            return path_.substr(0,path_.find( L'/',3));
        }

        std::wstring remotename;
        Path p = stripToRoot().removeBackSlash();

        if ( p.isNetDrive() )
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

        if ( isUNC() )
            ws = L"\\\\?\\UNC\\" + path_.substr(2);
        else
            ws = L"\\\\?\\" + path_;

        return ws;        
    }

private:
    const std::wstring path_;
};

}