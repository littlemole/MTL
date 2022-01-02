#pragma once

#include "mtl/sdk.h"
#include "mtl/obj/impl.h"
#include "mtl/disp/bstr.h"
#include "mtl/disp/variant.h"

#define USE_EDGEMODE_JSRT
#include <jsrt.h>


namespace mtl {

	namespace chakra {

		class prop
		{
		public:

			prop() {}

			prop( ::JsValueRef obj, ::JsPropertyIdRef ref)
				: value(obj), id(ref)
			{}

			prop( ::JsValueRef obj, int idx)
				: value(obj), index(idx)
			{}

			~prop() {}

			operator ::JsValueRef()
			{
				::JsValueRef result = nullptr;
				if (value && id)
				{
					::JsGetProperty(value, id, &result);
				}
				else if (value)
				{
					::JsValueRef idx;
					::JsIntToNumber(index, &idx);
					::JsGetIndexedProperty(value, idx, &result);
				}
				return result;
			}

			prop& operator=(::JsValueRef ref)
			{
				if (value && id)
				{
					::JsSetProperty(value, id, ref, true);
				}
				else if (value)
				{
					::JsValueRef idx;
					::JsIntToNumber(index, &idx);
					::JsSetIndexedProperty(value, idx, ref);
				}
				return *this;
			}

			::JsPropertyIdRef operator*()
			{
				return id;
			}

		private:
			::JsValueRef value = nullptr;
			::JsPropertyIdRef id = nullptr;
			int index = 0;
		};

		class value
		{
		public:
			value() {}

			value( ::JsValueRef value)
				: handle(value)
			{
				::JsAddRef(handle, nullptr);
			}

			static ::JsValueRef from_double(double value)
			{
				::JsValueRef result = nullptr;
				::JsDoubleToNumber(value, &result);
				return result;
			}

			static ::JsValueRef from_int(int value)
			{
				::JsValueRef result = nullptr;
				::JsIntToNumber(value, &result);
				return result;
			}

			static ::JsValueRef from_string(const std::wstring& str)
			{
				::JsValueRef result = nullptr;
				::JsPointerToString(str.c_str(), str.size(), &result);
				return result;
			}

			static ::JsValueRef from_bool(bool b)
			{
				::JsValueRef result = nullptr;
				::JsBoolToBoolean(b, &result);
				return result;
			}

			static ::JsValueRef from_variant(VARIANT* var)
			{
				::JsValueRef result = nullptr;
				::JsVariantToValue(var, &result);
				return result;
			}

			static ::JsValueRef undefined()
			{
				::JsValueRef result = nullptr;
				::JsGetUndefinedValue(&result);
				return result;
			}

			static ::JsValueRef null()
			{
				::JsValueRef result = nullptr;
				::JsGetNullValue(&result);
				return result;
			}

			~value()
			{
				if (handle)
				{
					::JsRelease(handle, nullptr);
					handle = nullptr;
				}
			}

			value(const value& rhs)
				: handle(rhs.handle)
			{
				if (handle)
				{
					::JsAddRef(handle, nullptr);
				}
			}

			value(value&& rhs)
				: handle(rhs.handle)
			{
				rhs.handle = nullptr;
			}

			value& operator=(const value& rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}
				handle = rhs.handle;
				if (handle)
				{
					::JsAddRef(handle, nullptr);
				}
				return *this;
			}

			value& operator=(value&& rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}
				handle = rhs.handle;
				rhs.handle = nullptr;
				return *this;
			}

			JsValueRef operator*() const
			{
				return handle;
			}

			prop operator[](const std::wstring& key)
			{
				prop p(handle, property_id(key));
				return p;
			}

			prop operator[](int index)
			{
				prop p(handle, index);
				return p;
			}

			bool exists(std::wstring& key)
			{
				bool result = false;
				if (handle)
				{
					::JsHasProperty(handle, property_id(key), &result);
				}
				return result;
			}

			JsValueRef remove(std::wstring& key)
			{
				JsValueRef result = nullptr;
				if (handle && exists(key))
				{
					::JsDeleteProperty(handle, property_id(key), true, &result);
				}
				return result;
			}

			JsPropertyIdRef property_id(const std::wstring& name)
			{
				JsPropertyIdRef result = nullptr;
				::JsGetPropertyIdFromName(name.c_str(), &result);
				return result;
			}

