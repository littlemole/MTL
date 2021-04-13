#define ISOLATION_AWARE_ENABLED 1
#include "gtest/gtest.h"
#include "MTL/uni.h"
#include "MTL/bstr.h"
#include "MTL/disp.h"
#include "MTL/variant.h"
#include "MTL/sf_array.h"
#include "MTL/punk.h"
#include <string>
#include <sstream>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>

using namespace MTL;

class CollectionTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    CollectionTest() {
        // You can do set-up work for each test here.
    }

    ~CollectionTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).

        reset_tout();
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.

    std::wostringstream tout;

    void reset_tout()
    {
        tout.str(L"");
        tout.clear();
    }

    // consumes a BSTR
    HRESULT api_taking_BSTR_by_value(BSTR b)
    {
        if (!b)
            return S_OK;
        std::wstring s(b, ::SysStringLen(b));
        tout << b << std::endl;
        return S_OK;
    }

    // returns a BSTR in the out param
    HRESULT api_taking_BSTR_by_ref(BSTR* b)
    {
        if (!b)
            return E_INVALIDARG;

        *b = ::SysAllocString(L"api_taking_BSTR_by_ref was called");
        return S_OK;
    }
};

template<class T, class I, class C>
class MtlCollection : public dispatch<T(I)>
{
public:

    MtlCollection()
    {}

    MtlCollection(const GUID& libid, int major = 1, int minor = 0)
        : dispatch<T(I)>(libid,major,minor)
    {}


    static punk<T> create()
    {
       return punk<T>(new T());
    }

    static punk<T> create(const GUID& libid, int major = 1, int minor = 0)
    {
       return punk<T>(new T(libid,major,minor));
    }

    virtual  HRESULT __stdcall get_Count(long* cnt)
    {
        if (!cnt)
            return E_INVALIDARG;

        *cnt = (long)data_.size();
        return S_OK;
    }

    virtual  HRESULT __stdcall get_Length(long* cnt)
    {
        return get_Count(cnt);
    }

    virtual  HRESULT __stdcall get_Size(long* cnt)
    {
        return get_Count(cnt);
    }

    virtual  HRESULT __stdcall Empty(VARIANT_BOOL* vb)
    {
        if (!vb)
            return E_INVALIDARG;

        *vb = data_.empty() ? VARIANT_TRUE : VARIANT_FALSE;
        return S_OK;
    }

    virtual HRESULT __stdcall Clear()
    {
        data_.clear();
        return S_OK;
    }

protected:

    std::vector<C> data_;
};

template<class T, class I>
class LongCollection : public MtlCollection<T,I,long>
{
public:

    LongCollection() {}

    LongCollection(const GUID& libid, int major = 1, int minor = 0)
        : MtlCollection<T,I,long>(libid,major,minor)
    {}

    virtual HRESULT __stdcall Value(long index, long* value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        *value = data_[index];
        return S_OK;
    }

    virtual HRESULT __stdcall Item(long index, long* value)
    {
        return Value(index,value);
    }

    virtual HRESULT __stdcall Put(long index, long value)
    {
        if (index >= data_.size())
            return E_INVALIDARG;

        data_[index] = value;
        return S_OK;
    }

    virtual HRESULT __stdcall Add(long value)
    {
        data_.push_back(value);
        return S_OK;
    }
};

template<class T, class I>
class BstrCollection : public MtlCollection<T,I,bstr>
{
public:

    BstrCollection() {}

    BstrCollection(const GUID& libid, int major = 1, int minor = 0)
        : MtlCollection<T,I,bstr>(libid,major,minor)
    {}

    virtual HRESULT __stdcall Value(long index, BSTR* value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        return data_[index].copy_to(value);
    }

    virtual HRESULT __stdcall Item(long index, BSTR* value)
    {
        return Value(index,value);
    }

    virtual HRESULT __stdcall Put(long index, BSTR value)
    {
        if (index >= data_.size())
            return E_INVALIDARG;

        data_[index] = bstr(b_copy(value));
        return S_OK;
    }

    virtual HRESULT __stdcall Add(BSTR value)
    {
        data_.push_back(bstr(b_copy(value)));
        return S_OK;
    }
};

template<class T, class I>
class VariantCollection : public MtlCollection<T,I,variant>
{
public:

