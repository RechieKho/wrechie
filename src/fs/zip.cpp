#include "fs/zip.hpp"

#include "err.hpp"
#include "log.hpp"

ZipReader::ZipReader(const std::string& path, mz_uint32 flags)
    : archive((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))), path(path) {
  ERR_COND_EXIT_MSG(
      !mz_zip_reader_init_file_v2(this->archive, path.c_str(), flags, 0, 0),
      FAIL_TO_READ_FILE,
      fmt::format(
          "Fail to open zip file '{}', {}.", path,
          mz_zip_get_error_string(mz_zip_get_last_error((this->archive)))));
}

ZipReader::ZipReader(const std::string& path, mz_uint32 flags,
                     mz_uint64 file_start_offset, mz_uint64 archive_size)
    : archive((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))), path(path) {
  ERR_COND_EXIT_MSG(
      !mz_zip_reader_init_file_v2(this->archive, path.c_str(), flags,
                                  file_start_offset, archive_size),
      FAIL_TO_READ_FILE,
      fmt::format(
          "Fail to open zip file '{}', {}.", path,
          mz_zip_get_error_string(mz_zip_get_last_error((this->archive)))));
}

ZipReader::~ZipReader() {
  mz_zip_reader_end(this->archive);
  free(this->archive);
  for (std::pair<mz_uint64, const char*> content : file_contents)
    free((void*)content.second);
}

mz_uint32 ZipReader::get_file_index(const std::string& path, mz_uint flags,
                                    const std::string& comment) const {
  if (file_indices.count(path)) return file_indices[path];
  mz_uint32 index;
  ERR_COND_EXIT_MSG(
      !mz_zip_reader_locate_file_v2(this->archive, path.c_str(),
                                    comment.c_str(), flags, &index),
      FAIL_TO_READ_FILE,
      fmt::format(
          "Fail to get file index of {} from zip file '{}', {}.", path,
          this->path,
          mz_zip_get_error_string(mz_zip_get_last_error((this->archive)))));
  file_indices[path] = index;
  return index;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(mz_uint32 file_index,
                                                  mz_uint32 flags) const {
  if (file_stats.count(file_index)) return file_stats[file_index];
  mz_zip_archive_file_stat stat;
  ERR_COND_EXIT_MSG(!mz_zip_reader_file_stat(this->archive, file_index, &stat),
                    FAIL_TO_READ_FILE,
                    fmt::format("Fail to get file stat of file with index of "
                                "{} from zip file '{}', {}.",
                                file_index, this->path,
                                mz_zip_get_error_string(
                                    mz_zip_get_last_error((this->archive)))));
  file_stats[file_index] = stat;
  return stat;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(const std::string& path,
                                                  mz_uint32 flags) const {
  mz_uint64 file_index = get_file_index(path, flags);
  if (file_stats.count(file_index)) return file_stats[file_index];
  return get_file_stat(file_index, flags);
}

std::string ZipReader::get_file_content(mz_uint file_index,
                                        mz_uint flags) const {
  if (file_contents.count(file_index)) return file_contents[file_index];
  size_t size;
  const char* buffer;
  ERR_COND_EXIT_MSG(!(buffer = (const char*)mz_zip_reader_extract_to_heap(
                          this->archive, file_index, &size, flags)),
                    FAIL_TO_READ_FILE,
                    fmt::format("Fail to get file content of file with index "
                                "of {} from zip file {}, {}.",
                                file_index, this->path,
                                mz_zip_get_error_string(
                                    mz_zip_get_last_error((this->archive)))));
  file_contents[file_index] = buffer;
  return std::string(buffer, size);
}

std::string ZipReader::get_file_content(const std::string& path,
                                        mz_uint flags) const {
  mz_uint64 file_index = get_file_index(path, flags);
  return get_file_content(file_index, flags);
}