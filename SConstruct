#!/usr/bin/env python

from SCons.Script import *
from builders import wrenfile_builder, wrenfile_header_builder

OUTPUT_DIR = "#dist"
WRENFILES_DIR = "#wrenfiles"
GEN_DIR = "#gen"
GEN_WRENFILES_DIR = f"{GEN_DIR}/wrenfiles"
SRC_DIR = "#src"
INC_DIR = "#include"

if not (
    # unix
    sys.platform.startswith("linux")
    or sys.platform.startswith("dragonfly")
    or sys.platform.startswith("freebsd")
    or sys.platform.startswith("netbsd")
    or sys.platform.startswith("openbsd")

    # win
    or sys.platform == "win32"
):
    print("Your system is not supported.")
    exit(1)
    

## Configure Options -->
env = Environment()
opts = Variables()
opts.Add(EnumVariable("mode", "release or debug mode", "debug", ("release", "debug"), ignorecase=2))
opts.Add(BoolVariable("compiledb", "Generate `compile_commands.json` for external editor", False))
opts.Add(BoolVariable("builtin_wren", "Use builtin wren", True))
opts.Add(BoolVariable("builtin_fmt", "Use builtin fmt", True))
opts.Add(BoolVariable("builtin_cxxopts", "Use builtin cxxopts", True))
opts.Add(BoolVariable("builtin_cxxopts", "Use builtin cxxopts", True))
opts.Add(BoolVariable("builtin_cpppath", "Use builtin cpppath", True))
opts.Add(BoolVariable("builtin_cjson", "Use builtin cjson", True))
opts.Add(BoolVariable("builtin_miniz", "Use builtin miniz", True))
opts.Add(BoolVariable("builtin_elfio", "Use builtin elfio (only for unix)", True))
opts.Add(BoolVariable("builtin_whereami", "Use builtin whereami", True))
opts.Update(env)
Help(opts.GenerateHelpText(env))

unknowns = opts.UnknownVariables()
if unknowns:
    print("WARNING: Unknown variables are ignored: " + unknowns)
## Configure Options <--

## Configure Environment -->
# No switch case? :(
if env["mode"] == "debug":
    env.Append(
        CXXFLAGS = "-g -O0"
        )
elif env["mode"] == "release":
    env.Append(
        CXXFLAGS = "-02"
    )

if env["compiledb"]:
    from SCons import __version__ as scons_raw_version
    scons_ver = env._get_major_minor_revision(scons_raw_version)
    if scons_ver >= (4, 0, 0):
        env.Tool("compilation_db")
        env.Alias("compiledb", env.CompilationDatabase())
## Configure Environment <--

## Builtin Libs -->
builtin_libs = []
if env["builtin_wren"]:
    env_wren = env.Clone()
    env_wren.Append(
        CPPPATH = [ 
            Dir("#thirdparty/wren"),
            Dir("#thirdparty/wren/vm"), 
            Dir("#thirdparty/wren/optional")
        ],
    )

    builtin_libs.append(env_wren.StaticLibrary(
        File("#thirdparty/wren/libwren.a"), 
        Glob("#thirdparty/wren/vm/*.c") + Glob("#thirdparty/wren/optional/*.c")
    ))
    env.Append(
        CPPPATH = [Dir("#thirdparty/wren")],
    )
else:
    env.Append(LIBS=["wren"])


if env["builtin_fmt"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/fmt")],
    )

    builtin_libs.append(env.StaticLibrary(
        File("#thirdparty/fmt/libfmt.a"),
        Glob("#thirdparty/fmt/src/*.cc")
    ))
else:
    env.Append(LIBS=["fmt"])


if env["builtin_cxxopts"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/cxxopts")]
    )


if env["builtin_cpppath"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/cpppath")]
    )

if env["builtin_cjson"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/cjson")],
    )

    builtin_libs.append(env.StaticLibrary(
        File("#thirdparty/cjson/libcjson.a"),
        File("#thirdparty/cjson/cjson/cJSON.c")
    ))
else:
    env.Append(LIBS=["cjson"])

if env["builtin_miniz"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/miniz")],
    )

    builtin_libs.append(env.StaticLibrary(
        File("#thirdparty/miniz/libminiz.a"),
        File("#thirdparty/miniz/miniz.c")
    ))
else:
    env.Append(LIBS=["miniz"])

if env["builtin_elfio"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/elfio")]
    )

if env["builtin_whereami"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/whereami")]
    )

    builtin_libs.append(env.StaticLibrary(
        File("#thirdparty/whereami/libwhereami.a"),
        File("#thirdparty/whereami/whereami.c")
    ))
else:
    env.Append(LIBS=["whereami"])
## Builtin Libs <--
    


env.Append(
    CPPPATH = [Dir(INC_DIR), Dir(GEN_DIR)],
    LIBS=builtin_libs,
    BUILDERS = {
        "WrenFile": wrenfile_builder,
        "WrenFileHeader": wrenfile_header_builder
    }
)

wrenfile_source = []
for root, dirnames, filenames in os.walk(Dir(WRENFILES_DIR).abspath):
    wrenfile_source += Glob(os.path.join(root, "*.wren"))
wrenfile_target = [File(f"{GEN_WRENFILES_DIR}/{os.path.basename(file.path)}.inc.hpp") for file in wrenfile_source]

wrenfiles = env.WrenFile(wrenfile_target, wrenfile_source)
wrenfile_header = env.WrenFileHeader(
    File(f"{GEN_WRENFILES_DIR}/wrenfiles.hpp"),
    wrenfiles
)


wrechie = env.Program(File(f"{OUTPUT_DIR}/wrechie"), 
    [Glob(os.path.join(root, "*.cpp")) for root, dirnames, filenames in os.walk(Dir(SRC_DIR).abspath)]
)

env.Depends(wrechie, builtin_libs)
env.Depends(wrechie, wrenfile_header)
env.Depends(wrenfile_header, wrenfiles)
