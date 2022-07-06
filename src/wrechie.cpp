#include <errno.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <miniz.h>
#include <whereami.h>

#include <cpppath.hpp>
#include <cxxopts.hpp>
#include <fstream>
#include <wren.hpp>

#include "err.hpp"
#include "fs/fsutils.hpp"
#include "fs/zip.hpp"
#include "log.hpp"
#include "modules/modules.hpp"
#include "runtime.hpp"
#include "typedef.hpp"

static const unsigned char magic_number[4] = {0xDE, 0xAD, 0x69, 0xAF};
struct project_data {
  std::uint32_t magic;
  std::uint64_t project_size;
};

static inline bool get_wrechie_size(
    const std::string& wrechie_path, std::uint64_t& project_size,
    std::uint64_t& whole_file_size);  // forward declaration
static inline void bundle_project(
    const std::string& wrechie_path,
    const std::string& project_path);  // forward declaration

int main(int argc, const char* argv[]) {
  int wrechie_path_length = wai_getExecutablePath(NULL, 0, NULL);
  char wrechie_path[wrechie_path_length + 1];
  wai_getExecutablePath(wrechie_path, wrechie_path_length, NULL);
  wrechie_path[wrechie_path_length] = '\0';

  std::uint64_t project_size;
  std::uint64_t whole_file_size;

  if (!get_wrechie_size(wrechie_path, project_size, whole_file_size)) {
    cxxopts::Options opts(
        "wrechie",
        fmt::format("wrechie version {}, a general purpose programming "
                    "environment that runs wren.\n",
                    WRECHIE_VER));
    opts.add_options()("h, help", "Show help and quit.")(
        "p, project", "Path to project (zip file).",
        cxxopts::value<std::string>());
    opts.parse_positional({"project"});
    opts.custom_help("[-h]");
    opts.positional_help("PROJECT_PATH");

    cxxopts::ParseResult result = opts.parse(argc, argv);
    if (result.count("help")) {
      fmt::print(opts.help());
      exit(OK);
    }

    ERR_COND_EXIT_MSG(!result.count("project"), TOO_FEW_ARGUMENTS,
                      "project path is not given.")
    bundle_project(wrechie_path, result["project"].as<std::string>());
    exit(OK);
  };

  ZipReader project(wrechie_path, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY,
                    whole_file_size - project_size - sizeof(project_data),
                    project_size);

  // Get main script -->
  std::string main_source;
  READ_FILE_FROM_ZIP(project, "main.wren", main_source, 1);
  // Get main script <--

  WrenVM* vm;
  NEW_WREN_VM(vm);
  LOAD_MODULE;
  GET_RUNTIME_STATE(vm)->project = &project;
  WrenInterpretResult result =
      wrenInterpret(vm, "main.wren", main_source.c_str());
  FREE_WREN_VM(vm);
}

static inline bool get_wrechie_size(const std::string& wrechie_path,
                                    std::uint64_t& project_size,
                                    std::uint64_t& whole_file_size) {
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_MSG(!wrechie.is_open(), FAIL_TO_READ_FILE,
                    fmt::format("Could not open file '{}'.", wrechie_path));
  project_data data = {0};
  whole_file_size = wrechie.seekg(-(sizeof(project_data)), std::ios::end)
                        .read((char*)&data, sizeof(project_data))
                        .tellg();
  wrechie.close();
  if (!memcmp(&data.magic, magic_number, sizeof(std::uint32_t))) {
    project_size = data.project_size;
    return true;
  }

  return false;
}

static inline void bundle_project(const std::string& wrechie_path,
                                  const std::string& project_path) {
  // Check whether is it a zip
  mz_zip_archive zip = {0};
  ERR_COND_EXIT_MSG(
      !mz_zip_reader_init_file_v2(&zip, project_path.c_str(), 0, 0, 0),
      WRONG_FILE_FORMAT,
      fmt::format("Project must be a valid zip file.", project_path));
  mz_zip_reader_end(&zip);

  // start file streams
  std::ifstream project(project_path, std::ios::binary);
  ERR_COND_EXIT_MSG(
      !project.is_open(), FAIL_TO_READ_FILE,
      fmt::format("Could not read from file '{}'.", project_path));
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_MSG(
      !wrechie.is_open(), FAIL_TO_READ_FILE,
      fmt::format("Could not read from file '{}'.", wrechie_path));
  std::string output_path =
      cpppath::join({cpppath::curdir(), cpppath::filebase(project_path)});
  std::ofstream output(output_path, std::ios::binary);
  ERR_COND_EXIT_MSG(!output.is_open(), FAIL_TO_READ_FILE,
                    fmt::format("Could not write to file '{}'.", output_path));

  // create magic
  project_data data;
  memcpy(&data.magic, magic_number, sizeof(std::uint32_t));
  project.seekg(0, std::ios::end);
  data.project_size = project.tellg();

  char c;
  wrechie.seekg(0, std::ios::beg);
  project.seekg(0, std::ios::beg);
  while (wrechie.get(c)) output << c;
  while (project.get(c)) output << c;
  for (size_t i = 0; i < sizeof(project_data); i++) output << ((char*)&data)[i];
}