    VariantCollection() {}

    VariantCollection(const GUID& libid, int major = 1, int minor = 0)
        : MtlCollection<T,I,variant>(libid,major,minor)
    {}

    virtual HRESULT __stdcall Value(long index, VARIANT* value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        return data_[index].copy_to(value);
    }

    virtual HRESULT __stdcall Item(long index, VARIANT* value)
    {
        return Value(index,value);
    }

    virtual HRESULT __stdcall Put(long index, VARIANT value)
    {
        if (index >= data_.size())
            return E_INVALIDARG;

        data_[index] = variant(value);
        return S_OK;
    }

    virtual HRESULT __stdcall Add(VARIANT value)
    {
        data_.push_back(variant(value));
        return S_OK;
    }
};

template<class T, class I>
class DispCollection : public MtlCollection<T,I,punk<IDispatch>>
{
public:

    DispCollection() {}

    DispCollection(const GUID& libid, int major = 1, int minor = 0)
        : MtlCollection<T,I,punk<IDispatch>>(libid,major,minor)
    {}

    virtual HRESULT __stdcall Value(long index, IDispatch** value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        return data_[index].queryInterface(value);
    }

    virtual HRESULT __stdcall Item(long index, IDispatch** value)
    {
        return Value(index,value);
    }

    virtual HRESULT __stdcall Put(long index, IDispatch* value)
    {
        if (index >= data_.size())
            return E_INVALIDARG;

        data_[index] = punk<IDispatch>(value);
        return S_OK;
    }

    virtual HRESULT __stdcall Add(IDispatch* value)
    {
        data_.push_back( punk<IDispatch>(value) );
        return S_OK;
    }
};

/*
struct __declspec(uuid("{3D816763-FB52-4759-9FAF-3DE1F3963F06}"))
ILongCollection : public IDispatch
{
    virtual HRESULT __stdcall get_Count(long* cnt) = 0;
    virtual HRESULT __stdcall get_Value(long index, long* value) = 0;
};
*/

#include "IFace.h"

class MyLongCollection : public LongCollection< MyLongCollection, ITestLongCollection>
{
public:

    MyLongCollection()
    {
        std::cout << "MyLongCollection()" << std::endl;
    }

    MyLongCollection(const GUID& libid, int major = 1, int minor = 0)
        : LongCollection< MyLongCollection, ITestLongCollection>(libid,major,minor)
    {
        std::cout << "MyLongCollection()" << std::endl;
    }

    ~MyLongCollection()
    {
        std::cout << "~MyLongCollection()" << std::endl;
    }


};

