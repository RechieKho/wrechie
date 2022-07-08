#include "fs/zip.hpp"

#include <cpppath.hpp>

#include "exception.hpp"
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

ZipReader::ZipReader(const std::string& path, mz_uint64 file_start_offset,
                     mz_uint64 archive_size, mz_uint32 flags)
    : path(path), reader((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))) {
  COND_THROW_EXC(
      !mz_zip_reader_init_file_v2(reader, path.c_str(), flags,
                                  file_start_offset, archive_size),
      ZIP_READER_FAIL,
      EXC_STR_ZIP_READER_FAIL(
          path, mz_zip_get_error_string(mz_zip_get_last_error(reader))));
  reader_initiated = true;
}

ZipReader::~ZipReader() {
  if (reader_initiated) mz_zip_reader_end(reader);
  free(reader);
}

mz_uint32 ZipReader::get_file_index(const std::string& path, mz_uint flags,
                                    const std::string& comment) {
  if (file_indices.count(path)) return file_indices[path];
  const std::string err_template = "fail to get file index of file '{}', {}";
  COND_THROW_EXC(
      !reader_initiated, ZIP_READER_FAIL,
      EXC_STR_ZIP_READER_FAIL(
          this->path, fmt::format(std::move(err_template), path, this->path,
                                  "zip reader is not initiated.")));
  mz_uint32 index = 0;
  COND_THROW_EXC(
      !mz_zip_reader_locate_file_v2(this->reader, path.c_str(), comment.c_str(),
                                    flags, &index),
      ZIP_READER_FAIL,
      fmt::format(std::move(err_template), path, this->path,
                  mz_zip_get_error_string(mz_zip_get_last_error(reader))));
  file_indices[path] = index;
  return index;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(mz_uint32 file_index,
                                                  mz_uint32 flags) {
  const std::string err_template =
      "cannot get file stat of file with the index of {}, {}.";
  if (file_stats.count(file_index)) return file_stats[file_index];
  COND_THROW_EXC(
      !reader_initiated, ZIP_READER_FAIL,
      EXC_STR_ZIP_READER_FAIL(this->path,
                              fmt::format(std::move(err_template), file_index,
                                          "zip reader is not initiated.")));
  mz_zip_archive_file_stat stat = {0};
  COND_THROW_EXC(!mz_zip_reader_file_stat(this->reader, file_index, &stat),
                 ZIP_READER_FAIL,
                 fmt::format(std::move(err_template), file_index,
                             mz_zip_get_error_string(
                                 mz_zip_get_last_error((this->reader)))));
  file_stats[file_index] = stat;
  return stat;
}

mz_zip_archive_file_stat ZipReader::get_file_stat(const std::string& path,
                                                  mz_uint32 flags) {
  try {
    mz_uint64 file_index = get_file_index(path, flags);
    if (file_stats.count(file_index)) return file_stats[file_index];
    return get_file_stat(file_index, flags);
  } catch (const WrechieException& e) {
    throw e;
  }
}

std::string ZipReader::get_file_content(mz_uint file_index, mz_uint flags) {
  if (file_contents.count(file_index)) return file_contents[file_index];
  const std::string err_template =
      "Fail to get the content of file with index of {}, "
      "{}.";
  COND_THROW_EXC(!reader_initiated, ZIP_READER_FAIL,
                 EXC_STR_ZIP_READER_FAIL(
                     path, fmt::format(std::move(err_template), file_index,
                                       "zip reader is not initiated.")));
  size_t size = 0;
  const char* buffer = (const char*)mz_zip_reader_extract_to_heap(
      reader, file_index, &size, flags);

  COND_THROW_EXC(
      !buffer, ZIP_READER_FAIL,
      EXC_STR_ZIP_READER_FAIL(
          path, fmt::format(std::move(err_template), file_index,
                            mz_zip_get_error_string(
                                mz_zip_get_last_error((this->reader))))));

  std::string content(buffer, size);
  file_contents[file_index] = content;
  free((void*)buffer);
  return content;
}

std::string ZipReader::get_file_content(const std::string& path,
                                        mz_uint flags) {
  try {
    mz_uint32 index = get_file_index(path, flags);
    return get_file_content(index, flags);
  } catch (const WrechieException& e) {
    throw e;
  }
}

void ZipReader::reread(mz_uint64 file_start_offset, mz_uint64 archive_size,
                       mz_uint32 flags) {
  file_contents.clear();
  file_indices.clear();
  file_stats.clear();
}

ZipWriter::ZipWriter(const std::string& path, mz_uint flags, bool append)
    : writer((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))), path(path) {
  const std::string err_template =
      "Fail to start writing zip to path '{}', {}.";
  if (append) {
    COND_THROW_EXC(
        !mz_zip_writer_init_from_reader_v2(writer, path.c_str(), flags),
        ZIP_WRITER_FAIL,
        fmt::format(std::move(err_template), path,
                    mz_zip_get_error_string(mz_zip_get_last_error(writer))));
  } else {
    COND_THROW_EXC(
        !mz_zip_writer_init_file_v2(writer, path.c_str(), 0, flags),
        ZIP_WRITER_FAIL,
        fmt::format(std::move(err_template), path,
                    mz_zip_get_error_string(mz_zip_get_last_error(writer))));
  }
}

