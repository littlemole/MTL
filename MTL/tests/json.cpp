#include "gtest/gtest.h"
#include <memory>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include "metacpp/meta.h"
#include "MTL/persist/json.h"
 
using namespace meta;
using namespace nlohmann;

class JsonTest : public ::testing::Test {
protected:

	static void SetUpTestCase() {

	}

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
}; // end test setup

 

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

class TestObj
{
public:
	unsigned int x;
	std::string txt;

	const std::string&  get() const
	{
		return txt;
	}

	void set(  std::string s)
	{
		txt = s;
	}
};


template<>
struct meta::Data<TestObj>
{
	static constexpr auto meta()
	{
		return meta::data(
			//entity_root("TEST"),
			member("x", &TestObj::x),
			"txt", &TestObj::txt
			//getter_setter("txt", &Test::get, &Test::set)
		);
	}
};


class ArrayTest
{
public:

	std::vector<TestObj> test;

	constexpr static auto meta() 
	{
		return meta::data(
			meta::entity_root("huhu"),
			meta::member("test", &ArrayTest::test)
		);
	}
};


template<>
class meta::Data<std::vector<TestObj>>
{
public:

	constexpr static auto meta()
	{
		return meta::data(
			meta::entity_root("vector")
		);
	}
};


TEST_F(JsonTest, simpleJson)
{
	TestObj t{ 42, "hello meta" };

	json value = toJson(t);

	std::string s = MTL::JSON::flatten(value);

	EXPECT_STREQ("{\"txt\":\"hello meta\",\"x\":42}",s.c_str());

	TestObj t2;
	fromJson(value, t2);

	EXPECT_STREQ("hello meta",t2.txt.c_str());
	EXPECT_EQ(42,t2.x);
}


TEST_F(JsonTest, simpleJsonVector)
{
	TestObj t{ 42, "hello meta" };
    ArrayTest at;
    at.test.push_back(t);
    at.test.push_back(t);

	json value = toJson(at);

	std::string s = MTL::JSON::flatten(value);

	EXPECT_STREQ("{\"huhu\":{\"test\":[{\"txt\":\"hello meta\",\"x\":42},{\"txt\":\"hello meta\",\"x\":42}]}}",s.c_str());

	ArrayTest at2;
	fromJson(value, at2);

    json value2 = toJson(at2);
	s = MTL::JSON::flatten(value2);

	EXPECT_STREQ("{\"huhu\":{\"test\":[{\"txt\":\"hello meta\",\"x\":42},{\"txt\":\"hello meta\",\"x\":42}]}}",s.c_str());

}
 
TEST_F(JsonTest, jsonVector)
{
	TestObj t{ 42, "hello meta" };

    std::vector<TestObj> v;
    v.push_back(t);
    v.push_back(t);

	json value = toJson(v);

	std::string s = MTL::JSON::flatten(value);

	EXPECT_STREQ("{\"vector\":[{\"txt\":\"hello meta\",\"x\":42},{\"txt\":\"hello meta\",\"x\":42}]}",s.c_str());

    std::vector<TestObj> v2;
	fromJson(value, v2);

    json value2 = toJson(v2);
	s = MTL::JSON::flatten(value2);

	EXPECT_STREQ("{\"vector\":[{\"txt\":\"hello meta\",\"x\":42},{\"txt\":\"hello meta\",\"x\":42}]}",s.c_str());

}



struct User
{
public:

	std::string username;
	std::string login;
	std::string pwd;
	std::vector<std::string> tags;
};

template<>
struct meta::Data<User>
{
        static constexpr auto meta()
        {
                return meta::data(
					entity_root("user"),
					"username", &User::username,
					"login", &User::login,
					"pwd", &User::pwd,
					"tags", &User::tags
				);
		}
};



TEST_F(JsonTest, toJson) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	json value = toJson(user);

	std::string s = MTL::JSON::flatten(value);

	std::cout << s << std::endl;

	EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",s);

	User other;
	fromJson(value,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
}



TEST_F(JsonTest, toJsonConst) 
{
	const User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	json value = toJson(user);

	std::string s = MTL::JSON::flatten(value); 

	std::cout << s << std::endl;

	EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",s);

	User other;
	fromJson(value,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
}