			JsPropertyIdRef property(const std::wstring& name)
			{
				JsValueRef result = nullptr;
				if (handle)
				{
					JsPropertyIdRef id = property_id(name);
					::JsGetProperty(handle, id, &result);
				}
				return result;
			}

			JsPropertyIdRef property(const std::wstring& name, JsValueRef value)
			{
				JsValueRef result = nullptr;
				if (handle)
				{
					JsPropertyIdRef id = property_id(name);
					::JsSetProperty(handle, id, value, true);
				}
				return result;
			}

			JsValueType type()
			{
				JsValueType result = JsUndefined;
				if (handle)
				{
					::JsGetValueType(handle, &result);
				}
				return result;
			}

			mtl::variant as_variant()
			{
				mtl::variant result;
				if (handle)
				{
					JsErrorCode ec = ::JsValueToVariant(handle, &result);
				}
				return result;
			}

			bool as_bool()
			{
				bool result = false;
				if (handle)
				{
					::JsBooleanToBool(handle, &result);
				}
				return result;
			}

			int as_int()
			{
				int result = 0;
				if (handle)
				{
					::JsNumberToInt(handle, &result);
				}
				return result;
			}

			double as_double()
			{
				double result = 0;
				if (handle)
				{
					::JsNumberToDouble(handle, &result);
				}
				return result;
			}

			std::wstring as_string()
			{
				std::wstring result;
				if (handle)
				{
					const wchar_t* buf = nullptr;
					size_t len = 0;
					::JsStringToPointer(handle, &buf, &len);
					result = std::wstring(buf, len);
				}
				return result;
			}


			std::wstring to_string()
			{
				std::wstring result;
				if (handle)
				{
					::JsValueRef tmp = nullptr;
					::JsConvertValueToString(handle, &tmp);

					const wchar_t* buf = nullptr;
					size_t len = 0;
					::JsStringToPointer(tmp, &buf, &len);
					result = std::wstring(buf, len);
				}
				return result;
			}