TEST_F(CollectionTest, testDispTypeLibLoading) {

    try 
    {
        punk<ITypeLib> tl;
        HR hr = ::LoadRegTypeLib(LIBID_ExampleLib,1,0,LOCALE_SYSTEM_DEFAULT,&tl);
        EXPECT_EQ(S_OK,*hr);
    }
    catch (HRESULT hr)
    {
        EXPECT_EQ(S_OK,hr);
        if (hr == DISP_E_MEMBERNOTFOUND)
        {
            std::cout << "        DISP_E_MEMBERNOTFOUND" << std::endl;
        }
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}

TEST_F(CollectionTest, testcollection) {

    using namespace MTL;

    punk<MyLongCollection> collection = MyLongCollection::create();
    collection->Add(1);
    collection->Add(2);
    collection->Add(3);

    punk<ITestLongCollection> col(collection);

    
    long cnt = 0;
    col->get_Count(&cnt);
    std::cout << cnt << std::endl;

    for (long i = 0; i < cnt; i++)
    {
        long val = 0;
        HR hr = col->Item(i, &val);
        std::cout << val << std::endl;
        EXPECT_EQ(i+1,val);
    }
    
}

TEST_F(CollectionTest, testDisp) {

    try {
        using namespace MTL;

        punk<MyLongCollection> collection = MyLongCollection::create();
        collection->Add(1);
        collection->Add(2);
        collection->Add(3);

        punk<IDispatch> col(collection);
        DISPPARAMS disp{ 0,0,0,0 };

        long cnt = 0;
        variant vResult;
        //    HR hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        HR hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        cnt = vResult.value_of<long>();
        std::cout << cnt << std::endl;

        EXPECT_EQ(3, cnt);

        for (long i = 0; i < cnt; i++)
        {
            long val = 0;
            variant v;
            variant index(i);
            DISPPARAMS disp{ 0,0,0,0 };
            disp.cArgs = 1;
            disp.rgvarg = &index;

//            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            val = v.value_of<long>();
            std::cout << val << std::endl;
            EXPECT_EQ(i + 1, val);
        }
    }
    catch (HRESULT hr)
    {
        if (hr == DISP_E_MEMBERNOTFOUND)
        {
            std::cout << "        DISP_E_MEMBERNOTFOUND" << std::endl;
        }
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}

TEST_F(CollectionTest, testDispLookup) {

    try 
    {
        punk<MyLongCollection> collection = MyLongCollection::create();
        collection->Add(1);
        collection->Add(2);
        collection->Add(3);

        punk<IDispatch> col(collection);

        DISPID did_Count;
        DISPID did_Item;
        wchar_t* count = L"Count";
        wchar_t* item = L"Item";
        col->GetIDsOfNames(IID_NULL, &count, 1, LOCALE_SYSTEM_DEFAULT, &did_Count);
        col->GetIDsOfNames(IID_NULL, &item, 1, LOCALE_SYSTEM_DEFAULT, &did_Item);

        EXPECT_EQ(1, did_Count);
        EXPECT_EQ(2, did_Item);

        punk<ITypeInfo> ti;
        HR hr = col->GetTypeInfo(1, LOCALE_SYSTEM_DEFAULT, &ti);

        TYPEATTR* att = 0;
        hr = ti->GetTypeAttr(&att);

        std::cout << att->cVars << " " << att->cFuncs << std::endl;

        EXPECT_EQ(0, att->cVars);

        for (int i = 0; i < att->cVars; i++)
        {
           
            VARDESC* vdesc = 0;
            ti->GetVarDesc(i, &vdesc);

            EXPECT_EQ(did_Count, vdesc->memid);

            EXPECT_EQ(VT_I4,vdesc->elemdescVar.tdesc.vt);

            ti->ReleaseVarDesc(vdesc);
        }

        EXPECT_EQ(9, att->cFuncs);
        for (int i = 8; i < att->cFuncs; i++)
        {
            FUNCDESC* fdesc = 0;
            ti->GetFuncDesc(i, &fdesc);

            if (i == 7)
            {
                EXPECT_EQ(1, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_PROPERTYGET, fdesc->invkind);
            }
            if (i == 8)
            {
                EXPECT_EQ(2, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_FUNC, fdesc->invkind);
            }
            ti->ReleaseFuncDesc(fdesc);
        }

        ti->ReleaseTypeAttr(att);

        DISPPARAMS disp{ 0,0,0,0 };

        long cnt = 0;
        variant vResult;
        //    HR hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        cnt = vResult.value_of<long>();
        std::cout << cnt << std::endl;

        EXPECT_EQ(3, cnt);

        for (long i = 0; i < cnt; i++)
        {
            long val = 0;
            variant v;
            variant index(i);
            DISPPARAMS disp{ 0,0,0,0 };
            disp.cArgs = 1;
            disp.rgvarg = &index;

//            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            val = v.value_of<long>();
            std::cout << val << std::endl;
            EXPECT_EQ(i + 1, val);
        }
    }
    catch (HRESULT hr)
    {
        if (hr == DISP_E_MEMBERNOTFOUND)
        {
            std::cout << "        DISP_E_MEMBERNOTFOUND" << std::endl;
        }
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}

TEST_F(CollectionTest, testDispExplicitTypelib) {

    try {
        using namespace MTL;

        punk<MyLongCollection> collection = MyLongCollection::create(LIBID_ExampleLib, 1, 0);

        collection->Add(1);
        collection->Add(2);
        collection->Add(3);

        punk<IDispatch> col(collection);

        DISPID did_Count;
        DISPID did_Item;
        wchar_t* count = L"Count";
        wchar_t* item = L"Item";
        col->GetIDsOfNames(IID_NULL, &count, 1, LOCALE_SYSTEM_DEFAULT, &did_Count);
        col->GetIDsOfNames(IID_NULL, &item, 1, LOCALE_SYSTEM_DEFAULT, &did_Item);

        EXPECT_EQ(1, did_Count);
        EXPECT_EQ(2, did_Item);

        punk<ITypeInfo> ti;
        HR hr = col->GetTypeInfo(1, LOCALE_SYSTEM_DEFAULT, &ti);

        TYPEATTR* att = 0;
        hr = ti->GetTypeAttr(&att);

        std::cout << att->cVars << " " << att->cFuncs << std::endl;

        EXPECT_EQ(0, att->cVars);

        for (int i = 0; i < att->cVars; i++)
        {

            VARDESC* vdesc = 0;
            ti->GetVarDesc(i, &vdesc);

            EXPECT_EQ(did_Count, vdesc->memid);

            EXPECT_EQ(VT_I4, vdesc->elemdescVar.tdesc.vt);

            ti->ReleaseVarDesc(vdesc);
        }

        EXPECT_EQ(9, att->cFuncs);
        for (int i = 8; i < att->cFuncs; i++)
        {
            FUNCDESC* fdesc = 0;
            ti->GetFuncDesc(i, &fdesc);

            if (i == 7)
            {
                EXPECT_EQ(1, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_PROPERTYGET, fdesc->invkind);
            }
            if (i == 8)
            {
                EXPECT_EQ(2, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_FUNC, fdesc->invkind);
            }
            ti->ReleaseFuncDesc(fdesc);
        }

        ti->ReleaseTypeAttr(att);

        DISPPARAMS disp{ 0,0,0,0 };

        long cnt = 0;
        variant vResult;
        //    HR hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        hr = col->Invoke(1, IID_NULL, 0, DISPATCH_PROPERTYGET, &disp, &vResult, 0, 0);
        cnt = vResult.value_of<long>();
        std::cout << cnt << std::endl;

        EXPECT_EQ(3, cnt);

        for (long i = 0; i < cnt; i++)
        {
            long val = 0;
            variant v;
            variant index(i);
            DISPPARAMS disp{ 0,0,0,0 };
            disp.cArgs = 1;
            disp.rgvarg = &index;

            //            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            HR hr = col->Invoke(2, IID_NULL, 0, DISPATCH_METHOD, &disp, &v, 0, 0);
            val = v.value_of<long>();
            std::cout << val << std::endl;
            EXPECT_EQ(i + 1, val);
        }
    }
    catch (HRESULT hr)
    {
        if (hr == DISP_E_MEMBERNOTFOUND)
        {
            std::cout << "        DISP_E_MEMBERNOTFOUND" << std::endl;
        }
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}


class TypeCache
{
public:
    TypeCache()
    {}

    TypeCache(IDispatch* disp)
    {
        build(disp);
    }

    struct Entry
    {
        int memberId;
        VARTYPE vt;
        int inokeKind;
    };

    void build(IDispatch* disp)
    {
        punk<ITypeInfo> ti;
        HR hr = disp->GetTypeInfo(1, LOCALE_SYSTEM_DEFAULT, &ti);

        TYPEATTR* att = 0;
        hr = ti->GetTypeAttr(&att);

        for (int i = 7; i < att->cFuncs; i++)
        {
            FUNCDESC* fdesc = 0;
            ti->GetFuncDesc(i, &fdesc);

            bstr name;
            ti->GetDocumentation(fdesc->memid, &name, 0, 0, 0);

            typeCache_[name.to_string()] = Entry{
                fdesc->memid,
                fdesc->elemdescFunc.tdesc.vt,
                fdesc->invkind
            };

            ti->ReleaseFuncDesc(fdesc);
        }

        ti->ReleaseTypeAttr(att);
    }

    const Entry& operator[](const std::string& key)
    {
        return typeCache_[key];
    }

private:
    std::map<std::string, Entry> typeCache_;
};

TEST_F(CollectionTest, walkDisp) {

    try
    {
        punk<MyLongCollection> collection = MyLongCollection::create();
        collection->Add(1);
        collection->Add(2);
        collection->Add(3);

        punk<IDispatch> col(collection);

        DISPID did_Count = 1;
        DISPID did_Item = 2;

        punk<ITypeInfo> ti;
        HR hr = col->GetTypeInfo(1, LOCALE_SYSTEM_DEFAULT, &ti);

        TYPEATTR* att = 0;
        hr = ti->GetTypeAttr(&att);

        std::cout << att->cVars << " " << att->cFuncs << std::endl;

        EXPECT_EQ(0, att->cVars);

        for (int i = 0; i < att->cVars; i++)
        {

            VARDESC* vdesc = 0;
            ti->GetVarDesc(i, &vdesc);

            EXPECT_EQ(did_Count, vdesc->memid);

            EXPECT_EQ(VT_I4, vdesc->elemdescVar.tdesc.vt);

            bstr name;
            ti->GetDocumentation(vdesc->memid, &name, 0, 0, 0);

            std::cout << "prop " << name.to_string() 
                << " " << vdesc->elemdescVar.tdesc.vt 
                << std::endl;

            ti->ReleaseVarDesc(vdesc);
        }

        EXPECT_EQ(9, att->cFuncs);
        for (int i = 7; i < att->cFuncs; i++)
        {
            FUNCDESC* fdesc = 0;
            ti->GetFuncDesc(i, &fdesc);

            if (i == 7)
            {
                EXPECT_EQ(1, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_PROPERTYGET, fdesc->invkind);
            }
            if (i == 8)
            {
                EXPECT_EQ(2, fdesc->memid);
                EXPECT_EQ(VT_I4, fdesc->elemdescFunc.tdesc.vt);
                EXPECT_EQ(INVOKE_FUNC, fdesc->invkind);
            }

            bstr name;
            ti->GetDocumentation(fdesc->memid, &name, 0, 0, 0);

            std::cout << "func " << name.to_string() 
                << " " << fdesc->elemdescFunc.tdesc.vt 
                << " " << fdesc->invkind
                << std::endl;


            ti->ReleaseFuncDesc(fdesc);
        }

        ti->ReleaseTypeAttr(att);
    }
    catch (HRESULT hr)
    {
        if (hr == DISP_E_MEMBERNOTFOUND)
        {
            std::cout << "        DISP_E_MEMBERNOTFOUND" << std::endl;
        }
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}

class MsgBox
{
public:

    MsgBox() 
    {}

    ~MsgBox() 
    {}

    using msg_t = std::function<void()>;

    void submit(const msg_t& msg)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(msg);
        condition_.notify_one();
    }

    bool empty() const
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(queue_.empty())
        {
            condition_.wait(lock, [this](){ return !queue_.empty(); });
        }   
        pop();

        return true;
    }

    bool wait(int ms )
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(queue_.empty() && ms)
        {
            bool r = condition_.wait_for(lock, std::chrono::milliseconds(ms), [this](){ return !queue_.empty(); });
            if( !r ) // timeout
            {
                return false;
            }

            pop();
            return true;
        }
        else if (!queue_.empty())
        {
            pop();
            return true;
        }

        return false;
    }

    bool poll(int ms = 20)
    {
        sleep(ms);

        if(empty())
        {
            return false;
        }

        std::unique_lock<std::mutex> lock(mutex_);
        pop();

        return true;
    }

    void sleep(int ms = 20) const
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));        
    }

