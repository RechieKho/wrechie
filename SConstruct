#!/usr/bin/env python

from SCons.Script import *

OUTPUT_DIR = "#dist"

## Configure Options -->
env = Environment()
opts = Variables()
opts.Add(BoolVariable("compiledb", "Generate `compile_commands.json` for external editor", False))
opts.Add(BoolVariable("builtin_wren", "Use builtin wren", True))
opts.Add(BoolVariable("builtin_fmt", "Use builtin fmt", True))
opts.Add(BoolVariable("builtin_cxxopts", "Use builtin cxxopts", True))
opts.Add(BoolVariable("builtin_cxxopts", "Use builtin cxxopts", True))
opts.Add(BoolVariable("builtin_cpppath", "Use builtin cpppath", True))
opts.Add(BoolVariable("builtin_dylib", "Use builtin dylib", True))
opts.Update(env)
Help(opts.GenerateHelpText(env))

unknowns = opts.UnknownVariables()
if unknowns:
    print("WARNING: Unknown variables are ignored: " + unknowns)
## Configure Options <--



## Configure Environment -->
if env["compiledb"]:
    from SCons import __version__ as scons_raw_version
    scons_ver = env._get_major_minor_revision(scons_raw_version)
    if scons_ver >= (4, 0, 0):
        env.Tool("compilation_db")
        env.Alias("compiledb", env.CompilationDatabase())


if env["builtin_wren"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/wren")],
        LIBS = [File("#thirdparty/wren/linux/64bit/libwren.a")]
    )
else:
    env.Append(LIBS=["wren"])


if env["builtin_fmt"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/fmt")],
        LIBS = [File("#thirdparty/fmt/linux/libfmt.a")]
    )
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

if env["builtin_dylib"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/dylib")],
        LIBS = ["dl"]
    )
## Configure Environment <--
    


env.Append(CPPPATH = Dir("#include"))
env.Program(File(f"{OUTPUT_DIR}/wrench"), 
    [Glob(os.path.join(root, "*.cpp")) for root, dirnames, filenames in os.walk('src')]
)