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


std::wstring ImageDocument::id()
{
	return id_;
}

std::wstring ImageDocument::filename()
{
	return filename_;
}

std::wstring ImageDocument::fileWatchToken()
{
	return fileWatchToken_;
}

void ImageDocument::fileWatchToken(std::wstring t)
{
	fileWatchToken_ = t;
}


DOC_TYPE ImageDocument::type()
{
	return DOC_IMG;
}


std::wstring HtmlDocument::id()
{
	return id_;
}

std::wstring HtmlDocument::filename()
{
	return filename_;
}

std::wstring HtmlDocument::fileWatchToken()
{
	return fileWatchToken_;
}

void HtmlDocument::fileWatchToken(std::wstring t)
{
	fileWatchToken_ = t;
}


DOC_TYPE HtmlDocument::type()
{
	return DOC_HTML;
}

