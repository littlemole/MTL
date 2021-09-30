#pragma once

#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
//C:\Users\mike\source\repos\vcpkg\installed\x64-windows\include
#include "metacpp/meta.h"

///////////////////////////////////////////////////////////////////////////////////////////

namespace mtl {
	namespace JSON {

		using namespace nlohmann;

		class ParseEx : public std::exception
		{
		public:
			ParseEx(const std::string& s)
				: msg(s)
			{}

			const char* what() const noexcept
			{
				return msg.c_str();
			}

			std::string msg;
		};

		//////////////////////////////////////////////////////////////

		//! parse string containing JSON 
		//! \ingroup json
		inline json parse(const std::string& txt)
		{
			try
			{
				json result = json::parse(txt);
				return result;
			}
			catch (...)
			{
				throw ParseEx("parsingn JSON failed");
			}
		}

		//////////////////////////////////////////////////////////////

		//! serialize JSON structure into plaintext
		//! \ingroup json

		inline const std::string stringify(json value)
		{
			std::string s = value.dump(4);
			return s;

		}

		//! flatten a JSON structure removing whitespace and newlines
		//! \ingroup json
		inline const std::string flatten(json value)
		{
			std::string s = value.dump();
			return s;
		}

	} // end namespace JSON

}

namespace meta {

	using namespace nlohmann;

