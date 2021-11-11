#include "gtest/gtest.h"
#include "MTL/win32/uni.h"
#include "MTL/disp/bstr.h"
#include "MTL/disp/disp.h"
#include "MTL/disp/variant.h"
#include "MTL/disp/sf_array.h"
#include "MTL/punk.h"
#include "MTL/win32/box.h"
#include "MTL/util/path.h"
#include "MTL/obj/localserver.h"
#include "MTL/persist/persist.h"
#include "mtl/persist/json.h"
#include "MTL/win32/mem.h"
#include "MTL/persist/stgm.h"
#include "MTL/util/path.h"
#include "MTL/ole/dataobj.h"
#include "mtl/disp/typeinfo.h"

//#include "metacpp/xml.h"
#include "../test_h.h"

using namespace mtl;

class StreamTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    StreamTest() {
        // You can do set-up work for each test here.
    }

    ~StreamTest() override {
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

};

class SubObject : public implements<SubObject(dual<ISubObject>, streamable)> 
{
public:

    bstr name_;
    variant value_;

    virtual HRESULT get_Name(BSTR* cnt)
    {
        name_.copy_to(cnt);
        return S_OK;
    }
    virtual HRESULT put_Name(BSTR cnt)
    {
        name_ = bstr(b_copy(cnt));
        return S_OK;
    }
    virtual HRESULT get_Value(VARIANT* cnt)
    {
        value_.copy_to(cnt);
        return S_OK;
    }
    virtual HRESULT put_Value(VARIANT cnt)
    {
        value_ = cnt;
        return S_OK;
    }
};

template<>
struct meta::Data<SubObject>
{
    static constexpr auto meta()
    {
        return meta::data(
            entity_root("SUBOBJ"),
            "name", &SubObject::name_,
            "value", &SubObject::value_
        );
    }
};




class 
    TestObject : public implements<TestObject( dual<ITestObject>, persistable, class_info, 
        error_info<void(ITestObject, IDispatch)>)>
//        SupportErrorInfo<TestObject(ITestObject,IDispatch)> )>
{
public:

    VARIANT_BOOL mode = VARIANT_TRUE;
    RECT rect;
    long value;
    bstr desc;
    variant type;
    punk<IDispatch> disp;

    std::vector<bstr> array;

    TestObject()
    {
        rect = { 0,0,100,100 };
    }

    HRESULT __stdcall get_Value(long* cnt) override
    {
        if (!cnt) return E_INVALIDARG;
        *cnt = value;
        return S_OK;
    }

    HRESULT __stdcall put_Value( long cnt) override
    {
        value = cnt;
        return S_OK;
    }

    HRESULT __stdcall get_Desc( BSTR* cnt) override
    {
        if (!cnt) return E_INVALIDARG;
        return desc.copy_to(cnt);
    }

    HRESULT __stdcall put_Desc(BSTR cnt) override
    {
        desc = b_copy(cnt);
        return S_OK;
    }

    HRESULT __stdcall get_Type( VARIANT* cnt) override
    {
        if (!cnt) return E_INVALIDARG;
        type.copy_to(cnt);
        return S_OK;
    }

    HRESULT __stdcall put_Type( VARIANT cnt) override
    {
        type = cnt;
        return S_OK;
    }

    HRESULT __stdcall get_Object(IDispatch** cnt) override
    {
        if (!cnt) return E_INVALIDARG;
        return disp.query_interface(cnt);
    }

    HRESULT __stdcall put_Object(IDispatch* cnt) override
    {
        disp = cnt;
        return S_OK;
    }

};

template<>
struct meta::Data<TestObject>
{
    static constexpr auto meta()
    {
        return meta::data(
            entity_root("TestObject"),
            "value", &TestObject::value,
            "desc", &TestObject::desc,
            "mode", &TestObject::mode,
            "type", &TestObject::type,
            "array", &TestObject::array,
            "object", &TestObject::disp
        );
    }
};


