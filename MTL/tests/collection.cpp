#include "gtest/gtest.h"
#include "MTL/uni.h"
#include "MTL/bstr.h"
#include "MTL/disp.h"
#include "MTL/variant.h"
#include "MTL/sf_array.h"
#include "MTL/punk.h"
#include <string>
#include <sstream>

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

    static punk<T> create()
    {
       return punk<T>(new T());
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

    MtlCollection() {};

    std::vector<C> data_;
};

template<class T, class I>
class LongCollection : public MtlCollection<T,I,long>
{
public:

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
class VriantCollection : public MtlCollection<T,I,variant>
{
public:

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

    ~MyLongCollection()
    {
        std::cout << "~MyLongCollection()" << std::endl;
    }


};


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

    const Entry& operator[](const std::string& key) const
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