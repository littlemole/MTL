var arg = WScript.arguments(0);

var ForReading = 1;
var ForWriting = 2;
var ForAppending = 8;

var trim = function(s)
{
    return s.replace(/^\s+|\s+$/g, '');
};

var lines = [];

var fs = new ActiveXObject("Scripting.FileSystemObject");
var f = fs.OpenTextFile(arg, ForReading, -1);
while(!f.AtEndOfStream)
{
    var l = f.ReadLine();
    lines.push(l);
}
f.Close();

var output = "#include \"mtl/ole/img.h\"\r\n\r\n";
output += "static void load_resource_ids()\r\n";
output += "{\r\n";
output += "    mtl::gui().add({\r\n";

var ids_seen = {};
var items = [];
for( i = 0; i < lines.length; i++)
{
    var line = trim(lines[i]);
    if(line.substr(0,7) != "#define") continue;
    var cols = line.split(/\s/);
    if(cols.length < 3) continue;
    var idstr = cols[1];
    var id = cols[2];

    if (idstr.substr(0, 5) == "_APS_")
    {
        continue;
    }

    if (id in ids_seen)
    {
        WScript.Echo("ERROR duplicate id " + id + " [ "+ idstr +" ]");
        WScript.Echo("previously seen as " + ids_seen[id]);
        WScript.Quit(1);
    }
    ids_seen[id] = idstr;
    var item = "        { " + id + ", \"" + idstr + "\" }";
    items.push(item);
    //WScript.Echo(id + " : " + idstr);
}
output += items.join(",\r\n");
output += "\r\n    });\r\n}\r\n\r\n";

WScript.Echo(output);

WScript.Echo("static int unused = [](){ load_resource_ids(); return 1; }();");

//var WshShell = WScript.CreateObject ("WScript.Shell");
//var cwd = WshShell.CurrentDirectory
//WScript.Echo(cwd);