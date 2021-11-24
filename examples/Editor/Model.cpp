#include "Model.h"



void EditorModel::removeDocument(const std::wstring& id, const std::wstring& active)
{
	if (!documents.exists(id)) return;

	activeDocument_ = active;

	fileService_.monitor.unwatch(documents[id].fileWatchToken, documents[id].textFile.filename);

	documents.erase(id);
}

EditorDocument EditorModel::transferDocument(const std::wstring& from)
{
	return rotService_.transferTab(instanceId(), from);
	/*EditorDocument result;

	auto info = mtl::split(from, ':');
	if (info.size() != 2)
	{
		return result;
	}

	std::wstring instanceId = info[0];
	std::wstring documentId = info[1];

	mtl::punk<IMTLEditor> remoteEditor = mtl::rot::object<IMTLEditor>(__uuidof(MTLEditor), instanceId);
	if (!remoteEditor) return result;

	mtl::punk<IMTLEditorDocuments> docs;
	HRESULT hr = remoteEditor->get_documents(&docs);
	if (!docs) return result;

	mtl::punk<IMTLEditorDocument> doc;
	mtl::variant vId{ mtl::ole_char(from.c_str()) };
	docs->item(vId, &doc);
	if (!doc) return result;

	mtl::bstr fn;
	doc->get_filename(&fn);
	mtl::bstr content;
	doc->get_content(&content);

	std::wostringstream woss;
	woss << this->instanceId << L":" << documentId;

	std::wstring newId = woss.str();

	std::string utf8 = content.to_string();
	TextFile textFile;
	textFile.fileEncoding.code_page = CP_UTF8;
	textFile.fileEncoding.eol = mtl::file_encoding::UNIX;
	textFile.fileEncoding.has_bom = false;
	textFile.fileEncoding.is_binary = false;
	textFile.filename = fn.str();
	textFile.last_written = 0;
	textFile.readonly = false;
	textFile.size = utf8.size();
	textFile.utf8 = utf8;

	result.id = newId;
	result.textFile = std::move(textFile);

	docs->remove(vId);
	*/
	//return result;
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

void EditorModel::insertDocument(const std::wstring& id, TextFile& textFile)
{
	activeDocument_ = id;
	documents.insert( id, new EditorDocument(id, textFile) );

	std::wstring token = fileService_.monitor.watch(
		textFile.filename,
		[this, id]()
	{
		if (!documents.exists(id)) return;

		std::wstring path = documents[id].textFile.filename;
		this->onFileChanged.fire(id, path);
	}
	);

	documents[id].fileWatchToken = token;

	/*
	auto scintilla = view_.createEditorWnd(id, path, textFile.utf8);


//	scintilla->set_text(textFile.utf8);

	scintilla->onNotify(SCN_MODIFIED, [this, id](NMHDR* nmhdr)
	{
		SCNotification* notify = (SCNotification*)nmhdr;
		if (notify->nmhdr.code == SCN_MODIFIED)
		{
			updateStatus(id);
		}
	});

	scintilla->onNotify(SCN_UPDATEUI, [this, id](NMHDR* nmhdr)
	{
		updateStatus(id);
	});

	updateStatus(id);


	HICON icon = mtl::shell::file_icon(documents[id]->textFile.filename);
	view_.mainWnd.set_icon(icon);

	std::wstring title = mtl::path(path).filename();
	view_.tabControl.add({ title, path, id }, scintilla->handle);
	*/
}

IO_ERROR EditorModel::openFile(const std::wstring& path, bool readOnly, long enc, std::function<void(EditorDocument)> cb)
{
	IO_ERROR ie = fileService_.read(path, enc, readOnly, [this, cb](IO_ERROR e, TextFile textFile)
	{
		if (e == IO_ERROR_SUCCESS)
		{
			std::wostringstream woss;
			woss << instanceId() << ":" << mtl::new_guid();
			std::wstring id = woss.str();

			EditorDocument result;
			result.id = id;
			result.textFile = std::move(textFile);

			cb(result);
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

	EditorDocument doc;
	doc.id = id;
	doc.textFile = std::move(textFile);
	return doc;
}

void EditorModel::saveDocument(std::wstring id, std::wstring path, int enc, EOL_TYPE eol, std::string utf8, std::function<void(IO_ERROR)> cb)
{
	if (!documents.exists(id))
	{
		cb(IO_ERROR_NOT_FOUND);
		return;
	}

	auto doc = documents[id];
	TextFile textFile = doc.textFile;

	textFile.utf8 = utf8;
	textFile.size = utf8.size();
	textFile.filename = path;
	textFile.last_written = time(0);

	if (enc != -1)
	{
		textFile.fileEncoding.code_page = enc;
	}

	textFile.fileEncoding.eol = (mtl::file_encoding::eol_mode)eol;

	fileService_.monitor.unwatch(doc.fileWatchToken, path);

	fileService_.write(textFile, [this, id, textFile, cb](IO_ERROR e)
	{
		mtl::timer::set_timeout(500, [this, id, textFile, cb, e](UINT_PTR)
		{
			std::wstring token = fileService_.monitor.watch(
				textFile.filename,
				[this, id]()
			{
				if (documents.exists(id) == 0) return;

				std::wstring path = documents[id].textFile.filename;
				this->onFileChanged.fire(id, path);
			}
			);

			documents[id].fileWatchToken = token;

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

	std::wstring path = documents[id].textFile.filename;
	bool readOnly = documents[id].textFile.readonly;
	long enc = documents[id].textFile.fileEncoding.code_page;
	openFile(path, readOnly, enc, [this, id, cb](EditorDocument doc)
	{
		if (!documents.exists(id))
		{
			return;
		}

		cb(doc.textFile.utf8);
	});
}
