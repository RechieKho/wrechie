#include <fmt/core.h>
#include <miniz.h>
#include <whereami.h>

#include <cpppath.hpp>
#include <wren.hpp>

#include "exception.hpp"
#include "fs/fsutils.hpp"
#include "fs/zip.hpp"
#include "log.hpp"
#include "modules/modules.hpp"
#include "project.hpp"
#include "runtime.hpp"
#include "typedef.hpp"

int main(int argc, const char* argv[]) {
  int wrechie_path_length = wai_getExecutablePath(NULL, 0, NULL);
  char wrechie_path[wrechie_path_length + 1];
  wai_getExecutablePath(wrechie_path, wrechie_path_length, NULL);
  wrechie_path[wrechie_path_length] = '\0';

  std::uint64_t project_size;
  std::uint64_t whole_file_size;

  if (!get_project_size_in_wrechie(wrechie_path, project_size,
                                   whole_file_size)) {
    char target_wrechie_path[MAX_PATH_LEN];
    strcpy(target_wrechie_path, wrechie_path);
    switch (argc) {
      case 3:
        try {
          COND_THROW_EXC(!GET_REAL_PATH(argv[1], target_wrechie_path),
                         FAIL_TO_RESOLVE_PATH,
                         EXC_STR_FAIL_TO_RESOLVE_PATH(argv[1]));
        } catch (const WrechieException& e) {
          fmt::print(e.second);
          return e.first;
        }
      case 2:
        char project_path[MAX_PATH_LEN];
        try {
          COND_THROW_EXC(!GET_REAL_PATH(argv[argc - 1], project_path),
                         FAIL_TO_RESOLVE_PATH,
                         EXC_STR_FAIL_TO_RESOLVE_PATH(argv[argc - 1]));
        } catch (const WrechieException& e) {
          fmt::print(e.second);
          return e.first;
        }
        bundle_project_in_dir(target_wrechie_path, project_path);
        break;
      default:
        fmt::print(
            "{}\n    {}\n",
            fmt::format("wrechie version {}, a general purpose programming "
                        "environment that runs wren.\n",
                        WRECHIE_VER),
            "usage: wrechie [WRECHIE_PATH] PROJECT_PATH\n");
        return INVALID_COMMAND_LINE_INPUT;
    }
    return OK;
  };

  try {
    ZipReader project(wrechie_path,
                      whole_file_size - project_size - sizeof(project_data),
                      project_size, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY);
    const std::string main_source = project.get_file_content("main.wren");
    WrenVM* vm;
    NEW_WREN_VM(vm);
    LOAD_MODULE;
    GET_RUNTIME_STATE(vm)->project = &project;
    WrenInterpretResult result =
        wrenInterpret(vm, "main.wren", main_source.c_str());
    FREE_WREN_VM(vm);
  } catch (const WrechieException& e) {
    fmt::print(e.second);
    return e.first;
  }
}
