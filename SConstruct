#!/usr/bin/env python

from SCons.Script import *
from builders import wrenfile_builder, wrenfile_header_builder

OUTPUT_DIR = "#dist"
WRENFILES_DIR = "#wrenfiles"
GEN_DIR = "#gen"
GEN_WRENFILES_DIR = f"{GEN_DIR}/wrenfiles"
SRC_DIR = "#src"
INC_DIR = "#include"


if (
    sys.platform.startswith("linux")
    or sys.platform.startswith("dragonfly")
    or sys.platform.startswith("freebsd")
    or sys.platform.startswith("netbsd")
    or sys.platform.startswith("openbsd")
):
    platform = "unix"
elif sys.platform == "win32":
    platform = "windows"
else:
    print("Unsupported platform.")
    exit(1)
    

## Configure Options -->
env = Environment(CXXFLAGS="-std=c++17 ")
opts = Variables()
opts.Add(EnumVariable("mode", "release or debug mode", "debug", ("release", "debug"), ignorecase=2))
opts.Add(BoolVariable("compiledb", "Generate `compile_commands.json` for external editor", False))
opts.Add(BoolVariable("builtin_wren", "Use builtin wren", True))
opts.Add(BoolVariable("builtin_fmt", "Use builtin fmt", True))
opts.Add(BoolVariable("builtin_cpppath", "Use builtin cpppath", True))
opts.Add(BoolVariable("builtin_cjson", "Use builtin cjson", True))
opts.Add(BoolVariable("builtin_miniz", "Use builtin miniz", True))
opts.Add(BoolVariable("builtin_whereami", "Use builtin whereami", True))
opts.Add(BoolVariable("builtin_imgui", "Use builtin imgui", True))
opts.Add(BoolVariable("builtin_glfw", "Use builtin glfw", True))
opts.Update(env)
Help(opts.GenerateHelpText(env))

unknowns = opts.UnknownVariables()
if unknowns:
    print("WARNING: Unknown variables are ignored: ")
    for key, value in unknowns.items():
        print(f"\t{key} = {value}")
## Configure Options <--

## Configure Environment -->
# No switch case? :(
if env["mode"] == "debug":
    env.Append(
        CXXFLAGS = "-g -O0 "
        )
elif env["mode"] == "release":
    env.Append(
        CXXFLAGS = "-O2 "
    )

if env["compiledb"]:
    from SCons import __version__ as scons_raw_version
    scons_ver = env._get_major_minor_revision(scons_raw_version)
    if scons_ver >= (4, 0, 0):
        env.Tool("compilation_db")
        env.Alias("compiledb", env.CompilationDatabase())
## Configure Environment <--

## Builtin Libs -->
builtin_libs = {}
system_libs = []
if env["builtin_wren"]:
    env_wren = env.Clone()
    env_wren.Append(
        CPPPATH = [ 
            Dir("#thirdparty/wren"),
            Dir("#thirdparty/wren/vm"), 
            Dir("#thirdparty/wren/optional")
        ],
    )

    builtin_libs["wren"] = (env_wren.StaticLibrary(
        File("#thirdparty/wren/libwren.a"), 
        Glob("#thirdparty/wren/vm/*.c") + Glob("#thirdparty/wren/optional/*.c")
    ))
    env.Append(
        CPPPATH = [Dir("#thirdparty/wren")],
    )
else:
    system_libs.append("wren")


if env["builtin_fmt"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/fmt")],
    )

    builtin_libs["fmt"] = (env.StaticLibrary(
        File("#thirdparty/fmt/libfmt.a"),
        Glob("#thirdparty/fmt/src/*.cc")
    ))
else:
    system_libs.append("fmt")


if env["builtin_cpppath"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/cpppath")]
    )


if env["builtin_cjson"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/cjson")],
    )

    builtin_libs["cjson"] = (env.StaticLibrary(
        File("#thirdparty/cjson/libcjson.a"),
        File("#thirdparty/cjson/cjson/cJSON.c")
    ))
else:
    system_libs.append("cjson")


if env["builtin_miniz"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/miniz")],
    )

    builtin_libs["miniz"] = (env.StaticLibrary(
        File("#thirdparty/miniz/libminiz.a"),
        File("#thirdparty/miniz/miniz.c")
    ))
else:
    system_libs.append("miniz")


