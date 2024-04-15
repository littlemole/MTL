#pragma once

#include <windows.h>

extern "C" {

#define WM_EDGE_NAVIGATE	WM_APP+1
#define WM_EDGE_JSON_MSG	WM_APP+2
#define WM_EDGE_JAVASCRIPT	WM_APP+3

#define NM_NAVIGATE_START	1
#define NM_NAVIGATE_SUCCESS 2
#define NM_NAVIGATE_ERROR	3
#define NM_JSON_MSG			4

#define WM_STYLE_EX_NO_CTX_MENU 2

extern const wchar_t* EDGE_WC;

void registerEdgeClass();


struct NM_NAVIGATE
{
	NMHDR hdr;
	const wchar_t* uri;
};


struct NM_JSON
{
	NMHDR hdr;
	const wchar_t* json;
};


}
