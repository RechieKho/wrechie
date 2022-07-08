#include "fs/zip.hpp"

#include <cpppath.hpp>

#include "err.hpp"
#include "fs/fsutils.hpp"
#include "log.hpp"

std::string ZipReader::is_zip_valid(const std::string& path,
                                    mz_uint64 file_start_offset,
                                    mz_uint64 archive_size, mz_uint32 flags) {
  const std::string err_template = "file '{}' is not a valid zip file, {}.";

  char resolved[MAX_PATH_LEN];
  if (GET_REAL_PATH(path.c_str(), resolved) != resolved)
    return fmt::format(std::move(err_template), path,
                       "given file path cannot be resolved");
  mz_zip_archive zip = {0};
  if (!mz_zip_reader_init_file_v2(&zip, path.c_str(), flags, file_start_offset,
                                  archive_size))
    return fmt::format(std::move(err_template), path,
                       mz_zip_get_error_string(mz_zip_get_last_error(&zip)));

  mz_zip_reader_end(&zip);
  return nullptr;
}

ZipReader::ZipReader(const std::string& path, std::string* ret_err_msg,
                     mz_uint64 file_start_offset, mz_uint64 archive_size,
                     mz_uint32 flags)
    : path(path), reader((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))) {
  if (!mz_zip_reader_init_file_v2(reader, path.c_str(), flags,
                                  file_start_offset, archive_size)) {
    *ret_err_msg =
        fmt::format("Fail to read zip file '{}', {}.", path,
                    mz_zip_get_error_string(mz_zip_get_last_error(reader)));
  } else
    reader_initiated = true;
}

ZipReader::~ZipReader() {
  if (reader_initiated) mz_zip_reader_end(reader);
  free(reader);
}

mz_uint32 ZipReader::get_file_index(const std::string& path,
                                    std::string* ret_err_msg, mz_uint flags,
                                    const std::string& comment) {
  if (file_indices.count(path)) return file_indices[path];
  const std::string err_template =
      "Fail to get file index of file '{}' in zip file '{}', {}.";
  if (!reader_initiated) {
    if (ret_err_msg) {
      *ret_err_msg = fmt::format(std::move(err_template), path, this->path,
                                 "zip reader is not initiated.");
    }
    return 0;
  }
  mz_uint32 index = 0;
  if (mz_zip_reader_locate_file_v2(this->reader, path.c_str(), comment.c_str(),
                                   flags, &index))
    file_indices[path] = index;
  else if (ret_err_msg) {
    *ret_err_msg =
        fmt::format(std::move(err_template), path, this->path,
                    mz_zip_get_error_string(mz_zip_get_last_error(reader)));
  }
  return index;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(mz_uint32 file_index,
                                                  std::string* ret_err_msg,
                                                  mz_uint32 flags) {
  if (file_stats.count(file_index)) return file_stats[file_index];
  const std::string err_template =
      "Fail to get stat of file with index of {} from zip file '{}', {}.";
  if (!reader_initiated) {
    if (ret_err_msg) {
      *ret_err_msg = fmt::format(std::move(err_template), file_index,
                                 this->path, "zip reader is not initiated.");
    }
    return {0};
  }
  mz_zip_archive_file_stat stat = {0};
  if (mz_zip_reader_file_stat(this->reader, file_index, &stat))
    file_stats[file_index] = stat;
  else if (ret_err_msg) {
    *ret_err_msg = fmt::format(
        std::move(err_template), file_index, this->path,
        mz_zip_get_error_string(mz_zip_get_last_error((this->reader))));
  }
  return stat;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(const std::string& path,
                                                  std::string* ret_err_msg,
                                                  mz_uint32 flags) {
  std::string err;
  mz_uint64 file_index = get_file_index(path, &err, flags);
  if (!err.empty()) {
    *ret_err_msg = err;
    return {0};
  }
  if (file_stats.count(file_index)) return file_stats[file_index];
  return get_file_stat(file_index, ret_err_msg, flags);
}

std::string ZipReader::get_file_content(mz_uint file_index,
                                        std::string* ret_err_msg,
                                        mz_uint flags) {
  if (file_contents.count(file_index)) return file_contents[file_index];
  const std::string err_template =
      "Fail to get the content of file with index of {} from zip file '{}', "
      "{}.";
  if (!reader_initiated) {
    if (ret_err_msg) {
      *ret_err_msg = fmt::format(std::move(err_template), file_index,
                                 this->path, "zip reader is not initiated.");
    }
    return nullptr;
  }
  size_t size = 0;
  const char* buffer = (const char*)mz_zip_reader_extract_to_heap(
      reader, file_index, &size, flags);
  if (buffer) {
    std::string content(buffer, size);
    file_contents[file_index] = content;
    free((void*)buffer);
    return content;
  } else if (ret_err_msg) {
    *ret_err_msg = fmt::format(
        std::move(err_template), file_index, this->path,
        mz_zip_get_error_string(mz_zip_get_last_error((this->reader))));
  }
  return "";
}

std::string ZipReader::get_file_content(const std::string& path,
                                        std::string* ret_err_msg,
                                        mz_uint flags) {
  std::string err;
  mz_uint32 index = get_file_index(path, &err, flags);
  if (!err.empty()) {
    *ret_err_msg = err;
    return "";
  }
  return get_file_content(index, ret_err_msg, flags);
}

void ZipReader::reread(std::string* ret_err_msg, mz_uint64 file_start_offset,
                       mz_uint64 archive_size, mz_uint32 flags) {
  file_contents.clear();
  file_indices.clear();
  file_stats.clear();
}

ZipWriter::ZipWriter(const std::string& path, mz_uint flags, bool append,
                     std::string* ret_err_msg)
    : writer((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))), path(path) {
  const std::string err_template =
      "Fail to start writing zip to path '{}', {}.";
  if (append) {
    if (!mz_zip_writer_init_from_reader_v2(writer, path.c_str(), flags))
      if (ret_err_msg)
        *ret_err_msg =
            fmt::format(std::move(err_template), path,
                        mz_zip_get_error_string(mz_zip_get_last_error(writer)));
  } else {
    if (!mz_zip_writer_init_file_v2(writer, path.c_str(), 0, flags))
      if (ret_err_msg)
        *ret_err_msg =
            fmt::format(std::move(err_template), path,
                        mz_zip_get_error_string(mz_zip_get_last_error(writer)));
  }
}

ZipWriter::~ZipWriter() {
  mz_zip_writer_finalize_archive(writer);
  mz_zip_writer_end(writer);
  free(writer);
}

bool ZipWriter::add_file(const std::string& path, mz_uint flags,
                         const std::string& archive_path_prefix,
                         std::string* ret_err_msg) {
  const std::string err_template =
      "Fail to add file '{}' to zip file '{}', {}.";

  std::string normalized = cpppath::normpath(archive_path_prefix);
  if (!strncmp(normalized.c_str(), "../", 3)) {
    if (ret_err_msg) {
      *ret_err_msg =
          fmt::format(std::move(err_template), path, this->path,
                      "archive path prefix is beyond the base of zip file");
    }
    return false;
  }

  std::string archive_path =
      cpppath::join({archive_path_prefix, cpppath::filename(path)});

  char resolved[MAX_PATH_LEN];
  if (GET_REAL_PATH(path.c_str(), resolved) != resolved) {
    if (ret_err_msg) {
      *ret_err_msg = fmt::format(std::move(err_template), path, this->path,
                                 "Fail to resolve given file path.");
    }
    return false;
  }

  mz_zip_writer_add_file(writer, archive_path.c_str(), resolved, "", 0, flags);
  return true;
}

ZipHeapWriter::ZipHeapWriter(size_t initial_allocation_size, mz_uint flags,
                             std::string* ret_err_msg)
    : writer((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))),
      finalized_zip(nullptr),
      finalized_zip_size(0) {
  const std::string err_template = "fail to start writing zip on heap, '{}'.";
  if (!mz_zip_writer_init_heap_v2(writer, 0, initial_allocation_size, flags))
    if (ret_err_msg)
      *ret_err_msg =
          fmt::format(std::move(err_template),
                      mz_zip_get_error_string(mz_zip_get_last_error(writer)));
}