TEST_F(StreamTest, testDispTypeLibLoadingStream) 
{
    punk<TestObject> from(new TestObject());

    from->put_Value(42);
    from->put_Desc(*bstr(L"Helo"));
    from->put_Type(variant(4711));

    from->array.push_back(bstr(L"One"));
    from->array.push_back(bstr(L"Two"));
    from->array.push_back(bstr(L"Three"));


    auto doc = toXml(**from);

    bstr xml;
    doc->get_xml(&xml);
   // std::string xml = doc->documentElement()->innerXml();
    std::cout << xml.to_string() << std::endl;
    

    punk<TestObject> to(new TestObject());
    to->array.push_back(bstr(L"XXX"));
    
    TestObject& t2 = **to;
    fromXml(xml.to_string(), t2);

    long l = 0;
    to->get_Value(&l);

    bstr b;
    to->get_Desc(&b);

    variant v;
    to->get_Type(&v);

    EXPECT_EQ(42, l);
    EXPECT_STREQ( L"Helo", b.str().c_str());
    EXPECT_EQ(VT_I4, v.vt);
    EXPECT_EQ(4711, v.lVal);
    EXPECT_EQ(100, to->rect.bottom);
    
    EXPECT_EQ(3, to->array.size());
    EXPECT_STREQ(L"Three", to->array[2].str().c_str() );
    
    /*
    auto doc2 = toXml(**to);
    std::string xml2 = doc2->documentElement()->innerXml();
    std::cout << xml2 << std::endl;
    */
    
}

TEST_F(StreamTest, testDispTypeLibLoading)
{
    punk<TestObject> to(new TestObject());

    to->put_Value(42);
    to->put_Desc(*bstr(L"Helo"));
    to->put_Type(variant(false));

    long l = 0;
    to->get_Value(&l);

    bstr b;
    to->get_Desc(&b);

    variant v;
    to->get_Type(&v);

    EXPECT_EQ(42, l);
    EXPECT_STREQ(L"Helo", b.str().c_str());
    EXPECT_EQ(VT_BOOL, v.vt);
    EXPECT_EQ(VARIANT_FALSE, v.boolVal);


}

TEST_F(StreamTest, testDispTypeLibLoadingStreamPersist)
{
    MTA enter;
    mtl::local_server<TestObject,SubObject> server;

    punk<ITestObject> from;
    from.create_object<TestObject>();

    from->put_Value(42);
    from->put_Desc(*bstr(L"Helo"));

    punk<ISubObject> subFrom;
    subFrom.create_object<SubObject>();
    subFrom->put_Name(*bstr(L"MyName"));
    subFrom->put_Value(variant(4711));
    from->put_Object(*subFrom);
    from->put_Type(variant(*subFrom));

    mtl::stream stream;

    punk<IPersistStream> psFrom(from);
    psFrom->Save(*stream,FALSE);

    stream.reset();

    std::string xml = stream.read();
    std::cout << xml << std::endl;

    stream.reset();

    punk<ITestObject> to;
    to.create_object<TestObject>();

    punk<IPersistStream> psTo(to);

    psTo->Load(*stream);

    long l = 0;
    to->get_Value(&l);

    bstr b;
    to->get_Desc(&b);

    variant v;
    to->get_Type(&v);

    punk<IDispatch> disp;
    to->get_Object(&disp);

    punk<ISubObject> so(disp);

    bstr n;
    so->get_Name(&n);
    variant val;
    so->get_Value(&val);

    EXPECT_EQ(42, l);
    EXPECT_STREQ(L"Helo", b.str().c_str());
    EXPECT_EQ(VT_DISPATCH, v.vt);
    EXPECT_STREQ(L"MyName",n.str().c_str());
    EXPECT_EQ(VT_I4, val.vt);
    EXPECT_EQ(4711, val.lVal);

}


TEST_F(StreamTest, testDispTypeLibLoadingStreamPersistFile)
{
    MTA enter;
    mtl::local_server<TestObject, SubObject> server;

    punk<ITestObject> from;
    from.create_object<TestObject>();

    from->put_Value(42);
    from->put_Desc(*bstr(L"Helo"));

    punk<ISubObject> subFrom;
    subFrom.create_object<SubObject>();
    subFrom->put_Name(*bstr(L"MyName"));
    subFrom->put_Value(variant(4711));
    from->put_Object(*subFrom);
    from->put_Type(variant(*subFrom));

    mtl::path p(L"test.xml");
    std::wstring filename(p.absolute().str());
    std::wcout << filename << std::endl;

    punk<IPersistFile> psFrom(from);
    psFrom->Save(filename.c_str(), FALSE);

    punk<ITestObject> to;
    to.create_object<TestObject>();

    punk<IPersistFile> psTo(to);

    psTo->Load(filename.c_str(), STGM_READ);

    long l = 0;
    to->get_Value(&l);

    bstr b;
    to->get_Desc(&b);

    variant v;
    to->get_Type(&v);

    punk<IDispatch> disp;
    to->get_Object(&disp);

    punk<ISubObject> so(disp);

    bstr n;
    so->get_Name(&n);
    variant val;
    so->get_Value(&val);

    EXPECT_EQ(42, l);
    EXPECT_STREQ(L"Helo", b.str().c_str());
    EXPECT_EQ(VT_DISPATCH, v.vt);
    EXPECT_STREQ(L"MyName", n.str().c_str());
    EXPECT_EQ(VT_I4, val.vt);
    EXPECT_EQ(4711, val.lVal);

}


