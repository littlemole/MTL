var arg = WScript.arguments(0);

var ForReading = 1;
var ForWriting = 2;
var ForAppending = 8;

var trim = function (s) {
    return s.replace(/^\s+|\s+$/g, '');
};

var lines = [];

var dom = new ActiveXObject("msxml2.DOMDocument.6.0");
dom.async = false;
dom.resolveExternals = false;
dom.load(arg);

dom.setProperty("SelectionNamespaces", "xmlns:rib='http://schemas.microsoft.com/windows/2009/Ribbon'");
dom.setProperty("SelectionLanguage", "XPath");

objNodeList = dom.documentElement.selectNodes("//rib:Application/rib:Application.Commands/rib:Command");


function extract_child(obj,cmd, key) {

    var n = obj.selectSingleNode("rib:Command." + key + "/rib:String");
    if (n) {
        cmd[key]["Content"] = n.getAttribute("Content");
        cmd[key]["Id"] = n.getAttribute("Id");
        cmd[key]["Symbol"] = n.getAttribute("Symbol");
    }

    if (!cmd[key].Symbol) {
        cmd[key].Symbol = cmd.Symbol + "_" + key + "_RESID";
    }

    return cmd;
}


function extract_img(obj, cmd, key) {

    var n = obj.selectSingleNode("rib:Command." + key + "/rib:Image[1]");
    if (n) {
        cmd["Image"]["Source"] = n.getAttribute("Source");
        cmd["Image"]["Id"] = n.getAttribute("Id");
        cmd["Image"]["Symbol"] = n.getAttribute("Symbol");

        if (!cmd.Image.Id) cmd.Image.Id = "1";

        var dpi = n.getAttribute("MinDPI");

        if (!cmd.Image.Symbol) {
            cmd.Image.Symbol = cmd.Symbol + "_" + key;
            if (dpi) {
                cmd.Image.Symbol += "_" + dpi + "_";
            }
            cmd.Image.Symbol += "_RESID";
        }

    }
    if (!cmd.Image.Symbol) {
        cmd.Image.Symbol = 0;
    }
    return cmd;
}

function print_cmd(cmd) {

    var str = "mtl::gui().add_ribbon( " + cmd.Symbol + ', L"' + cmd.Symbol + '", ' + cmd.LabelTitle.Symbol + ", " + cmd.Image.Symbol + ");";
    WScript.Echo(str);
}

for (i = 0; i < objNodeList.length; i++) {

    obj = objNodeList[i];

    var cmd = {
        "Id"     : obj.getAttribute("Id"),
        "Symbol": obj.getAttribute("Symbol"),
        "LabelTitle": {
            "Content": obj.getAttribute("LabelTitle"),
            "Id": "",
            "Symbol": ""
        },
        "TooltipTitle": {
            "Content": obj.getAttribute("TooltipTitle"),
            "Id": "",
            "Symbol": ""
        },
        "Image": {}
    };

    cmd = extract_child(obj, cmd, "LabelTitle");
    cmd = extract_child(obj, cmd, "TooltipTitle");
    cmd = extract_img(obj, cmd, "LargeImages");
    if (!cmd["Image"].Id) {
        cmd = extract_img(obj, cmd, "SmallImages");
    }

    
    print_cmd(cmd);
}

//WScript.Echo(dom.xml);
