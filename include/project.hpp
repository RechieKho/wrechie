#ifndef _PROJECT_HPP_
#define _PROJECT_HPP_

#include <cstdint>
#include <string>

struct project_data {
  std::uint32_t magic;
  std::uint64_t project_size;
};

bool get_project_size_in_wrechie(const std::string& wrechie_path,
                                 std::uint64_t& project_size,
                                 std::uint64_t& whole_file_size);

void bundle_project(const std::string& wrechie_path,
                    const std::string& project_path);

#endif  //_PROJECT_HPP_