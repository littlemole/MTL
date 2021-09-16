#include "gtest/gtest.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/disp/sf_array.h"
#include "MTL/punk.h"
#include "MTL/win32/box.h"
#include "MTL/util/path.h"
#include "MTL/obj/collection.h"
#include "MTL/obj/marshall.h"
#include "MTL/ole/shell.h"
#include <string>
#include <sstream>
#include <deque>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ShlObj.h>

using namespace MTL;

class ShellTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    ShellTest() {
        // You can do set-up work for each test here.
    }

    ~ShellTest() override {
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

TEST_F(ShellTest, desktop) {

    try
    {
        auto desk = desktop();

        auto win = desk.getSpecialFolder(CSIDL_WINDOWS);

//        punk<IPersistFolder2> pf2(win.folder);
 //       LPITEMIDLIST pidl = 0;
 //       pf2->GetCurFolder(&pidl);

        std::wstring dn = desk.getDisplayName(*win);
  //      desk.release_pidl(pidl);

        std::wcout << dn << std::endl;

        EXPECT_STREQ(L"C:\\Windows", dn.c_str());
    }
    catch (HRESULT hr)
    {
        EXPECT_EQ(S_OK, hr);
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }
}


TEST_F(ShellTest, enumerate) {

    bool hasNotepad = false;

    try
    {
        auto desk = desktop();

        auto win = desk.getSpecialFolder(CSIDL_WINDOWS);

        //        punk<IPersistFolder2> pf2(win.folder);
         //       LPITEMIDLIST pidl = 0;
         //       pf2->GetCurFolder(&pidl);

        auto path = desk.getDisplayName(*win);

        MTL::Shell::Folder folder(path);
        /*
        ULONG chEaten = 0;
        LPITEMIDLIST pidl = 0;
        HR hr = desk.folder->ParseDisplayName(0, 0, (wchar_t*)path.c_str(), &chEaten, &pidl, 0);
        punk<IShellFolder> f;
        hr = desk.folder->BindToObject(pidl, 0, IID_IShellFolder, (void**)&f);
        desk.release_pidl(pidl);

        MTL::Shell::Folder folder(*f);
        */
        std::wcout << folder.getDisplayName() << std::endl;
//        std::wcout << desk.getDisplayName(f) << std::endl;

        auto enumerator = folder.enumerate();

        Shit shit = enumerator.next();

        while (shit)
        {
            std::wcout << folder.getDisplayName(*shit) << std::endl;

            if (folder.getDisplayName(*shit) == L"C:\\Windows\\notepad.exe")
            {
                hasNotepad = true;
            }
            shit = enumerator.next();
        }

//        EXPECT_STREQ(L"C:\\Windows", dn.c_str());
    }
    catch (HRESULT hr)
    {
        EXPECT_EQ(S_OK, hr);
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }

    EXPECT_TRUE(hasNotepad);
}


TEST_F(ShellTest, copy) 
{
    std::wstring selfPath = MTL::pathToSelf();
    bool copied = false;
    bool renamed = false;
    bool removed = false;
    try
    {
        std::wstring to = Path(selfPath).parentDir().str() + L"/tmp.exe";
        MTL::Shell::Copy(nullptr,selfPath, to);

        copied = Path(to).exists();

        std::wstring to2 = Path(selfPath).parentDir().str() + L"/tmp2.exe";

        MTL::Shell::Move(nullptr,to, to2);

        renamed = Path(to2).exists();
        removed = !Path(to).exists();

        EXPECT_TRUE(removed);

        MTL::Shell::Remove(nullptr, to2);
        removed = !Path(to2).exists();
    }
    catch (HRESULT hr)
    {
        EXPECT_EQ(S_OK, hr);
        std::wstring errMsg = HR::msg(hr);
        std::cout << "HR: " << hr << " " << to_string(errMsg) << std::endl;
    }

    EXPECT_TRUE(copied);
    EXPECT_TRUE(renamed);
    EXPECT_TRUE(removed);
}