#ifndef _ZIP_HPP_
#define _ZIP_HPP_

#include <miniz.h>

#include <map>
#include <string>

// input zip
class ZipReader {
  mutable mz_zip_archive* archive;
  std::string path;
  mutable std::map<std::string, mz_uint64> file_indices;
  mutable std::map<mz_uint64, const char*>
      file_contents;  // strings are on heap
  mutable std::map<mz_uint64, mz_zip_archive_file_stat> file_stats;

 public:
  ZipReader(const std::string& path, mz_uint32 flags);
  ZipReader(const std::string& path, mz_uint32 flags,
            mz_uint64 file_start_offset, mz_uint64 archive_size);

  ~ZipReader();

  mz_uint32 get_file_index(const std::string& path, mz_uint flags = 0,
                           const std::string& comment = "") const;
  mz_zip_archive_file_stat get_file_stat(mz_uint32 file_index,
                                         mz_uint32 flags = 0) const;
  mz_zip_archive_file_stat get_file_stat(const std::string& path,
                                         mz_uint32 flags = 0) const;
  std::string get_file_content(mz_uint file_index, mz_uint flags = 0) const;
  std::string get_file_content(const std::string& path,
                               mz_uint flags = 0) const;
};

#endif  //_ZIP_HPP_