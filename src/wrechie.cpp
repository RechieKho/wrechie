#include <fmt/core.h>
#include <miniz.h>
#include <whereami.h>

#include <cpppath.hpp>
#include <wren.hpp>

#include "err.hpp"
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
      case 1:
        fmt::print(
            "{}\n{}\n",
            fmt::format("wrechie version {}, a general purpose programming "
                        "environment that runs wren.\n",
                        WRECHIE_VER),
            "    usage: wrechie [WRECHIE_PATH] PROJECT_PATH\n");
        std::exit(TOO_FEW_ARGUMENTS);
        break;
      case 3:
        ERR_COND_EXIT_FAIL_TO_RESOLVE_PATH(
            !GET_REAL_PATH(argv[1], target_wrechie_path), argv[1]);
      case 2:
        char project_path[MAX_PATH_LEN];
        ERR_COND_EXIT_FAIL_TO_RESOLVE_PATH(
            !GET_REAL_PATH(argv[argc - 1], project_path), argv[argc - 1]);
        bundle_project_in_dir(target_wrechie_path, project_path);
        std::exit(OK);
        break;
      default:
        ERR_EXIT_MSG(TOO_MANY_ARGUMENTS, "Too many arguments.");
        break;
    }
  };

  std::string err_msg;
  ZipReader project(wrechie_path, &err_msg,
                    whole_file_size - project_size - sizeof(project_data),
                    project_size, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY);
  ERR_COND_EXIT_MSG(!err_msg.empty(), FAIL_TO_OPEN_FILE, err_msg);

  // Get main script -->
  const std::string main_source =
      project.get_file_content("main.wren", &err_msg);
  ERR_COND_EXIT_MSG(!err_msg.empty(), FAIL_TO_OPEN_FILE, err_msg);
  // Get main script <--

  WrenVM* vm;
  NEW_WREN_VM(vm);
  LOAD_MODULE;
  GET_RUNTIME_STATE(vm)->project = &project;
  WrenInterpretResult result =
      wrenInterpret(vm, "main.wren", main_source.c_str());
  FREE_WREN_VM(vm);
}