		private:
			JsValueRef handle = nullptr;
		};

		template<class F>
		class function
		{
		public:

			function(F f)
				:fun(f)
			{}
			
			JsValueRef callback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount)
			{
				return fun(callee, isConstructCall, arguments, argumentCount);
			}

			static JsValueRef CALLBACK functionCallback(JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, void* callbackState)
			{
				function* f = (function*)callbackState;
				return f->callback(callee, isConstructCall, arguments, argumentCount);
			}

		private:
			F fun;
		};

		class script_ctx
		{
		public:

			script_ctx()
			{}

			script_ctx( ::JsContextRef ref)
				: ctx(ref)
			{}

			~script_ctx()
			{
				dispose();
			}

			script_ctx(const script_ctx& rhs) = delete;
			script_ctx(script_ctx&& rhs) noexcept
			{
				ctx = rhs.ctx;
				rhs.ctx = nullptr;
			}

			script_ctx& operator=(const script_ctx& rhs) = delete;
			script_ctx& operator=(script_ctx&& rhs) noexcept
			{
				if (this == &rhs)
				{
					return *this;
				}

				ctx = rhs.ctx;
				rhs.ctx = nullptr;
				return *this;
			}

			void dispose()
			{
				if (ctx)
				{
					JsContextRef ref = nullptr;
					::JsGetCurrentContext(&ref);
					if (ref == ctx)
					{
						::JsSetCurrentContext(nullptr);
					}
					::JsRelease(ctx, nullptr);
					ctx = nullptr;
				}
			}

			::JsContextRef operator*()
			{
				return ctx;
			}


			bool hasException()
			{
				bool result = false;
				::JsHasException(&result);
				return result;
			}

			JsValueRef getAndClearException()
			{
				JsValueRef result = nullptr;
				::JsGetAndClearException(&result);
				return result;
			}


		private:
			::JsContextRef ctx = nullptr;
		};

		class active_ctx
		{
		public:

			active_ctx()
			{
				::JsGetCurrentContext(&handle);
			}

			active_ctx(JsContextRef ctx)
				:handle(ctx)
			{
				::JsContextRef ref = nullptr;
				::JsGetCurrentContext(&ref);

				if (ref == ctx) return;
 
				previous = ref;
				::JsSetCurrentContext(handle);
			}

			~active_ctx()
			{
				if (handle)
				{
					if (previous)
					{
						//::JsSetCurrentContext(previous);
						previous = nullptr;
					}
					handle = nullptr;
				}
			}

			operator bool()
			{
				return handle != nullptr;
			}

			active_ctx(const active_ctx& rhs) = delete;
			active_ctx(active_ctx&& rhs) = delete;

			active_ctx& operator=(const active_ctx& rhs) = delete;
			active_ctx& operator=(active_ctx&& rhs) = delete;

			JsContextRef operator*()
			{
				return handle;
			}

			JsValueRef create_object()
			{
				JsValueRef result;
				::JsCreateObject(&result);
				return result;
			}

			JsValueRef from_variant(VARIANT* var)
			{
				JsValueRef result;

				::JsVariantToValue(var, &result);
				return result;
			}

			mtl::variant to_variant(JsValueRef value)
			{
				mtl::variant result;
				JsErrorCode ec = ::JsValueToVariant(value, &result);
				if (ec != JsNoError)
				{
					::MessageBox(0, L"ERRO CREATE VARIANT", L"x", 0);
				}
				return result;
			}


			JsValueRef global()
			{
				JsValueRef result;
				::JsGetGlobalObject(&result);
				return result;
			}

			JsPropertyIdRef property_id(const std::wstring& name)
			{
				JsPropertyIdRef result;
				::JsGetPropertyIdFromName(name.c_str(), &result);
				return result;
			}

			JsPropertyIdRef property(JsValueRef obj, const std::wstring& name)
			{
				JsValueRef result;
				JsPropertyIdRef id = property_id(name);
				::JsGetProperty(obj, id, &result);
				return result;
			}

			JsPropertyIdRef property(JsValueRef obj, const std::wstring& name, JsValueRef value)
			{
				JsValueRef result;
				JsPropertyIdRef id = property_id(name);
				::JsSetProperty(obj, id, value, true);
				return result;
			}

			JsValueType get_type(JsValueRef value)
			{
				JsValueType result;
				::JsGetValueType(value, &result);
				return result;
			}


			JsValueRef make_fun(JsNativeFunction fun,void* data = nullptr)
			{
				JsValueRef result;
				::JsCreateFunction(fun, data, &result);
				return result;
			}

			template<class T>
			JsValueRef make_fun(function<T>& f)
			{
				JsValueRef result;
				::JsCreateFunction(&function<T>::functionCallback, &f, &result);
				return result;
			}

			JsValueRef run(const std::wstring& source, const std::wstring& fn)
			{
				static unsigned currentSourceContext = 0;

				::JsValueRef result;
				if (JsNoError != ::JsRunScript(source.c_str(), currentSourceContext, fn.c_str(), &result))
				{
					//::MessageBox(0, L"JS ERROR", L"ERR 1", MB_ICONERROR);
				}
				return result;
			}

			bool hasException()
			{
				bool result = false;
				::JsHasException(&result);
				return result;
			}

			JsValueRef getAndClearException()
			{
				JsValueRef result = nullptr;
				::JsGetAndClearException(&result);
				return result;
			}

		private:
			JsContextRef previous = nullptr;
			JsContextRef handle = nullptr;
		};


		class runtime
		{
		public:

			runtime()
			{
				if (JsNoError != ::JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &handle))
				{
					::OutputDebugString(L"Chakra JSRT init failed");
					exit(0);
				}
			}

			runtime(JsRuntimeAttributes attrs)
			{
				if (JsNoError != ::JsCreateRuntime(attrs, nullptr, &handle))
				{
					::OutputDebugString(L"Chakra JSRT init failed");
					exit(0);
				}
				
			}

			~runtime()
			{
				dispose();
			}

			void dispose()
			{
				if (handle)
				{
					::JsSetCurrentContext(JS_INVALID_REFERENCE);
					::JsDisposeRuntime(handle);
					handle = nullptr;
				}
			}

			runtime(const runtime& rhs) = delete;
			runtime(runtime&& rhs)
			{
				handle = rhs.handle;
				rhs.handle = nullptr;
			}

			runtime& operator=(const runtime& rhs) = delete;
			runtime& operator=(runtime&& rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				handle = rhs.handle;
				rhs.handle = nullptr;

				return *this;
			}

			JsRuntimeHandle operator*()
			{
				return handle;
			}

			JsContextRef make_context()
			{
				JsContextRef result;
				::JsCreateContext(handle, &result);
				return result;
			}

		private:
			JsRuntimeHandle handle = nullptr;
		};
	}
}