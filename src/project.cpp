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

void bundle_project(const std::string& wrechie_path,
                    const std::string& project_path) {
  // start file streams
  std::ifstream project(project_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!project.is_open(), project_path);
  std::ifstream wrechie(wrechie_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!wrechie.is_open(), wrechie_path);
  std::string output_path =
      cpppath::join({cpppath::curdir(), cpppath::filebase(project_path)});
  std::ofstream output(output_path, std::ios::binary);
  ERR_COND_EXIT_FAIL_TO_OPEN_FILE(!output.is_open(), output_path);

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