#pragma once

#include "Editor.h"


enum EOL_TYPE {
	EOL_UNIX,
	EOL_DOS
};

enum IO_ERROR {
	IO_ERROR_SUCCESS,
	IO_ERROR_NOT_FOUND,
	IO_ERROR_ACCESS_DENIED
};

enum DOC_TYPE {
	DOC_TXT,
	DOC_IMG,
	DOC_HTML
};

class TextFile
{
public:
	bool readonly = false;
	std::wstring filename;
	unsigned long long size;
	time_t last_written;
	mtl::file_encoding fileEncoding;
	std::string utf8;
};

class Document
{
public:

	mtl::event<void(std::wstring)> onClose;

	virtual ~Document() 
	{
		onClose.fire(id_);
	};

	virtual std::wstring id() = 0;
	virtual std::wstring filename() = 0;
	virtual std::wstring fileWatchToken() = 0;
	virtual void fileWatchToken(std::wstring) = 0;
	virtual DOC_TYPE type() = 0;

protected:

	std::wstring id_;
	std::wstring fileWatchToken_;
};

class EditorDocument : public Document
{
public:

	TextFile textFile;

	EditorDocument()
	{}

	EditorDocument(const std::wstring& id, const TextFile& f, std::wstring t)
		: textFile(f)
	{
		id_ = id;
		fileWatchToken_ = t;
	}

	virtual std::wstring id() override;
	virtual std::wstring filename() override;
	virtual std::wstring fileWatchToken() override;
	virtual void fileWatchToken(std::wstring) override;
	virtual DOC_TYPE type() override;

};


class ImageDocument : public Document
{
public:

	std::wstring filename_;

	ImageDocument()
	{}

	ImageDocument(const std::wstring& id, std::wstring p, std::wstring t)
		: filename_(p)
	{
		id_ = id;
		fileWatchToken_ = t;
	}

	virtual std::wstring id() override;
	virtual std::wstring filename() override;
	virtual std::wstring fileWatchToken() override;
	virtual void fileWatchToken(std::wstring) override;
	virtual DOC_TYPE type() override;

};


class HtmlDocument : public Document
{
public:

	std::wstring filename_;

	HtmlDocument()
	{}

	HtmlDocument(const std::wstring& id, const std::wstring& f, std::wstring t)
		:filename_(f)
	{
		id_ = id;
		fileWatchToken_ = t;
	}

	virtual std::wstring id() override;
	virtual std::wstring filename() override;
	virtual std::wstring fileWatchToken() override;
	virtual void fileWatchToken(std::wstring) override;
	virtual DOC_TYPE type() override;

};
