#include "mtl/ole/img.h"

static void load_resource_ids()
{
    mtl::gui().add({
        { 101, "IDS_APP_TITLE" },
        { 128, "IDR_MAINFRAME" },
        { 102, "IDD_HELLORIBBON_DIALOG" },
        { 103, "IDD_ABOUTBOX" },
        { 104, "IDM_ABOUT" },
        { 105, "IDM_EXIT" },
        { 107, "IDI_HELLORIBBON" },
        { 108, "IDI_SMALL" },
        { 109, "IDC_HELLORIBBON" },
        { 110, "IDC_BUTT" },
        { 111, "IDC_CONTENT" },
        { 500, "IDI_JPEG" },
        { 2, "IDC_MYICON" },
        { -1, "IDC_STATIC" }
    });
}


static int unused = [](){ load_resource_ids(); return 1; }();