	inline json exToJson(const std::exception& ex)
	{
		json err;
		err["type"] = typeid(ex).name();
		err["msg"] = ex.what();

		json result;
		result["error"] = err;
		return result;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	namespace impl {

		///////////////////////////////////////////////////////////////////////////////////////////
		// forwards

		template<class T>
		void toJson(const char* name, T& from, json& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

		template<class T>
		void toJson(const char* name, const T& from, json& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

		template<class T>
		void fromJson(const char* name, const json&, T& t);

		///////////////////////////////////////////////////////////////////////////////////////////

		inline void toJson(const char* name, json& from, json& to)
		{
			to[name] = from;
		}

		inline void toJson(const char* name, const json& from, json& to)
		{
			to[name] = from;
		}

		inline void toJson(const char* name, const std::string& from, json& to)
		{
			to[name] = from;
		}

		inline void toJson(const char* name, std::string& from, json& to)
		{
			to[name] = from;
		}




		inline void toJson(const char* name, const long long int& from, json& to)
		{
			to[name] = from;
		}

		template<class T >
		void toJson(const char* name, const T& from, json& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
		{
			to[name] = from;
		}

		template<class T>
		void toJson(const char* name, const std::vector<T>& from, json& to)
		{
			json result;

			for (auto& i : from)
			{
				json item;
				toJson(name, i, item);
				result.push_back(item[name]);
			}

			to[name] = result;
		}

		template<class T>
		void toJson(const char* name, std::vector<T>& from, json& to)
		{
			json result;

			for (auto& i : from)
			{
				json item;
				toJson(name, i, item);
				result.push_back(item[name]);
			}

			to[name] = result;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////

		inline void fromJson(const char* name, const json& from, std::string& t)
		{
			if (!name)
			{
				t = from;
				return;
			}

			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, int& t)
		{
			if (!name)
			{
				t = from;
				return;
			}
			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, unsigned int& t)
		{
			if (!name)
			{
				t = from;
				return;
			}

			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, double& t)
		{
			if (!name)
			{
				t = from;
				return;
			}
			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, float& t)
		{
			if (!name)
			{
				t = from;
				return;
			}
			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, bool& t)
		{
			if (!name)
			{
				t = from;
				return;
			}

			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, long long& t)
		{
			if (!name)
			{
				t = from;
				return;
			}

			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, unsigned long long& t)
		{
			if (!name)
			{
				t = from;
				return;
			}

			if (from.count(name))
			{
				t = from[name];
			}
		}

		inline void fromJson(const char* name, const json& from, char& t)
		{
			if (!name)
			{
				int tmp = from;
				t = (char)tmp;
				return;
			}

			if (from.count(name))
			{
				int tmp = from[name];
				t = (char)tmp;
			}
		}

		inline void fromJson(const char* name, const json& from, unsigned char& t)
		{
			if (!name)
			{
				int tmp = from;
				t = (unsigned char)tmp;
				return;
			}
			if (from.count(name))
			{
				int tmp = from[name];
				t = (unsigned char)tmp;
			}
		}

		inline void fromJson(const char* name, const json& from, json& to)
		{
			if (!name)
			{
				to = from;
				return;
			}
			if (from.count(name))
			{
				to = from[name];
			}
		}

		template<class T>
		void fromJson(const char* name, const json& from, std::vector<T>& v)
		{
			v.clear();

			if (name && !from.count(name))
			{
				return;
			}

			auto f = name ? from[name] : from;

			unsigned int size = (unsigned int)f.size();
			for (unsigned int i = 0; i < size; i++)
			{
				T t;
				fromJson(0, f[i], t);
				v.push_back(std::move(t));
			}
		}



		///////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////


		template<class T>
		void toJson(const char* n, const T& from, json& to, typename std::enable_if<std::is_class<T>::value>::type*)
		{
			json result;

			auto visitor = [&from, &to, n](auto name, auto m)
			{
				auto value = *m;
				toJson(name.name, value, to[n]);
			};
			meta::visit(from, visitor);
		}

		template<class T>
		void toJson(const char* n, T& from, json& to, typename std::enable_if<std::is_class<T>::value>::type*)
		{
			json result;

			auto visitor = [&to, n](auto name, auto m)
			{
				auto value = *m;
				toJson(name.name, value, to[n]);
			};
			meta::visit(from, visitor);
		}

		///////////////////////////////////////////////////////////////////////////////////////////

		template<class T>
		void fromJson(const char* name, const json& from, T& t)
		{
			if (name && !from.count(name))
				return;

			auto f = name ? from[name] : from;

			auto visitor = [&f](auto name, auto m)
			{
				std::remove_reference_t<typename decltype(m)::setter_value_t> value;
				fromJson(name.name, f, value);
				m = value;
			};

			meta::visit(t, visitor);
		}

	} // end namespace impl

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////

	template<class T>
	json toJson(const T& t)
	{
		json result;

		auto root = meta::entity_root<T>();

		if (root.name)
		{
			impl::toJson(root.name, t, result);
		}
		else
		{
			auto visitor = [&result](auto name, auto m)
			{
				auto value = *m;
				impl::toJson(name.name, value, result);
			};
			meta::visit(t, visitor);
		}

		return result;
	}

	template<class T>
	json toJson(T& t)
	{
		json result;

		auto root = meta::entity_root<T>();

		if (root.name)
		{
			impl::toJson(root.name, t, result);
		}
		else
		{
			auto visitor = [&result](auto name, auto m)
			{
				auto value = *m;
				impl::toJson(name.name, value, result);
			};
			meta::visit(t, visitor);
		}

		return result;
	}

	inline json toJson(const json& t)
	{
		return t;
	}

	inline json toJson(json& t)
	{
		return t;
	}

	template<class T>
	json toJson(const std::vector<T>& t)
	{
		json v;
		json& result = v;
		json root;

		for (auto& i : t)
		{
			json item;
			impl::toJson("dummy", i, item);
			v.push_back(item["dummy"]);
		}

		auto r = meta::entity_root<std::vector<T>>();
		if (r.name)
		{
			root[r.name] = v;
			result = root;
		}

		return result;
	}

	template<class T>
	json toJson(std::vector<T>& t)
	{
		json v;
		json& result = v;
		json root;

		for (auto& i : t)
		{
			json item;
			impl::toJson("dummy", i, item);
			v.push_back(item["dummy"]);
		}

		auto r = meta::entity_root<std::vector<T>>();
		if (r.name)
		{
			root[r.name] = v;
			result = root;
		}

		return result;
	}


	///////////////////////////////////////////////////////////////////////////////////////////


	template<class T>
	void fromJson(const json& from, T& t)
	{
		auto root = meta::entity_root<T>();

		impl::fromJson(root.name, from, t);
	}

	template<class T>
	void fromJson(const std::string& from, T& t)
	{
		json json = mtl::JSON::parse(from);
		fromJson(json, t);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////

} // end namespace meta
