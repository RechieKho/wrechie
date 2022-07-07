#ifndef _ZIP_HPP_
#define _ZIP_HPP_

#include <miniz.h>

#include <map>
#include <string>

class Zip {
  std::string path;
  mz_zip_archive* reader;
  bool reader_initiated;

  std::map<std::string, mz_uint64> file_indices;
  std::map<mz_uint64, std::string> file_contents;  // strings are on heap
  std::map<mz_uint64, mz_zip_archive_file_stat> file_stats;

  std::map<std::string, std::string> files_to_be_zipped;

 public:
  Zip(const std::string& path, std::string* ret_err_msg = nullptr,
      mz_uint64 file_start_offset = 0, mz_uint64 archive_size = 0,
      mz_uint32 flags = 0);

  ~Zip();

  // Returns error string if the zip file is invalid.
  static std::string is_zip_valid(const std::string& path,
                                  mz_uint64 file_start_offset = 0,
                                  mz_uint64 archive_size = 0,
                                  mz_uint32 flags = 0);

  // Queue file to be zipped by [write_zip].
  bool add_file(const std::string& path,
                const std::string& archive_path_prefix = "",
                std::string* ret_err_msg = nullptr);
  // Write files queued by [add_file] to zip. Note that you need to [reread] to
  // mount the newly zipped file into the memory in order to read them.
  bool write_zip(std::string* ret_err_msg = nullptr,
                 mz_uint64 levels_and_flags = 0);
  // Get file index.
  mz_uint32 get_file_index(const std::string& path,
                           std::string* ret_err_msg = nullptr,
                           mz_uint flags = 0, const std::string& comment = "");
  // Get file stat.
  mz_zip_archive_file_stat get_file_stat(mz_uint32 file_index,
                                         std::string* ret_err_msg = nullptr,
                                         mz_uint32 flags = 0);
  // Get file stat.
  mz_zip_archive_file_stat get_file_stat(const std::string& path,
                                         std::string* ret_err_msg = nullptr,
                                         mz_uint32 flags = 0);
  // Get file content.
  std::string get_file_content(mz_uint file_index,
                               std::string* ret_err_msg = nullptr,
                               mz_uint flags = 0);
  // Get file content.
  std::string get_file_content(const std::string& path,
                               std::string* ret_err_msg = nullptr,
                               mz_uint flags = 0);
  // Reread the zip file. If the zip file is modified, you need to [reread] or
  // else it will give outdated information.
  void reread(std::string* ret_err_msg = nullptr,
              mz_uint64 file_start_offset = 0, mz_uint64 archive_size = 0,
              mz_uint32 flags = 0);
};

#endif  //_ZIP_HPP_