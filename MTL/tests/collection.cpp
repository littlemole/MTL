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


template<class T, class I>
class LongCollection : public implements<T(I)>
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
        if (!cnt)
            return E_INVALIDARG;

        *cnt = (long)data_.size();
        return S_OK;
    }

    virtual  HRESULT __stdcall get_Size(long* cnt)
    {
        if (!cnt)
            return E_INVALIDARG;

        *cnt = (long)data_.size();
        return S_OK;
    }

    virtual  HRESULT __stdcall Empty(VARIANT_BOOL* vb)
    {
        if (!vb)
            return E_INVALIDARG;

        *vb = data_.empty() ? VARIANT_TRUE : VARIANT_FALSE;
        return S_OK;
    }

    virtual HRESULT __stdcall get_Value(long index, long* value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        *value = data_[index];
        return S_OK;
    }

    virtual HRESULT __stdcall get_Item(long index, long* value)
    {
        if (!value)
            return E_INVALIDARG;

        if (index >= data_.size())
            return E_INVALIDARG;

        *value = data_[index];
        return S_OK;
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

    virtual HRESULT __stdcall Clear()
    {
        data_.clear();
        return S_OK;
    }

//protected:

    LongCollection() {};

    std::vector<long> data_;
};


struct __declspec(uuid("{3D816763-FB52-4759-9FAF-3DE1F3963F06}"))
ILongCollection : public IUnknown
{
    virtual HRESULT __stdcall get_Count(long* cnt) = 0;
    virtual HRESULT __stdcall get_Value(long index, long* value) = 0;
};

class MyLongCollection : public LongCollection< MyLongCollection, ILongCollection>
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


TEST_F(CollectionTest, testcolelction) {

    using namespace MTL;

    punk<MyLongCollection> collection = MyLongCollection::create();
    collection->Add(1);
    collection->Add(2);
    collection->Add(3);

    punk<ILongCollection> col(collection);

    
    long cnt = 0;
    col->get_Count(&cnt);
    std::cout << cnt << std::endl;

    for (long i = 0; i < cnt; i++)
    {
        long val = 0;
        HR hr = col->get_Value(i, &val);
        std::cout << val << std::endl;
    }
    
}
