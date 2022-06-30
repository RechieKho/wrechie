from SCons.Script import *

def build_wrenfiles(target, source, env):
    def wren_to_c_string(filename, wren_source_lines):
        PREAMBLE = """
#ifndef __{0}__//-->
#define __{0}__
// Generated automatically. Do not edit.
static const char* {1}_wrenfile=
{2};
#endif//__{0}__ <--
"""
        wren_source = ""
        for line in wren_source_lines:
            if line.strip(" ") == "\n": continue
            line = line.rstrip()
            line = line.replace("\\", "\\\\")
            line = line.replace('"', "\\\"")
            if wren_source: wren_source += "\n"
            wren_source += '"' + line + '\\n"'

        return PREAMBLE.format(filename.upper(), filename, wren_source)

    if len(target) != len(source):
        raise Exception("number of target is not the same as number of source")
    correspond = tuple(zip(target, source))

    for t, s in correspond:
        source_path = s.abspath
        target_path = t.abspath
        with open(source_path, "r") as source_file:
            with open(target_path, "w") as target_file:
                target_file.write(wren_to_c_string(
                    os.path.splitext(
                        os.path.basename(source_path)
                    )[0],
                    source_file.readlines()
                ))


def build_wrefile_header(target, source, env):
    """
    @param
        target - the output header
        source - list of wrenfile
    """

    target_path = target[0].abspath
    source_paths = [file.abspath for file in source]

    macro_statments = []
    for source_path in source_paths:
        wrenfile_name = os.path.basename(source_path).split(os.extsep)[0]
        macro_statments.append(f"map[\"{wrenfile_name}\"] = {wrenfile_name}_wrenfile; \\")

    includes = [f"#include \"{os.path.relpath(source_path, os.path.dirname(target_path))}\"" for source_path in source_paths]
    header_guard= os.path.basename(target_path).replace(".", "_").upper()

    with open(target_path, "w") as file:
        file.write("""#ifndef __{0}__//-->
#define __{0}__
// Generated automatically. Do not edit.

{1}

#define LOAD_WRENFILES(map) {{ \\
{2}
}}

#endif//__{0}__ <--
""".format(header_guard, "\n".join(includes), "\n".join(macro_statments))
        )


wrenfile_builder= Builder(
    action=build_wrenfiles
)

wrenfile_header_builder = Builder(
    action=build_wrefile_header
)