private:

    void pop()
    {
        if(queue_.empty()) return;
        
        msg_t task = queue_.front();
        queue_.pop_front();

        task();
    }

    mutable std::mutex mutex_;
    mutable std::condition_variable condition_;
    std::deque<msg_t> queue_;
};

TEST_F(CollectionTest, testMsgBox) {

    MsgBox msgBox;
    bool quit = false;

    auto task = [&msgBox,&quit]()
    {
        DWORD tid = ::GetCurrentThreadId();

        for( int i = 0; i < 10; i++)
        {
            msgBox.sleep(30);
            msgBox.submit( [tid]()
            {
                DWORD id = ::GetCurrentThreadId();
                std::cout << "from " << tid << " on " << id << std::endl;
            });
        }

        msgBox.sleep(30);
        msgBox.submit( [tid,&quit]()
        {
            DWORD id = ::GetCurrentThreadId();
            std::cout << "quit from " << tid << " on " << id << std::endl;
            quit = true;
        });
    };

    std::thread t(task);
    while(!quit)
    {
        bool b = msgBox.poll();
        std::cout << "polling: " << b << std::endl;
    }

    t.join();

    quit = false;

    std::thread t2(task);
    while(!quit)
    {
        bool b = msgBox.wait();
        std::cout << "waiting: " << b << std::endl;
    }

    t2.join();

}

