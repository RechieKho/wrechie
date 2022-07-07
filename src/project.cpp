#include "project.hpp"

#include <miniz.h>

#include <cpppath.hpp>
#include <fstream>

#include "err.hpp"
#include "log.hpp"

static const magic_t project_magic_number = 3735944939;  // 0xDEADFEEB

bool get_project_size_in_wrechie(const std::string& wrechie_path,
                                 size_t& project_size,
                                 size_t& whole_file_size) {
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_MSG(!wrechie.is_open(), FAIL_TO_READ_FILE,
                    fmt::format("Could not open file '{}'.", wrechie_path));
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

void bundle_project(const std::string& wrechie_path,
                    const std::string& project_path) {
  // Check whether is it a zip
  mz_zip_archive zip = {0};
  ERR_COND_EXIT_MSG(
      !mz_zip_reader_init_file_v2(&zip, project_path.c_str(), 0, 0, 0),
      FAIL_TO_READ_FILE,
      fmt::format("Project '{}' is not a valid zip file, {}.", project_path,
                  mz_zip_get_error_string(mz_zip_get_last_error(&zip))));
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

  // get project size in wrechie
  project_data data = {0};
  size_t whole_file_size;
  whole_file_size = wrechie.seekg(-(sizeof(project_data)), std::ios::end)
                        .read((char*)&data, sizeof(project_data))
                        .tellg();

  // create magic
  project_data new_data;
  memcpy(&new_data.magic, &project_magic_number, sizeof(size_t));
  project.seekg(0, std::ios::end);
  new_data.project_size = project.tellg();

  size_t wrechie_size = whole_file_size;
  if (!memcmp(&data.magic, &project_magic_number, sizeof(magic_t)))
    wrechie_size -= (data.project_size + sizeof(project_data));
  // read binary into buffer
  char* binary = new char[wrechie_size + new_data.project_size];
  wrechie.seekg(0, std::ios::beg);
  wrechie.read(binary, wrechie_size);
  project.seekg(0, std::ios::beg);
  project.read(binary + wrechie_size, new_data.project_size);

  // write to output
  output.write(binary, wrechie_size + new_data.project_size);
  output.write((const char*)&new_data, sizeof(project_data));

  // finishing
  wrechie.close();
  project.close();
  output.close();
  delete[] binary;
}