import re
from sys import argv
import os

expr = re.compile(R'DB3D_GFX_API (.+?) (d3_\w+)\((.+)\);')

if len(argv) < 4:
    print(f"Usage: {argv[0]} <input header> <output header> <output source>")
    exit(-1)

with open(argv[1]) as f:
    input_header = f.read()

in_defs = [i.groups() for i in expr.finditer(input_header)]


def generate_header():
    output_header = """// Generated using {generator} on {date}
#ifndef DB3D_GFX_LOADER_H
#define DB3D_GFX_LOADER_H
#include <daybreak/gfx/db3d_gfx.h>
#include <daybreak/hedley.h>
HEDLEY_BEGIN_C_DECLS
{defs}
HEDLEY_NON_NULL(1)
bool d3_loader_load(const char* plugin_path);
HEDLEY_END_C_DECLS
#endif"""

    defs = ""

    for match in in_defs:
        defs += """typedef {0} (*pfn_{1})({2});
extern pfn_{1} db3d_loader_{1};
#define {1} db3d_loader_{1}
""".format(*match)

    return output_header.format(generator=" ".join(argv), date="now", defs=defs)


def generate_source():
    output_source = """// Generated using {generator} on {date}
#include <daybreak/gfx_loader/loader.h>
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
{extern_defs}
static HINSTANCE hDLL = NULL;
bool d3_loader_load(const char* plugin_path) {{
hDLL = LoadLibraryA(plugin_path);
if (!hDLL) return false;
bool ret = true;
{method_loads}
if (!ret) return false;
bool (*can_load)(void) = (bool (*)(void))GetProcAddress((HMODULE)hDLL, "__d3_can_load");
return can_load && can_load();
}}
"""

    extern_defs = ""
    method_loads = ""

    for match in in_defs:
        extern_defs += "pfn_{1} db3d_loader_{1} = 0;".format(*match)
        method_loads += """db3d_loader_{1} = (pfn_{1})GetProcAddress((HMODULE)hDLL, "{1}");
if (!db3d_loader_{1}) {{
printf("Plugin %s missing method {1}\\n", plugin_path);
ret = false;
}}""".format(*match)

    return output_source.format(generator=" ".join(argv), date="now", extern_defs=extern_defs, method_loads=method_loads)


os.makedirs(os.path.dirname(argv[2]), exist_ok=True)
with open(argv[2], "w+") as f:
    f.write(generate_header())

os.makedirs(os.path.dirname(argv[3]), exist_ok=True)
with open(argv[3], "w+") as f:
    f.write(generate_source())