class MsgBoxWin32
{
public:

    MsgBoxWin32() 
    {
        event_ = ::CreateEvent(NULL,FALSE,FALSE,NULL);
    }

    ~MsgBoxWin32() 
    {
        ::CloseHandle(event_);
    }

    using task_t = std::function<void()>;

    void submit(const task_t& t)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(t);
        ::SetEvent(event_);
    }

    void operator()(const task_t& t)
    {
        submit(t);
    }

    bool empty() const
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool wait(int ms = INFINITE)
    {
        HANDLE handles = event_;
        while (true)
        {
            DWORD r = ::WaitForMultipleObjectsEx(1,&handles,FALSE,ms,TRUE);
            if (r == WAIT_IO_COMPLETION)
            {
                continue;
            }
            if( r == WAIT_OBJECT_0 )
            {
                return true;
            }
            if(r == WAIT_TIMEOUT || r == WAIT_FAILED)
            {
                break;
            }
        }
        return false;
    }

    int run()
    {
        MSG msg;
        while (true)
        {
            HANDLE handles = event_;
            DWORD r = ::MsgWaitForMultipleObjectsEx(1, &handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE | MWMO_ALERTABLE);
            if (r == WAIT_IO_COMPLETION)
            {
                continue;
            }

            if( r == WAIT_OBJECT_0 )
            {
                pull();
                continue;
            }

            if (!::GetMessage(&msg, 0, 0, 0))
            {
                break;
            }

            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }       

        return (int)msg.wParam;
    }

