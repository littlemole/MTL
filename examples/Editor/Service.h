#pragma once

#include "mvc.h"


class FileService
{
public:

	mtl::file_monitor monitor;

	mtl::path find(const std::wstring& relative_path);
	mtl::path find_in(const std::wstring& relative_path, const std::wstring& root);

	IO_ERROR read(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR, TextFile)> cb);
	IO_ERROR write(TextFile textFile, std::function<void(IO_ERROR)> cb);

	IO_ERROR read_admin(const std::wstring& path, long encoding, bool readonly, std::function<void(IO_ERROR, TextFile)> cb);
	IO_ERROR write_admin(TextFile textFile, std::function<void(IO_ERROR)> cb);
};

class RotService
{
public:

	EditorDocument transferTab(std::wstring instance, const std::wstring& from);

};


class ScriptService;

class Script : public std::enable_shared_from_this<Script>
{
public:

	mtl::chakra::runtime rt;
	mtl::chakra::script_ctx scriptContext;
	unsigned currentSourceContext = 0;

	Script(HWND mainWnd, ScriptService& service, FileService& fs, const std::wstring& id, const std::wstring& s, const std::wstring& fn);

	~Script();

	bool run(mtl::punk<IUnknown> obj);

	UINT_PTR set_timeout(unsigned int ms, IDispatch* cb);

	void dispose();
	void close();

	void importSource(std::wstring file);

	std::wstring filename() { return filename_; }
	std::wstring id() { return id_; }
	bool wait() { return wait_; }
	void wait(bool b) { wait_ = b; }

	void onError(std::function<void(long, long, std::wstring, std::wstring)> cb);


private:

	ScriptService& scriptService_;
	FileService& fileService_;

	HWND hWnd = nullptr;
	bool wait_ = false;

	std::wstring id_;
	std::wstring source_;
	std::wstring filename_;

	mtl::punk<IUnknown> hostObj_;
	std::function<void(long, long, std::wstring, std::wstring)> onError_;
	std::set<UINT_PTR> timeouts_;
};


class ScriptService
{
public:

	HWND mainWnd = nullptr;
	FileService& fileService;

	ScriptService( FileService& service);
	~ScriptService();

	void start(mtl::punk<IUnknown> unk, HWND hWnd);
	void stop();

	void run(std::wstring scriptSource, std::wstring filename, std::function<void(long, long, std::wstring, std::wstring)> onError);

	void getScripts(std::function<void(std::map<std::wstring, std::wstring>)> cb);

	void kill(std::wstring id);

	// private
	bool isScript(const std::wstring& id);
	void erase(std::wstring id);


private:

	void threadfun(mtl::proxy<IUnknown> p);

	mtl::punk<IUnknown> unk_;
	std::thread worker_;
	mtl::thread_box<void()> box_;

	std::map<std::wstring, std::shared_ptr<Script>> scripts;
};


