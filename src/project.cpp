#include "project.hpp"

#include <miniz.h>
#include <string.h>
#include <sys/stat.h>

#include <cpppath.hpp>
#include <filesystem>
#include <fstream>

#include "exception.hpp"
#include "fs/fsutils.hpp"
#include "fs/zip.hpp"
#include "log.hpp"

static const magic_t project_magic_number = 3735944939;  // 0xDEADFEEB

bool get_project_size_in_wrechie(const std::string& wrechie_path,
                                 size_t& project_size,
                                 size_t& whole_file_size) {
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  COND_THROW_EXC(!wrechie.is_open(), FAIL_TO_OPEN_FILE,
                 EXC_STR_FAIL_TO_OPEN_FILE(wrechie_path));
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
  COND_THROW_EXC(!wrechie.is_open(), FAIL_TO_OPEN_FILE,
                 EXC_STR_FAIL_TO_OPEN_FILE(wrechie_path));
  std::string output_path = cpppath::join({cpppath::curdir(), output_name});
  std::ofstream output(output_path, std::ios::binary);
  COND_THROW_EXC(!output.is_open(), FAIL_TO_OPEN_FILE,
                 EXC_STR_FAIL_TO_OPEN_FILE(output_path));

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
  COND_THROW_EXC(!project.is_open(), FAIL_TO_OPEN_FILE,
                 EXC_STR_FAIL_TO_OPEN_FILE(project_path));

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
  COND_THROW_EXC(!GET_REAL_PATH(dir_path.c_str(), resolved_dir_path),
                 FAIL_TO_RESOLVE_PATH, EXC_STR_FAIL_TO_RESOLVE_PATH(dir_path));

  struct stat dir_stat;
  stat(resolved_dir_path, &dir_stat);
  COND_THROW_EXC(
      !S_ISDIR(dir_stat.st_mode), INVALID_FILE_FORMAT,
      EXC_STR_INVALID_FILE_FORMAT(resolved_dir_path, "expecting a directory"));

  try {
    ZipHeapWriter writer(500, 0);
    for (const std::filesystem::directory_entry& dir_entry :
         std::filesystem::recursive_directory_iterator(resolved_dir_path)) {
      if (dir_entry.is_regular_file()) {
        std::string entry_path = dir_entry.path().string();
        std::string archive_path_prefix = cpppath::dirname(entry_path.substr(
            strlen(resolved_dir_path) +
            1));  // assuming resolve_dir_path is the root of project
        writer.add_file(entry_path, 0, archive_path_prefix);
      }
    }
    void* zip_binary;
    size_t zip_binary_size;
    writer.finalize(&zip_binary, &zip_binary_size);

    std::string project_name =
        cpppath::filebase(cpppath::normpath(resolved_dir_path)) + ".bin";
    bundle_project_on_mem(wrechie_path, zip_binary, zip_binary_size,
                          project_name);
  } catch (const WrechieException& e) {
    throw e;
  }
}