ZipHeapWriter::~ZipHeapWriter() {
  mz_zip_writer_end(writer);
  free(writer);
  if (finalized_zip) free(finalized_zip);
}

bool ZipHeapWriter::add_file(const std::string& path, mz_uint flags,
                             const std::string& archive_path_prefix,
                             std::string* ret_err_msg) {
  const std::string err_template =
      "Fail to add file '{}' to zip file on heap, {}.";

  if (finalized_zip)
    if (ret_err_msg) {
      *ret_err_msg =
          fmt::format(std::move(err_template), "zip already be finalized.");
    }

  std::string normalized = cpppath::normpath(archive_path_prefix);
  if (!strncmp(normalized.c_str(), "../", 3)) {
    if (ret_err_msg) {
      *ret_err_msg =
          fmt::format(std::move(err_template),
                      "archive path prefix is beyond the base of zip file");
    }
    return false;
  }

  std::string archive_path =
      cpppath::join({archive_path_prefix, cpppath::filename(path)});

  char resolved[MAX_PATH_LEN];
  if (GET_REAL_PATH(path.c_str(), resolved) != resolved) {
    if (ret_err_msg) {
      *ret_err_msg = fmt::format(std::move(err_template),
                                 "Fail to resolve given file path.");
    }
    return false;
  }

  mz_zip_writer_add_file(writer, archive_path.c_str(), resolved, "", 0, flags);
  return true;
}

bool ZipHeapWriter::finalize(void** ret_buffer, size_t* ret_size,
                             std::string* ret_err_msg) {
  if (finalized_zip) {
    *ret_buffer = finalized_zip;
    *ret_size = finalized_zip_size;
    return true;
  }
  if (!mz_zip_writer_finalize_heap_archive(writer, ret_buffer, ret_size)) {
    if (ret_err_msg)
      *ret_err_msg =
          fmt::format("Fail to finalize zip on heap, {}.",
                      mz_zip_get_error_string(mz_zip_get_last_error(writer)));
    return false;
  }
  finalized_zip = *ret_buffer;
  finalized_zip_size = *ret_size;
  return true;
}