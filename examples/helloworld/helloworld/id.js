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

var output = "#include \"mtl/win/img.h\"\r\n\r\n";
output += "void load_resource_ids()\r\n";
output += "{\r\n";
output += "    mtl::gui().add({\r\n";

var items = [];
for( i = 0; i < lines.length; i++)
{
    var line = trim(lines[i]);
    if(line.substr(0,7) != "#define") continue;
    var cols = line.split(/\s/);
    if(cols.length < 3) continue;
    var idstr = cols[1];
    var id = cols[2];
    var item = "        { " + id + ", \"" + idstr + "\" }";
    items.push(item);
    //WScript.Echo(id + " : " + idstr);
}
output += items.join(",\r\n");
output += "\r\n    });\r\n}\r\n\r\n";

WScript.Echo(output);

var WshShell = WScript.CreateObject ("WScript.Shell");
var cwd = WshShell.CurrentDirectory
WScript.Echo(cwd);