ZipWriter::~ZipWriter() {
  mz_zip_writer_finalize_archive(writer);
  mz_zip_writer_end(writer);
  free(writer);
}

void ZipWriter::add_file(const std::string& path, mz_uint flags,
                         const std::string& archive_path_prefix) {
  const std::string err_template = "Fail to add file '{}', {}.";

  std::string normalized = cpppath::normpath(archive_path_prefix);
  COND_THROW_EXC(
      !strncmp(normalized.c_str(), "../", 3), ZIP_WRITER_FAIL,
      EXC_STR_ZIP_WRITER_FAIL(
          this->path,
          fmt::format(std::move(err_template), path,
                      "archive path prefix is beyond the base of zip file")));

  std::string archive_path =
      cpppath::join({archive_path_prefix, cpppath::filename(path)});

  char resolved[MAX_PATH_LEN];
  COND_THROW_EXC(
      GET_REAL_PATH(path.c_str(), resolved) != resolved, ZIP_WRITER_FAIL,
      EXC_STR_ZIP_WRITER_FAIL(this->path,
                              fmt::format(std::move(err_template), path,
                                          "fail to resolve given file path")));

  COND_THROW_EXC(
      !mz_zip_writer_add_file(writer, archive_path.c_str(), resolved, "", 0,
                              flags),
      ZIP_WRITER_FAIL,
      EXC_STR_ZIP_WRITER_FAIL(this->path, fmt::format(std::move(err_template),
                                                      path, "internal error")));
}

ZipHeapWriter::ZipHeapWriter(size_t initial_allocation_size, mz_uint flags)
    : writer((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))),
      finalized_zip(nullptr),
      finalized_zip_size(0) {
  const std::string err_template = "Fail to start writing zip on heap, '{}'.";
  COND_THROW_EXC(
      !mz_zip_writer_init_heap_v2(writer, 0, initial_allocation_size, flags),
      ZIP_WRITER_FAIL,
      fmt::format(std::move(err_template),
                  mz_zip_get_error_string(mz_zip_get_last_error(writer))));
}

ZipHeapWriter::~ZipHeapWriter() {
  mz_zip_writer_end(writer);
  free(writer);
  if (finalized_zip) free(finalized_zip);
}

void ZipHeapWriter::add_file(const std::string& path, mz_uint flags,
                             const std::string& archive_path_prefix) {
  const std::string err_template =
      "Fail to add file '{}' to zip file on heap, {}.";

  COND_THROW_EXC(
      finalized_zip, ZIP_WRITER_FAIL,
      fmt::format(std::move(err_template), path, "zip already be finalized"));

  std::string normalized = cpppath::normpath(archive_path_prefix);
  COND_THROW_EXC(
      !strncmp(normalized.c_str(), "../", 3), ZIP_WRITER_FAIL,
      fmt::format(std::move(err_template), path,
                  "archive path prefix is beyond the base of zip file"));

  std::string archive_path =
      cpppath::join({archive_path_prefix, cpppath::filename(path)});

  char resolved[MAX_PATH_LEN];
  COND_THROW_EXC(GET_REAL_PATH(path.c_str(), resolved) != resolved,
                 ZIP_WRITER_FAIL,
                 fmt::format(std::move(err_template), path,
                             "fail to resolve given file path"));

  COND_THROW_EXC(!mz_zip_writer_add_file(writer, archive_path.c_str(), resolved,
                                         "", 0, flags),
                 ZIP_WRITER_FAIL,
                 fmt::format(std::move(err_template), path, "internal error"));
}

void ZipHeapWriter::finalize(void** ret_buffer, size_t* ret_size) {
  if (finalized_zip) {
    *ret_buffer = finalized_zip;
    *ret_size = finalized_zip_size;
  }
  COND_THROW_EXC(
      !mz_zip_writer_finalize_heap_archive(writer, ret_buffer, ret_size),
      ZIP_WRITER_FAIL,
      fmt::format("Fail to finalize zip on heap, {}.",
                  mz_zip_get_error_string(mz_zip_get_last_error(writer))));
  finalized_zip = *ret_buffer;
  finalized_zip_size = *ret_size;
}