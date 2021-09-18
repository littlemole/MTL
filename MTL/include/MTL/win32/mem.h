#pragma once

#include "mtl/sdk.h"

#include <Objbase.h>

#include <string.h>
#include <iomanip>
#include <sstream>

namespace mtl {


    template<class T>
    class co_mem
    {
    public:
        using type = T;
        const size_t type_size = sizeof(T);

        co_mem()
            : data_(nullptr), size_(0)
        {}

        explicit co_mem(size_t nElements)
            : data_(nullptr), size_(nElements)
        {
            if (size_)
                data_ = (T*)::CoTaskMemAlloc(size_ * type_size);
        }

        co_mem(const co_mem<T>&& rhs)
            : data_(nullptr), size_(rhs.size_)
        {
            if (size_)
            {
                data_ = (T*)::CoTaskMemAlloc(size_ * type_size);
                memcpy(data, rhs.data_, size_ * type_size);
            }
        }

        co_mem(co_mem<T>&& rhs)
            : data_(rhs.data_), size_(rhs.size_)
        {
            rhs.size_ = 0;
            rhs.data_ = 0;
        }

        ~co_mem()
        {
            if (data_)
            {
                ::CoTaskMemFree(data_);
            }
        }

        T& operator[](size_t index)
        {
            return data_[index];
        }

        T* data()
        {
            return data_;
        }

        T* operator*()
        {
            return data_;
        }

        T** operator&()
        {
            return &data_;
        }

        co_mem<T>* address_of()
        {
            return this;
        }

        size_t size() const
        {
            return size_;
        }

    private:

        co_mem<T>& operator=(const co_mem<T>& rhs) = delete;
        co_mem<T>& operator=(co_mem<T>&& rhs) = delete;
        T* data_;
        size_t size_;
    };

    // null terminated wide string allocated with the com memory allocator
    // to pass around OLECHARS

    class co_str
    {
    public:
        co_str()
        {}

        co_str(const co_str& rhs)
        {
            size_t len = rhs.size() + 1;
            str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, rhs.str_, len);
        }

        co_str(co_str&& rhs)
            : str_(rhs.str_)
        {
            rhs.str_ = nullptr;
        }