TEST_F(StreamTest, testDispTypeLibLoadingStreamPersistStorage)
{
    MTA enter;
    mtl::local_server<TestObject, SubObject> server;

    punk<ITestObject> from = from_clsid(CLSID_TestObject);

    from->put_Value(42);
    from->put_Desc(*bstr(L"Helo"));

    punk<ISubObject> subFrom = from_object<SubObject>();
    subFrom->put_Name(*bstr(L"MyName"));
    subFrom->put_Value(variant(4711));
    from->put_Object(*subFrom);
    from->put_Type(variant(*subFrom));

    std::wstring filename(L"C:\\moe\\test.stg");

    mtl::storage storage;
    storage.create(filename);

    punk<IPersistStorage> psFrom(from);
    psFrom->Save(*storage, FALSE);

    storage.release();
    storage.open(filename);

    punk<ITestObject> to = from_object<TestObject>();

    punk<IPersistStorage> psTo(to);
    psTo->Load(*storage);

    long l = 0;
    to->get_Value(&l);

    bstr b;
    to->get_Desc(&b);

    variant v;
    to->get_Type(&v);

    punk<IDispatch> disp;
    to->get_Object(&disp);

    punk<ISubObject> so(disp);

    bstr n;
    so->get_Name(&n);
    variant val;
    so->get_Value(&val);

    EXPECT_EQ(42, l);
    EXPECT_STREQ(L"Helo", b.str().c_str());
    EXPECT_EQ(VT_DISPATCH, v.vt);
    EXPECT_STREQ(L"MyName", n.str().c_str());
    EXPECT_EQ(VT_I4, val.vt);
    EXPECT_EQ(4711, val.lVal);
}


TEST_F(StreamTest, testDispTypeLibLoadingStreamPersistStorageCLassInfo)
{
    MTA enter;
    mtl::local_server<TestObject, SubObject> server;

    punk<ITestObject> from = from_clsid(CLSID_TestObject);

    punk<IProvideClassInfo> pci(from);

    punk<ITypeInfo> ti;
    HR hr = pci->GetClassInfoW(&ti);

    bstr name;
    hr = ti->GetDocumentation(MEMBERID_NIL, &name, 0, 0, 0);

    std::wcout << name.str() << std::endl;
    EXPECT_STREQ(L"TestObject", name.str().c_str());
}


TEST_F(StreamTest, testDispTypeLibLoadingStreamPersistStorageErrorInfo)
{
    MTA enter;
    mtl::local_server<TestObject, SubObject> server;

    punk<ITestObject> from = from_clsid(CLSID_TestObject);

    punk<ISupportErrorInfo> sei(from);

    HRESULT hr = sei->InterfaceSupportsErrorInfo(IID_ITestObject);
    EXPECT_EQ(S_OK, hr);

    hr = sei->InterfaceSupportsErrorInfo(IID_IDispatch);
    EXPECT_EQ(S_OK, hr);

    hr = sei->InterfaceSupportsErrorInfo(IID_IUnknown);
    EXPECT_EQ(S_FALSE, hr);
}


TEST_F(StreamTest, parsetypelib) 
{
    auto lib = mtl::load_typelib(mtl::path_to_self());

    //auto json = meta::toJson(lib);
   // std::string str = JSON::stringify(json);

   // std::cout << str << std::endl;

   // mtl::typelib::MetaParam mp{ L"int", L"param1" };

    //auto json = meta::toXml(mp);
    mtl::punk<IXMLDOMDocument> xml = mtl::toXml(lib);

    mtl::bstr xmlStr;
    xml->get_xml(&xmlStr);

    std::wcout << xmlStr.str() << std::endl;

    auto json = meta::toJson(lib);
    std::string s = JSON::stringify(json);

    std::cout << s << std::endl;
}