private:

    bool pull()
    {
        if(empty())
        {
            return false;
        }

        try 
        {
            task_t t = pop();    
            t();
        }   
        catch(...)
        {
            return false;
        }

        return true;
    }

    task_t pop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(queue_.empty()) throw "queue is empty";
        
        task_t result = queue_.front();
        queue_.pop_front();
        return result;
    }

    mutable std::mutex mutex_;
    std::deque<task_t> queue_;
    HANDLE event_;
};

inline void sleep(int ms = 20) 
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));        
}


inline MsgBoxWin32& ui_thread()
{
    static MsgBoxWin32 uithread;
    return uithread;
}

inline void ui_thread( const std::function<void()>& fun)
{
    ui_thread().submit(fun);
}

inline void task( const std::function<void()>& fun)
{
    std::thread t(fun);
    t.detach();
}

TEST_F(CollectionTest, testMsgBoxWin32) 
{
    DWORD mainThreadId = ::GetCurrentThreadId();

    auto worker = [mainThreadId]()
    {
        DWORD tid = ::GetCurrentThreadId();

        for( int i = 0; i < 10; i++)
        {
            sleep(30);
            ui_thread( [tid]()
            {
                DWORD id = ::GetCurrentThreadId();
                std::cout << "from " << tid << " on " << id << std::endl;
            });
        }  

        sleep(30);
        ui_thread( [tid,mainThreadId]()
        {
            DWORD id = ::GetCurrentThreadId();
            std::cout << "quit from " << tid << " on " << id << std::endl;
            //::PostThreadMessage(mainThreadId,WM_QUIT,0,0);
            ::PostQuitMessage(0);
        });
    };

    task(worker);
    task(worker);

    int r = ui_thread().run();
}


TEST_F(CollectionTest, testMsgBoxWin32NoLoop) 
{
    DWORD mainThreadId = ::GetCurrentThreadId();

    auto worker = [mainThreadId]()
    {
        DWORD tid = ::GetCurrentThreadId();

        for( int i = 0; i < 10; i++)
        {
            sleep(30);
            ui_thread( [tid]()
            {
                DWORD id = ::GetCurrentThreadId();
                std::cout << "from " << tid << " on " << id << std::endl;
            });
        }

        sleep(30);
        ui_thread( [tid,mainThreadId]()
        {
            DWORD id = ::GetCurrentThreadId();
            std::cout << "quit from " << tid << " on " << id << std::endl;
            ::PostThreadMessage(mainThreadId,WM_QUIT,0,0);
        });
    };

    task(worker);
    task(worker);

    int r = ui_thread().wait(1000);
}
