#include "Model.h"



void EditorModel::removeDocument(const std::wstring& id)
{
	if (!documents.exists(id)) return;

	fileService_.monitor.unwatch(documents[id].fileWatchToken(), documents[id].filename());

	documents.erase(id);
}

EditorDocument EditorModel::transferDocument(const std::wstring& from)
{
	return rotService_.transferTab(instanceId(), from);
}

/*
void EditorModel::updateStatus(std::wstring id)
{
	if (documents.count(id) == 0) return;

	//std::wstring path = documents[id]->textFile.filename;
	//int cp = documents[id]->textFile.fileEncoding.code_page;
	//EOL_TYPE eol = (EOL_TYPE)documents[id]->textFile.fileEncoding.eol;

	//view_.updateStatus( *documents[id] );
}
*/

void EditorModel::insertDocument(const std::wstring& id, Document* doc)
{

	std::wstring token = fileService_.monitor.watch(
		doc->filename(),
		[this, id]()
		{
			if (!documents.exists(id)) return;

			std::wstring path = documents[id].filename();
			this->onFileChanged.fire(id, path);
		}
	);

	//auto doc = new EditorDocument{ id, textFile, token };
	documents.insert(id, doc);
}

IO_ERROR EditorModel::openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(TextFile)> cb)
{
	IO_ERROR ie = fileService_.read(path, enc, readOnly, [this, cb](IO_ERROR e, TextFile textFile)
	{
		if (e == IO_ERROR_SUCCESS)
		{
			/*
			std::wostringstream woss;
			woss << instanceId() << ":" << mtl::new_guid();
			std::wstring id = woss.str();

			EditorDocument result{ id, std::move(textFile) , }
			result.id = id;
			result.textFile = std::move(textFile);
			*/
			cb(textFile);
		}
	});
	return ie;
}


EditorDocument EditorModel::openNew()
{
	std::wostringstream woss;
	woss << instanceId() << ":" << mtl::new_guid();
	std::wstring id = woss.str();

	std::wstring fn = L"New File.txt";

	TextFile textFile;
	textFile.filename = fn;
	textFile.last_written = time(0);
	textFile.readonly = false;
	textFile.size = 0;
	textFile.fileEncoding.code_page = CP_UTF8;
	textFile.fileEncoding.eol = mtl::file_encoding::UNIX;
	textFile.fileEncoding.has_bom = false;
	textFile.fileEncoding.is_binary = false;

	EditorDocument doc{ id, textFile, L"" };
	return doc;
}

void EditorModel::saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb)
{
	if (!documents.exists(id))
	{
		cb(IO_ERROR_NOT_FOUND);
		return;
	}

	auto& doc = documents[id];

	if (doc.type() != DOC_TXT)
	{
		return;
	}

	EditorDocument& document = dynamic_cast<EditorDocument&>(doc);

	TextFile textFile = document.textFile;

	textFile.utf8 = utf8;
	textFile.size = utf8.size();
	textFile.filename = path;
	textFile.last_written = time(0);

	if (enc != -1)
	{
		textFile.fileEncoding.code_page = enc;
	}

	textFile.fileEncoding.eol = (mtl::file_encoding::eol_mode)eol;

	fileService_.monitor.unwatch(doc.fileWatchToken(), path);

	fileService_.write(textFile, [this, id, textFile, cb](IO_ERROR e)
	{
		mtl::timer::set_timeout(500, [this, id, textFile, cb, e](UINT_PTR)
		{
			std::wstring token = fileService_.monitor.watch(
				textFile.filename,
				[this, id]()
			{
				if (documents.exists(id) == 0) return;

				std::wstring path = documents[id].filename();
				this->onFileChanged.fire(id, path);
			}
			);

			documents[id].fileWatchToken(token);

			cb(e);
		});
	});
}

void EditorModel::reloadFile(const std::wstring& id, std::function<void(std::string)> cb)
{
	if (!documents.exists(id))
	{
		return;
	}

	auto& doc = documents[id];
	if (doc.type() != DOC_TXT)
	{
		return;
	}

	EditorDocument& document = dynamic_cast<EditorDocument&>(doc);


	std::wstring path = doc.filename();
	bool readOnly = document.textFile.readonly;
	long enc = document.textFile.fileEncoding.code_page;
	openFile(path, readOnly, enc, [this, id, cb](TextFile textFile)
	{
		if (!documents.exists(id))
		{
			return;
		}

		cb(textFile.utf8);
	});
}
