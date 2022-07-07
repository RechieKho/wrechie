#include "fs/zip.hpp"

#include <cpppath.hpp>

#include "err.hpp"
#include "fs/fsutils.hpp"
#include "log.hpp"

std::string Zip::is_zip_valid(const std::string& path,
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

Zip::Zip(const std::string& path, std::string* ret_err_msg,
         mz_uint64 file_start_offset, mz_uint64 archive_size, mz_uint32 flags)
    : path(path), reader((mz_zip_archive*)calloc(1, sizeof(mz_zip_archive))) {
  if (!mz_zip_reader_init_file_v2(reader, path.c_str(), flags,
                                  file_start_offset, archive_size)) {
    *ret_err_msg =
        fmt::format("Fail to read zip file '{}', {}.", path,
                    mz_zip_get_error_string(mz_zip_get_last_error(reader)));
  } else
    reader_initiated = true;
}

Zip::~Zip() {
  if (reader_initiated) mz_zip_reader_end(reader);
  free(reader);
}

mz_uint32 Zip::get_file_index(const std::string& path, std::string* ret_err_msg,
                              mz_uint flags, const std::string& comment) {
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

mz_zip_archive_file_stat Zip::get_file_stat(mz_uint32 file_index,
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

mz_zip_archive_file_stat Zip::get_file_stat(const std::string& path,
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

std::string Zip::get_file_content(mz_uint file_index, std::string* ret_err_msg,
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

std::string Zip::get_file_content(const std::string& path,
                                  std::string* ret_err_msg, mz_uint flags) {
  std::string err;
  mz_uint32 index = get_file_index(path, &err, flags);
  if (!err.empty()) {
    *ret_err_msg = err;
    return nullptr;
  }
  return get_file_content(index, ret_err_msg, flags);
}

bool Zip::add_file(const std::string& path,
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

  files_to_be_zipped[archive_path] = resolved;
  return true;
}

bool Zip::write_zip(std::string* ret_err_msg, mz_uint64 levels_and_flags) {
  mz_zip_archive zip = {0};
  const std::string err_template = "Fail to write zip file '{}', {}.";

  if (reader_initiated)
    if (!mz_zip_writer_init_from_reader_v2(&zip, path.c_str(),
                                           levels_and_flags)) {
      *ret_err_msg =
          fmt::format(std::move(err_template), path,
                      mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
      return false;
    }
  for (std::pair<std::string, std::string> file : files_to_be_zipped)
    if (!mz_zip_writer_add_file(&zip, file.first.c_str(), file.second.c_str(),
                                nullptr, 0, levels_and_flags)) {
      std::string& err_msg = *ret_err_msg;
      err_msg = fmt::format(
          std::move(err_template), path,
          fmt::format("{} when writing file '{}' as '{}'.",
                      mz_zip_get_error_string(mz_zip_get_last_error(&zip)),
                      file.second, file.first));
      return false;
    }

  mz_zip_writer_finalize_archive(&zip);
  mz_zip_writer_end(&zip);
  files_to_be_zipped.clear();
  return true;
}

void Zip::reread(std::string* ret_err_msg, mz_uint64 file_start_offset,
                 mz_uint64 archive_size, mz_uint32 flags) {
  file_contents.clear();
  file_indices.clear();
  file_stats.clear();
}
