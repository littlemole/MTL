#include "mvc.h"


std::wstring EditorDocument::id()
{
	return id_;
}

std::wstring EditorDocument::filename()
{
	return textFile.filename;
}

std::wstring EditorDocument::fileWatchToken()
{
	return fileWatchToken_;
}

void EditorDocument::fileWatchToken(std::wstring t)
{
	fileWatchToken_ = t;
}


DOC_TYPE EditorDocument::type()
{
	return DOC_TXT;
}

