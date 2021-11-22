#pragma once

#include "mvc.h"

class EditorView;

class EditorModel
{
public:

	mtl::event<void()> onUpdate;
	mtl::event<void(std::wstring, std::wstring)> onFileChanged;

	EditorModel(FileService& fileService, RotService rotService, ScriptService& scriptService)
		: fileService_(fileService),
		rotService_(rotService),
		scriptService_(scriptService)
	{
		instanceId = mtl::new_guid();
	}

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
private:
	FileService& fileService_;
	RotService& rotService_;
	ScriptService& scriptService_;
};