        explicit co_str(const wchar_t* s)
        {
            size_t len = wcslen(s) + 1;
            str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, s, len);
        }

        explicit co_str(const std::wstring& s)
        {
            size_t len = s.size() + 1;
            str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, s.c_str(), len);
        }

        ~co_str()
        {
            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }
        }

        co_str& operator=(const co_str& rhs)
        {
            if (str_ == rhs.str_)
            {
                return *this;
            }

            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }

            size_t len = rhs.size() + 1;
            str_ = (wchar_t*)::CoTaskMemAlloc(len * sizeof(wchar_t));
            memcpy(str_, rhs.str_, len);

            return *this;
        }

        co_str& operator=(co_str&& rhs)
        {
            if (str_ == rhs.str_)
            {
                return *this;
            }

            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }

            str_ = rhs.str_;
            rhs.str_ = nullptr;

            return *this;
        }

        size_t size() const
        {
            if (!str_) return 0;
            return wcslen(str_);
        }

        std::wstring str()
        {
            if (!str_) return L"";
            return std::wstring(str_, size());
        }

        wchar_t* operator*()
        {
            return str_;
        }

        wchar_t** operator&()
        {
            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = nullptr;
            }
            return &str_;
        }

    private:
        wchar_t* str_ = nullptr;
    };

    /*
    class CoStr
    {
    public:

        CoStr()
        {}

        CoStr(wchar_t* s)
            : str_(s)
        {}

        CoStr(const CoStr& rhs)
        {
            if (rhs.str_)
            {
                size_t len = rhs.size();
                wchar_t* str_ = (wchar_t*) ::CoTaskMemAlloc(len * sizeof(wchar_t));
                memcpy(str_, rhs.str_, len);
            }
        }

        CoStr(CoStr&& rhs)
        {
            if (rhs.str_)
            {
                wchar_t* str_ = rhs.str_;
                rhs.str_ = 0;
            }
        }

        CoStr& operator=(const CoStr& rhs)
        {
            if (this == &rhs)
                return *this;

            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }
            if (rhs.str_)
            {
                size_t len = rhs.size();
                wchar_t* str_ = (wchar_t*) ::CoTaskMemAlloc(len * sizeof(wchar_t));
                memcpy(str_, rhs.str_, len);
            }

            return *this;
        }

        CoStr& operator=(CoStr&& rhs)
        {
            if (this == &rhs)
                return *this;

            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }
            if (rhs.str_)
            {
                str_ = rhs.str_;
                rhs.str_ = 0;
            }

            return *this;
        }

        CoStr& operator=(wchar_t* rhs)
        {

            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }
            if (rhs)
            {
                str_ = rhs;
            }

            return *this;
        }
        size_t size() const
        {
            if (!str_) return 0;
            return wcslen(str_);
        }


        wchar_t* operator*()
        {
            return str_;
        }

        ~CoStr()
        {
            if (str_)
            {
                ::CoTaskMemFree(str_);
                str_ = 0;
            }
        }

        wchar_t* str_ = nullptr;
    };
    */
    class global
    {
    public:

        template<class T>
        class lock
        {
        public:
            lock(HGLOBAL global)
                : global_(global)
            {
                t_ = (T) ::GlobalLock(global);
            }

            ~lock()
            {
                ::GlobalUnlock(global_);
            }

            T get()
            {
                return t_;
            }

            T operator*()
            {
                return t_;
            }

            size_t size()
            {
                return ::GlobalSize(global_);
            }

        private:
            T t_ = nullptr;
            HGLOBAL global_;
        };

        global()
        {}

        global(HGLOBAL glob)
            : global_(glob)
        {}

        global(void* t, size_t s, int flags = GMEM_MOVEABLE | GMEM_NODISCARD)
        {
            alloc(s,flags);
            lock<void*> lock(**this);
            memcpy(lock.get(), t, s);
        }

        global(const std::string& s, int flags = GMEM_MOVEABLE | GMEM_NODISCARD)
        {
            alloc(s.size()+1, flags);
            lock<void*> lock(**this);
            memcpy(lock.get(), s.c_str(), s.size()+1);
        }

        global(const std::wstring& s, int flags = GMEM_MOVEABLE | GMEM_NODISCARD)
        {
            alloc( (s.size()+1)*sizeof(wchar_t), flags);
            lock<void*> lock(**this);
            memcpy(lock.get(), s.c_str(), (s.size()+1)*sizeof(wchar_t));
        }


        ~global()
        {
            dispose();
        }

        void dispose()
        {
            if (global_)
            {
                ::GlobalFree(global_);
                global_ = nullptr;
            }
        }

        HGLOBAL operator*()
        {
            return global_;
        }


        void alloc(size_t s, int flags = GMEM_MOVEABLE | GMEM_NODISCARD)
        {
            dispose();
            global_ = ::GlobalAlloc(flags, s);
        }

        size_t size()
        {
            ::GlobalSize(global_);
        }

        std::string to_string()
        {
            lock<char*> lock(**this);
            return std::string(lock.get());
        }

        std::wstring to_wstring()
        {
            lock<wchar_t*> lock(**this);
            return std::wstring(lock.get());
        }

        void detach()
        {
            global_ = nullptr;
        }

    private:
        HGLOBAL global_ = nullptr;
    };


    class file_info : public BY_HANDLE_FILE_INFORMATION
    {
    public:

        DWORD attributes()
        {
            return dwFileAttributes;
        }

        FILETIME& creation_time()
        {
            return ftCreationTime;
        }

        std::wstring created()
        {
            return sys_time(creation_time());
        }

        FILETIME& last_access_time()
        {
            return ftLastAccessTime;
        }

        std::wstring last_accessed()
        {
            return sys_time(last_access_time());
        }

        FILETIME& last_write_time()
        {
            return ftLastWriteTime;
        }

        std::wstring last_written()
        {
            return sys_time(last_write_time());
        }

        DWORD file_size_high()
        {
            return nFileSizeHigh;
        }

        DWORD file_size_low()
        {
            return nFileSizeLow;
        }

        unsigned long long size()
        {
            ULARGE_INTEGER uli;
            uli.HighPart = nFileSizeHigh;
            uli.LowPart = nFileSizeLow;
            return uli.QuadPart;
        }

        std::wstring file_size()
        {
            LARGE_INTEGER li;
            li.HighPart = nFileSizeHigh;
            li.LowPart = nFileSizeLow;
            return file_size(li);
        }

        static SYSTEMTIME file_time_to_sys_time(FILETIME& FileTime)
        {
            SYSTEMTIME st;
            ::FileTimeToSystemTime(&FileTime, &st);
            return st;
        }

        static std::wstring sys_time(FILETIME& FileTime)
        {
            SYSTEMTIME st;
            if (!::FileTimeToSystemTime(&FileTime, &st))
                return L"";

            std::wostringstream oss;
            oss << st.wYear << L"-" << st.wMonth << L"-" << st.wDay << L" " << st.wHour << L":" << st.wMinute;
            return oss.str();
        }

        static std::wstring file_size(DWORD hi, DWORD lo)
        {
            LARGE_INTEGER li;
            li.HighPart = hi;
            li.LowPart = lo;
            return file_size(li);
        }

        static std::wstring file_size(LARGE_INTEGER li)
        {
            std::wostringstream oss;
            oss << std::setprecision(4);
            if (li.HighPart)
            {
                oss << (li.HighPart * 4) << L"GB";
            }
            else
            {
                double i = li.LowPart;
                double max = 1024 * 1024;
                if (i > max)
                    oss << i / (1024 * 1024) << L" MByte";
                else
                {
                    max = 1024;
                    if (i > max)
                        oss << i / 1024 << L" KByte";
                    else
                        oss << i << L" Byte";
                }
            }
            return oss.str();
        }

    private:
        BY_HANDLE_FILE_INFORMATION bhfi_;
    };



    class memory_mapped_file
    {
    public:
        memory_mapped_file()
            : file_(INVALID_HANDLE_VALUE),
            mapping_(0),
            view_(0)
        {}

        ~memory_mapped_file()
        {
            close();
        }

        operator bool()
        {
            if (file_ == INVALID_HANDLE_VALUE)
                return false;
            if (!mapping_)
                return false;
            if (!view_)
                return false;
            return true;
        }

        bool map(const std::wstring& file, DWORD access = GENERIC_READ | GENERIC_WRITE, DWORD disp = OPEN_EXISTING)
        {
            close();

            file_ = ::CreateFile(file.c_str(), access, 0, NULL, disp, FILE_ATTRIBUTE_NORMAL, NULL);
            if (file_ == INVALID_HANDLE_VALUE)
                return false;

            file_info fi;
            ::GetFileInformationByHandle(file_, &fi);

            fsize_ = fi.size();

            DWORD flag = PAGE_READWRITE;
            if (!(access & GENERIC_WRITE))
                flag = PAGE_READONLY;
            mapping_ = ::CreateFileMapping(file_, NULL, flag, 0, 0, 0);
            if (mapping_ == NULL)
            {
                close();
                return false;
            }

            flag_ = FILE_MAP_WRITE;
            if (!(access & GENERIC_WRITE))
                flag_ = FILE_MAP_READ;

            return remap(0);
        }

        void close()
        {
            if (view_)
            {
                ::UnmapViewOfFile(view_);
                view_ = 0;
            }
            if (mapping_)
            {
                ::CloseHandle(mapping_);
                mapping_ = 0;
            }
            if (file_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(file_);
                file_ = INVALID_HANDLE_VALUE;
            }
        }

        unsigned long long size()
        {
            return fsize_;
        }

        unsigned long long offset()
        {
            return offset_;
        }

        char* operator[](unsigned long long index)
        {
            if (index < offset_)
            {
                remap(index);
            }
            else
            {
                if (index >= offset_ + page_size()) //upperbound_ )
                {
                    remap(index);
                }
            }

            unsigned long long base = index - offset_;

            return ((char*)view_) + base;
        }

        void flush()
        {
            if (view_)
                ::FlushViewOfFile(view_, NULL);
        }

        static unsigned long page_size()
        {
            static unsigned long l = getPageSize();
            return l;
        }

    private:

        static unsigned long getPageSize()
        {
            SYSTEM_INFO systemInfo;
            ::GetSystemInfo(&systemInfo);
            return systemInfo.dwAllocationGranularity;
        }

        bool remap(unsigned long long offset)
        {
            if (!file_)
                return false;
            if (!mapping_)
                return false;

            if (offset > fsize_)
                return false;

            if (view_)
                ::UnmapViewOfFile(view_);

            view_ = 0;

            int s = page_size();

            offset_ = (offset / s) * s;

#ifdef FILEMAP_DEBUG
            char buf[256];
            wsprintf(buf, "FILEMAPPING:\r\nremapped index %lx to offset offset %lx", (unsigned long)offset, (unsigned long)offset_);
            ODBGS(buf);
#endif

            ULARGE_INTEGER uli;
            uli.QuadPart = offset_;
            unsigned long mapsize = s + offset_ > fsize_ ? (unsigned long)(fsize_ - offset_) : s;
            view_ = ::MapViewOfFile(mapping_, flag_, uli.HighPart, uli.LowPart, mapsize);
            if (view_ == NULL)
            {
                close();
                return false;
            }
            return true;
        }

        HANDLE				file_;
        HANDLE				mapping_;
        DWORD				flag_;
        void*               view_;
        unsigned long long	fsize_;
        unsigned long long	offset_;
    };
}
