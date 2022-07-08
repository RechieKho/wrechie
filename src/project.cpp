#include "project.hpp"

#include <miniz.h>
#include <string.h>
#include <sys/stat.h>

#include <cpppath.hpp>
#include <filesystem>
#include <fstream>

#include "err.hpp"
#include "fs/fsutils.hpp"
#include "fs/zip.hpp"
#include "log.hpp"

static const magic_t project_magic_number = 3735944939;  // 0xDEADFEEB

bool get_project_size_in_wrechie(const std::string& wrechie_path,
                                 size_t& project_size,
                                 size_t& whole_file_size) {
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!wrechie.is_open(), wrechie_path);
  project_data data = {0};
  whole_file_size = wrechie.seekg(-(sizeof(project_data)), std::ios::end)
                        .read((char*)&data, sizeof(project_data))
                        .tellg();
  wrechie.close();
  if (!memcmp(&data.magic, &project_magic_number, sizeof(magic_t))) {
    project_size = data.project_size;
    return true;
  }

  return false;
}

void bundle_project_on_mem(const std::string& wrechie_path, const void* buffer,
                           size_t size, const std::string& output_name) {
  // start file streams
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!wrechie.is_open(), wrechie_path);
  std::string output_path = cpppath::join({cpppath::curdir(), output_name});
  std::ofstream output(output_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!output.is_open(), output_path);

  // get project size in wrechie
  project_data data = {0};
  size_t whole_file_size;
  whole_file_size = wrechie.seekg(-(sizeof(project_data)), std::ios::end)
                        .read((char*)&data, sizeof(project_data))
                        .tellg();

  // create magic
  project_data new_data = {0};
  memcpy(&new_data.magic, &project_magic_number, sizeof(size_t));
  new_data.project_size = size;

  size_t wrechie_size = whole_file_size;
  if (!memcmp(&data.magic, &project_magic_number, sizeof(magic_t)))
    wrechie_size -= (data.project_size + sizeof(project_data));
  // read binary into buffer
  size_t binary_size = wrechie_size + size + sizeof(project_data);
  char* binary = new char[binary_size];
  wrechie.seekg(0, std::ios::beg);
  wrechie.read(binary, wrechie_size);
  memcpy(binary + wrechie_size, buffer, size);
  memcpy(binary + wrechie_size + size, &new_data, sizeof(project_data));

  // write to output
  output.write(binary, binary_size);

  // finishing
  wrechie.close();
  output.close();
  delete[] binary;
}

void bundle_project(const std::string& wrechie_path,
                    const std::string& project_path) {
  // start file streams
  std::ifstream project(project_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!project.is_open(), project_path);

  project.seekg(0, std::ios::end);

  // read project into buffer
  size_t project_size = project.tellg();
  char* binary = new char[project_size];
  project.seekg(0, std::ios::beg);
  project.read(binary, project_size);

  // write to output
  bundle_project_on_mem(wrechie_path, binary, project_size,
                        cpppath::filebase(project_path));

  // finishing
  project.close();
  delete[] binary;
}

void bundle_project_in_dir(const std::string& wrechie_path,
                           const std::string& dir_path) {
  char resolved_dir_path[MAX_PATH_LEN];
  ERR_COND_EXIT_FAIL_TO_RESOLVE_PATH(
      !GET_REAL_PATH(dir_path.c_str(), resolved_dir_path), dir_path);

  struct stat dir_stat;
  stat(resolved_dir_path, &dir_stat);
  ERR_COND_EXIT_MSG(
      !S_ISDIR(dir_stat.st_mode), FAIL_TO_OPEN_FILE,
      fmt::format("Fail to bundle project, given path '{}' is not a directory.",
                  resolved_dir_path));

  std::string err;
  ZipHeapWriter writer(500, 0, &err);
  ERR_COND_EXIT_MSG(!err.empty(), ZIP_WRITTER_FAIL, err);

  for (const std::filesystem::directory_entry& dir_entry :
       std::filesystem::recursive_directory_iterator(resolved_dir_path)) {
    if (dir_entry.is_regular_file()) {
      std::string entry_path = dir_entry.path();
      std::string archive_path_prefix = cpppath::dirname(entry_path.substr(
          strlen(resolved_dir_path) +
          1));  // assuming resolve_dir_path is the root of project
      writer.add_file(entry_path, 0, archive_path_prefix, &err);
      ERR_COND_EXIT_MSG(!err.empty(), ZIP_WRITTER_FAIL, err);
    }
  }

  void* zip_binary;
  size_t zip_binary_size;
  writer.finalize(&zip_binary, &zip_binary_size, &err);
  ERR_COND_EXIT_MSG(!err.empty(), ZIP_WRITTER_FAIL, err);

  bundle_project_on_mem(wrechie_path, zip_binary, zip_binary_size,
                        cpppath::filebase(resolved_dir_path) + ".bin");
}