if env["builtin_whereami"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/whereami")]
    )

    builtin_libs["whereami"] = (env.StaticLibrary(
        File("#thirdparty/whereami/libwhereami.a"),
        File("#thirdparty/whereami/whereami.c")
    ))
else:
    system_libs.append("whereami")


if env["builtin_imgui"]:
    env.Append(
        CPPPATH = [Dir("#thirdparty/imgui")]
    )
    IMGUI_BACKEND_DIR = "#thirdparty/imgui/backends"
    imgui_sources = Glob("#thirdparty/imgui/*.cpp") + [ #using glfw / opengl
        File(f"{IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp"),
        File(f"{IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp"),
    ]

    builtin_libs["imgui"] = (env.StaticLibrary(
        File("#thirdparty/imgui/libimgui.a"),
        imgui_sources
    ))

    env.Append(
        CPPPATH = [Dir(IMGUI_BACKEND_DIR)]
    )
else:
    system_libs.append("imgui")


if env["builtin_glfw"]: 
    # Reference: https://www.glfw.org/docs/3.3/compile.html#:~:text=If%20you%20wish%20to%20compile,specific%20ones%20for%20various%20features.
    glfw_env = env.Clone()
    
    GLFW_SRC_DIR = "#thirdparty/glfw/src"
    glfw_defines = []
    glfw_sources = [
        File(f"{GLFW_SRC_DIR}/context.c"),
        File(f"{GLFW_SRC_DIR}/init.c"),
        File(f"{GLFW_SRC_DIR}/input.c"),
        File(f"{GLFW_SRC_DIR}/monitor.c"),
        File(f"{GLFW_SRC_DIR}/vulkan.c"),
        File(f"{GLFW_SRC_DIR}/window.c"),
    ]

    if platform == "windows":
        glfw_defines.append("_GLFW_WIN32=1")
        glfw_sources += [
            File(f"{GLFW_SRC_DIR}/win32_init.c"),
            File(f"{GLFW_SRC_DIR}/win32_joystick.c"),
            File(f"{GLFW_SRC_DIR}/win32_monitor.c"),
            File(f"{GLFW_SRC_DIR}/win32_time.c"),
            File(f"{GLFW_SRC_DIR}/win32_thread.c"),
            File(f"{GLFW_SRC_DIR}/win32_window.c"),
            File(f"{GLFW_SRC_DIR}/wgl_context.c"),
            File(f"{GLFW_SRC_DIR}/egl_context.c"),
            File(f"{GLFW_SRC_DIR}/osmesa_context.c"),
        ]
    elif platform=="unix":
        glfw_defines.append("_GLFW_X11=1")
        glfw_sources += [
            File(f"{GLFW_SRC_DIR}/x11_init.c"),
            File(f"{GLFW_SRC_DIR}/x11_monitor.c"),
            File(f"{GLFW_SRC_DIR}/x11_window.c"),
            File(f"{GLFW_SRC_DIR}/xkb_unicode.c"),
            File(f"{GLFW_SRC_DIR}/posix_time.c"),
            File(f"{GLFW_SRC_DIR}/posix_thread.c"),
            File(f"{GLFW_SRC_DIR}/linux_joystick.c"),
            File(f"{GLFW_SRC_DIR}/glx_context.c"),
            File(f"{GLFW_SRC_DIR}/egl_context.c"),
            File(f"{GLFW_SRC_DIR}/osmesa_context.c"),
        ]

    glfw_env.Append(
        CPPPATH = [Dir("#thirdparty/glfw/GLFW"), Dir("#thirdparty/glfw/src")],
        CPPDEFINES = glfw_defines,
    )

    builtin_libs["glfw"] = (glfw_env.StaticLibrary(
        File("#thirdparty/glfw/libglfw.a"),
        glfw_sources
    ))

    env.Append(
        CPPPATH = [Dir("#thirdparty/glfw/include")]
    )
else:
    system_libs.append("glfw")
if platform == "unix":
    system_libs += (["dl", "X11", "GL", "pthread"])


if env["builtin_glfw"] and env["builtin_imgui"]:
    env.Depends(builtin_libs["imgui"], builtin_libs["glfw"])
## Builtin Libs <--


env.Append(
    CPPPATH = [Dir(INC_DIR), Dir(GEN_DIR)],
    LIBS=list(builtin_libs.values()) + system_libs, 
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


env.Depends(wrechie, builtin_libs.values())
env.Depends(wrechie, wrenfile_header)
env.Depends(wrenfile_header, wrenfiles)
