#include "util.h"
#include "Objbase.h"


std::string to_upper(const std::string& s)
{
    std::string str = s;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string trim(const std::string s)
{
    if (s.empty())
        return s;

    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        start = 0;

    size_t end = s.find_last_not_of(" \t\r\n");
    if (end == std::string::npos)
        end = s.size();

    return s.substr(start, end - start + 1);
}

std::string unbracket(const std::string& s)
{
    if (s.empty())
        return s;

    size_t start = 0;
    if (s[0] == '{')
        start = 1;

    size_t end = s.size() - 1;
    if (end > 0)
    {
        if (s[end] == '}')
            end = end - 1;
    }

    return s.substr(start, end - start + 1);
}

std::string unquote(const std::string& s)
{
    if (s.empty())
        return s;

    size_t start = 0;
    if (s[0] == '\"')
        start = 1;

    size_t end = s.size() - 1;
    if (end > 0)
    {
        if (s[end] == '\"')
            end = end - 1;
    }

    return s.substr(start, end - start + 1);
}


std::string escape(const std::string& str)
{
    std::ostringstream oss;
    for (char c : str)
    {
        if (c == '\\') oss << "\\";
        if (c == '"') oss << "\\";

        oss.write(&c, 1);
    }
    return oss.str();
}


std::string new_uuid()
{
    GUID guid;
    HRESULT hr = ::CoCreateGuid(&guid);
    char guidStr[37];
    sprintf_s(
        guidStr,
        "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    std::string s(guidStr);
    return s;
}

HMODULE getSelfModule()
{
    HMODULE hm = NULL;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&getSelfModule, &hm) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        exit(1);
    }
    return hm;
}

std::string pathToSelf()
{
    HMODULE hm = getSelfModule();

    char path[MAX_PATH];
    if (GetModuleFileNameA(hm, path, sizeof(path)) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        exit(1);
    }

    return std::string(path);
}