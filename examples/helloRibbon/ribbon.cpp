#include "framework.h"
#include "mtl/ole/img.h"
#include "ribbon.h"

static void add_ribbon()
{
	mtl::gui().add_ribbon(ID_CMD_EXIT, L"ID_CMD_EXIT", ID_CMD_EXIT_LABEL, ID_CMD_EXIT_IMG);
	mtl::gui().add_ribbon(ID_CMD_NEW, L"ID_CMD_NEW", ID_CMD_NEW_LabelTitle_RESID, ID_CMD_NEW_SmallImages_96__RESID);
	mtl::gui().add_ribbon(ID_CMD_OPEN, L"ID_CMD_OPEN", ID_CMD_OPEN_LabelTitle_RESID, ID_CMD_OPEN_SmallImages_96__RESID);
	mtl::gui().add_ribbon(ID_CMD_SPLIT, L"ID_CMD_SPLIT", ID_CMD_SPLIT_LabelTitle_RESID, ID_CMD_SPLIT_SmallImages_96__RESID);
	mtl::gui().add_ribbon(ID_CMD_ZOOM, L"ID_CMD_ZOOM", ID_CMD_ZOOM_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_SYNTAX, L"ID_CMD_SYNTAX", ID_CMD_SYNTAX_LabelTitle_RESID, ID_CMD_SYNTAX_LargeImages_RESID);
	mtl::gui().add_ribbon(ID_CMD_FIRST, L"ID_CMD_FIRST", ID_CMD_FIRST_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_SECOND, L"ID_CMD_SECOND", ID_CMD_SECOND_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_THIRD, L"ID_CMD_THIRD", ID_CMD_THIRD_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_FOURTH, L"ID_CMD_FOURTH", ID_CMD_FOURTH_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_PLAIN, L"ID_CMD_PLAIN", ID_CMD_PLAIN_LabelTitle_RESID, ID_CMD_PLAIN_LargeImages_96__RESID);
	mtl::gui().add_ribbon(ID_CMD_HTML, L"ID_CMD_HTML", ID_CMD_HTML_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_JAVASCRIPT, L"ID_CMD_JAVASCRIPT", ID_CMD_JAVASCRIPT_LabelTitle_RESID, 0);
	mtl::gui().add_ribbon(ID_CMD_BOM, L"ID_CMD_BOM", ID_CMD_BOM_LabelTitle_RESID, 0);
}

static int unused = []() {

	add_ribbon();
	return 1;
}();