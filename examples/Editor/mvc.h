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

class EditorDocument
{
public:
	std::wstring id;
	TextFile textFile;
	std::wstring fileWatchToken;
};

class IEditorModel
{
public:

	mtl::event<void()> onUpdate;
	mtl::event<void(std::wstring, std::wstring)> onFileChanged;

	virtual ~IEditorModel() {}
	virtual std::wstring instanceId() const;
	virtual std::wstring activeDocument() const;
	virtual void activeDocument(std::wstring) ;

	std::wstring instanceId;
	std::wstring activeDocument;
	bool regexSearch = false;
	std::map<std::wstring, std::shared_ptr<EditorDocument>> documents;

	EditorDocument openNew();
	EditorDocument transferDocument(const std::wstring& from);
	IO_ERROR openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(EditorDocument)> cb);
	void insertDocument(const std::wstring& id, TextFile& textFile);
	void saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb);
	void removeDocument(const std::wstring& id, const std::wstring& active);
	void reloadFile(const std::wstring& id, std::function<void(std::string)> cb);
};