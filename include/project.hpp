#ifndef _PROJECT_HPP_
#define _PROJECT_HPP_

#include <cstdint>
#include <string>

typedef std::uint32_t magic_t;

struct project_data {
  magic_t magic;
  size_t project_size;
};

bool get_project_size_in_wrechie(const std::string& wrechie_path,
                                 size_t& project_size, size_t& whole_file_size);

void bundle_project(const std::string& wrechie_path,
                    const std::string& project_path);

#endif  //_PROJECT_HPP_