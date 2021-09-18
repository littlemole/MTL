#include "gtest/gtest.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/disp/variant.h"
#include <sstream>

class VariantTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    VariantTest() {
        // You can do set-up work for each test here.
    }

    ~VariantTest() override {
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



TEST_F(VariantTest, test_bstr_variants) {

    using namespace mtl;

    variant v(ole_char(L"a variant str"));
    variant v2(v);

    EXPECT_EQ(v.vt, VT_BSTR);
    EXPECT_STREQ(L"a variant str", v.to_wstring().c_str() );

    EXPECT_EQ(v2.vt, VT_BSTR);
    EXPECT_STREQ(L"a variant str", v2.to_wstring().c_str());

    variant v3(std::move(v2));

    EXPECT_EQ(v3.vt, VT_BSTR);
    EXPECT_STREQ(L"a variant str", v3.to_wstring().c_str());

    EXPECT_EQ(v2.vt, VT_EMPTY);
    EXPECT_EQ( 0, v2.ullVal);

}

TEST_F(VariantTest, test_long_variants) {

    using namespace mtl;

    long value = 42;

    variant v(value);
    variant v2(v);

    EXPECT_EQ(v.vt, VT_I4);
    EXPECT_EQ(value, v.value_of<long>());

    EXPECT_EQ(v2.vt, VT_I4);
    EXPECT_EQ(value, v2.value_of<long>());
    
    
    variant v3(std::move(v2));

    EXPECT_EQ(v3.vt, VT_I4);
    EXPECT_EQ(value, v3.value_of<long>());

    EXPECT_EQ(v2.vt, VT_EMPTY);
    EXPECT_EQ(0, v2.ullVal);
    
}

TEST_F(VariantTest, test_long_ref_variants) {

    using namespace mtl;

    long value = 42;

    variant v(&value);

    EXPECT_EQ(v.vt, VT_I4|VT_BYREF);
    EXPECT_EQ(value, v.value_of<long>());

    variant v2(v);

    EXPECT_EQ(v2.vt, VT_I4 | VT_BYREF);
    EXPECT_EQ(value, v2.value_of<long>());

    variant v3(std::move(v2));

    EXPECT_EQ(v3.vt, VT_I4 | VT_BYREF);
    EXPECT_EQ(value, v3.value_of<long>());

    EXPECT_EQ(v2.vt, VT_EMPTY);
    EXPECT_EQ(0, v2.ullVal);
    
    char c = 0;
    HRESULT hr = v3.changeType(VT_I1);

    EXPECT_EQ(S_OK, hr);
    EXPECT_EQ(v3.vt, VT_I1);
    EXPECT_EQ(42, v3.bVal);
    EXPECT_EQ(42, v3.value_of<char>());

}


TEST_F(VariantTest, test_modify_ref_variants) {

    using namespace mtl;

    long value = 42;

    variant v(&value);

    EXPECT_EQ(v.vt, VT_I4 | VT_BYREF);
    EXPECT_EQ(value, v.value_of<long>());

    *(v.plVal) = 43;

    EXPECT_EQ(v.vt, VT_I4 | VT_BYREF);
    EXPECT_EQ(43, v.value_of<long>());
    EXPECT_EQ(43, value);

}

TEST_F(VariantTest, test_change_variant_type) {

    using namespace mtl;

    long value = 42;

    variant v(value);

    EXPECT_EQ(v.vt, VT_I4);
    EXPECT_EQ(value, v.value_of<char>());
    EXPECT_EQ(42.0, v.value_of<float>());
    EXPECT_EQ(42.0, v.value_of<double>());

    EXPECT_STREQ(L"42", v.value_of<std::wstring>().c_str());

}

class TestInterface : public IUnknown
{
public:
    // Inherited via IUnknown
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override
    {
        return E_NOTIMPL;
    }
    virtual ULONG __stdcall AddRef(void) override
    {
        cnt_++;
        return 0;
    }
    virtual ULONG __stdcall Release(void) override
    {
        cnt_--;
        return 0;
    }

    int cnt_ = 0;
};

TEST_F(VariantTest, test_assign_variants) {

    using namespace mtl;

    long value = 42;

    variant v(value);

    EXPECT_EQ(v.vt, VT_I4);
    EXPECT_EQ(value, v.value_of<long>());

    v = ole_char(L"a string");
    EXPECT_EQ(VT_BSTR,v.vt);
    EXPECT_STREQ(L"a string", v.value_of<std::wstring>().c_str());

    v = true;

    EXPECT_EQ(VT_BOOL, v.vt);
    EXPECT_EQ(true, v.value_of<bool>());

    auto i = new TestInterface();

    v = i;

    EXPECT_EQ(1, i->cnt_);
    v = (long) 0;

    EXPECT_EQ(0, i->cnt_);

    v = &i;

    EXPECT_EQ(0, i->cnt_);
    v = (long)0;

    EXPECT_EQ(0, i->cnt_);

}
