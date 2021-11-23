#pragma once

#include "mvc.h"
#include "service.h"

class EditorView;

class Documents
{
public:

	void insert(std::wstring id, EditorDocument* doc)
	{
		documents_[id] = std::shared_ptr<EditorDocument>(doc);
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

	EditorDocument& operator[](const std::wstring& id)
	{
		static EditorDocument empty;
		if (documents_.count(id))
		{
			return *documents_[id];
		}
		return empty;
	}

private:

	std::map<std::wstring, std::shared_ptr<EditorDocument>> documents_;
};

class EditorModel
{
public:

	Documents documents;

	//mtl::event<void()> onUpdate;
	mtl::event<void(std::wstring, std::wstring)> onFileChanged;

	EditorModel(FileService& fileService, RotService rotService, ScriptService& scriptService)
		: fileService_(fileService),
		rotService_(rotService),
		scriptService_(scriptService)
	{
		instanceId_ = mtl::new_guid();
	}

	std::wstring instanceId() { return instanceId_; }
	std::wstring activeDocument() { return activeDocument_; }

	void activate(const std::wstring& id)
	{
		if (documents.exists(id))
		{
			activeDocument_ = id;
		}
	}


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

	std::wstring instanceId_;
	std::wstring activeDocument_;
};

