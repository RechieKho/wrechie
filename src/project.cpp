#include "project.hpp"

#include <miniz.h>

#include <cpppath.hpp>
#include <fstream>

#include "err.hpp"
#include "log.hpp"

static const unsigned char project_magic_number[4] = {0xDE, 0xAD, 0x69, 0xAF};

bool get_project_size_in_wrechie(const std::string& wrechie_path,
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
  if (!memcmp(&data.magic, project_magic_number, sizeof(std::uint32_t))) {
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

  // create magic
  project_data data;
  memcpy(&data.magic, project_magic_number, sizeof(std::uint32_t));
  project.seekg(0, std::ios::end);
  data.project_size = project.tellg();

  char c;
  wrechie.seekg(0, std::ios::beg);
  project.seekg(0, std::ios::beg);
  while (wrechie.get(c)) output << c;
  while (project.get(c)) output << c;
  for (size_t i = 0; i < sizeof(project_data); i++) output << ((char*)&data)[i];
}