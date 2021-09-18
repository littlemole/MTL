
#include "gtest/gtest.h"
#include "MTL/sdk.h"
#include "MTL/win32/module.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/win/app.h"
#include <string>
#include <sstream>

class BstrTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.
 
    BstrTest() {
        // You can do set-up work for each test here.
    }

    ~BstrTest() override {
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



TEST_F(BstrTest, testIt) {

    
    BSTR b = ::SysAllocString(L"call via raw BSTR");
    HRESULT hr = api_taking_BSTR_by_value(b);
    ::SysFreeString(b);
    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"call via raw BSTR\n", tout.str().c_str());
}


TEST_F(BstrTest, construct_bstr_from_olechar) {

    using namespace mtl;

    bstr b(ole_char(L"using a bstr constructed from OLECHAR"));
    HRESULT hr = api_taking_BSTR_by_value(*b);
    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"using a bstr constructed from OLECHAR\n", tout.str().c_str());
}


TEST_F(BstrTest, construct_bstr_from_bstr) {

    using namespace mtl;

    bstr b(ole_char(L"using a bstr copy constructed from other bstr"));

    bstr c(b);
    HRESULT hr = api_taking_BSTR_by_value(*c);
    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"using a bstr copy constructed from other bstr\n", tout.str().c_str());
}


TEST_F(BstrTest, construct_bstr_from_BSTR) {

    using namespace mtl;

    BSTR b = ::SysAllocString(L"using a bstr copy constructed from other BSTR");

    bstr c = bstr(b_copy(b));
    HRESULT hr = api_taking_BSTR_by_value(*c);
    ::SysFreeString(b);

    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"using a bstr copy constructed from other BSTR\n", tout.str().c_str());
}


TEST_F(BstrTest, construct_bstr_from_moved_bstr) {
     
    using namespace mtl;

    bstr b(ole_char(L"using a bstr move constructed from moved bstr"));

    bstr c(std::move(b));
    HRESULT hr = api_taking_BSTR_by_value(*c);
    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"using a bstr move constructed from moved bstr\n", tout.str().c_str());
}

TEST_F(BstrTest, construct_bstr_from_moved_BSTR) {

    using namespace mtl;

    BSTR b = ::SysAllocString(L"using a bstr move constructed from moved BSTR");
    bstr c(std::move(b_move(b)));

    HRESULT hr = api_taking_BSTR_by_value(*c);

    EXPECT_EQ( 0, b);

    //::SysFreeString(b);

    EXPECT_EQ(S_OK, hr);
    EXPECT_STREQ(L"using a bstr move constructed from moved BSTR\n", tout.str().c_str());
}

TEST_F(BstrTest, assign_some_bstrs) {

    using namespace mtl;

    bstr b(ole_char(L"INIT"));

    EXPECT_STREQ(L"INIT", b.str().c_str());

    b = ole_char(L"FIRST");
    EXPECT_STREQ(L"FIRST", b.str().c_str());

    bstr tmp(ole_char(L"SECOND"));
    b = tmp;
    EXPECT_STREQ(L"SECOND", b.str().c_str());

    tmp = ole_char(L"THIRD");
    b = std::move(tmp);
    EXPECT_STREQ(L"THIRD", b.str().c_str());
    EXPECT_EQ(0, tmp.data());

    BSTR x = ::SysAllocString(L"FOURTH");
    b = b_move(x);
    EXPECT_STREQ(L"FOURTH", b.str().c_str());
    EXPECT_EQ(0, x);
}

/*
template<class I>
class strong_ptr;

template<class I>
class weak_ptr
{
    friend class strong_ptr<I>;
public:
    weak_ptr(const std::shared_ptr<MTL::punk<I>>& ptr)
        : weak_(ptr)
    {}

    MTL::punk<I> lock()
    {
        auto sp = weak_.lock();
        if (!sp) return MTL::punk<I>;
        return (*sp);
    }

private:
    std::weak_ptr<MTL::punk<I>> weak_;
};

template<class I>
class strong_ptr
{
public:

    strong_ptr(MTL::punk<I>& p)
        : unknown( new MTL::punk<I>(p) )
    {
    }

    ~strong_ptr()
    {
        //punk<I>& pun = (punk<I>&) * unknown;
        //pun.release();
    }

    weak_ptr weak()
    {
        return weak_ptr(unknown);
    }

    std::shared_ptr<MTL::punk<I>> unknown;

private:
};
*/
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
