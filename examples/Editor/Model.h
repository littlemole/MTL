#pragma once

#include "mvc.h"
#include "service.h"

class EditorView;

class Documents
{
public:

	void insert(std::wstring id, Document* doc)
	{
		documents_[id] = std::shared_ptr<Document>(doc);
	}

	void erase(std::wstring id)
	{
		if (documents_.count(id))
		{
			documents_.erase(id);
		}
	}

	bool exists(const std::wstring& id)
	{
		return documents_.count(id);
	}

	size_t count()
	{
		return documents_.size();
	}

	Document& operator[](const std::wstring& id)
	{
		static EditorDocument empty;
		if (documents_.count(id))
		{
			return *documents_[id];
		}
		return empty;
	}

private:

	std::map<std::wstring, std::shared_ptr<Document>> documents_;
};

class EditorModel
{
public:

	Documents documents;

	mtl::event<void(std::wstring, std::wstring)> onFileChanged;

	EditorModel(FileService& fileService, RotService rotService, ScriptService& scriptService)
		: fileService_(fileService),
		rotService_(rotService),
		scriptService_(scriptService)
	{
		instanceId_ = mtl::new_guid();
	}

	std::wstring instanceId() { return instanceId_; }

	EditorDocument openNew();
	EditorDocument transferDocument(const std::wstring& from);
	IO_ERROR openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(TextFile)> cb);
	void insertDocument(const std::wstring& id, const TextFile& textFile);
	void saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb);
	void removeDocument(const std::wstring& id);
	void reloadFile(const std::wstring& id, std::function<void(std::string)> cb);


private:
	FileService& fileService_;
	RotService& rotService_;
	ScriptService& scriptService_;

	std::wstring instanceId_;
};

