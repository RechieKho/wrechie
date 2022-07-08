#ifndef _ZIP_HPP_
#define _ZIP_HPP_

#include <miniz.h>

#include <map>
#include <string>

class ZipReader {
  std::string path;
  mz_zip_archive* reader;
  bool reader_initiated;

  std::map<std::string, mz_uint64> file_indices;
  std::map<mz_uint64, std::string> file_contents;  // strings are on heap
  std::map<mz_uint64, mz_zip_archive_file_stat> file_stats;

 public:
  ZipReader(const std::string& path, mz_uint64 file_start_offset = 0,
            mz_uint64 archive_size = 0, mz_uint32 flags = 0);

  ~ZipReader();

  // Returns error string if the zip file is invalid.
  static std::string is_zip_valid(const std::string& path,
                                  mz_uint64 file_start_offset = 0,
                                  mz_uint64 archive_size = 0,
                                  mz_uint32 flags = 0);

  // Get file index.
  mz_uint32 get_file_index(const std::string& path, mz_uint flags = 0,
                           const std::string& comment = "");
  // Get file stat.
  mz_zip_archive_file_stat get_file_stat(mz_uint32 file_index,
                                         mz_uint32 flags = 0);
  // Get file stat.
  mz_zip_archive_file_stat get_file_stat(const std::string& path,
                                         mz_uint32 flags = 0);
  // Get file content.
  std::string get_file_content(mz_uint file_index, mz_uint flags = 0);
  // Get file content.
  std::string get_file_content(const std::string& path, mz_uint flags = 0);
  // Reread the zip file. If the zip file is modified, you need to [reread] or
  // else it will give outdated information.
  void reread(mz_uint64 file_start_offset = 0, mz_uint64 archive_size = 0,
              mz_uint32 flags = 0);
};

class ZipWriter {
  mz_zip_archive* writer;
  std::string path;

 public:
  ZipWriter(const std::string& path, mz_uint flags = 0, bool append = true);
  ~ZipWriter();

  void add_file(const std::string& path, mz_uint flags = 0,
                const std::string& archive_path_prefix = "");
};
class ZipHeapWriter {
  mz_zip_archive* writer;
  void* finalized_zip;
  size_t finalized_zip_size;

 public:
  ZipHeapWriter(size_t initial_allocation_size, mz_uint flags = 0);
  ~ZipHeapWriter();
  void add_file(const std::string& path, mz_uint flags = 0,
                const std::string& archive_path_prefix = "");
  void finalize(void** ret_buffer, size_t* ret_size);
};
#endif  //_ZIP_HPP_