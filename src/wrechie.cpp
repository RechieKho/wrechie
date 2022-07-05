#include <errno.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <miniz.h>
#include <whereami.h>

#include <cpppath.hpp>
#include <cxxopts.hpp>
#include <fstream>
#include <wren.hpp>

#include "err.hpp"
#include "fs/fsutils.hpp"
#include "fs/zip.hpp"
#include "log.hpp"
#include "modules/modules.hpp"
#include "runtime.hpp"
#include "typedef.hpp"

// follows semantic versioning
#define WRECHIE_VER "0.0.7"

static inline size_t get_program_size(
    const std::string &path);  // forward declaration

int main(int argc, const char *argv[]) {
  int executable_path_length = wai_getExecutablePath(NULL, 0, NULL);
  char executable_path[executable_path_length + 1];
  wai_getExecutablePath(executable_path, executable_path_length, NULL);
  executable_path[executable_path_length] = '\0';

  int program_size = get_program_size(executable_path);
  int actual_size;
  GET_FILE_SIZE(executable_path, actual_size);
  int archive_size = actual_size - program_size;

  if (archive_size <= 0) {
    fmt::print(fmt::format(
        "{}\n{}\n\n",
        fmt::format(fmt::fg(fmt::color::white) | fmt::emphasis::bold,
                    "wrechie version {}, a general purpose programming "
                    "environment that runs wren.",
                    WRECHIE_VER),
        fmt::format(fmt::fg(fmt::color::gray),
                    "Currently no project loaded. Append your project "
                    "in a zip file to run the project.")));
    fmt::print(
        "The informations shown should be correct:\n"
        " > Full path to program: {}\n"
        " > Program size: {} bytes \n\n",
        (char *)executable_path, program_size);
    exit(0);
  };

  ZipReader project(executable_path, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY,
                    program_size, archive_size);

  // Get main script -->
  std::string main_source;
  READ_FILE_FROM_ZIP(project, "main.wren", main_source, 1);
  // Get main script <--

  WrenVM *vm;
  NEW_WREN_VM(vm);
  LOAD_MODULE;
  GET_RUNTIME_STATE(vm)->project = &project;
  WrenInterpretResult result =
      wrenInterpret(vm, "main.wren", main_source.c_str());
  FREE_WREN_VM(vm);
}

// get_program_size -->
#if defined(_PLATFORM_UNIX_)
#include <elfio/elfio.hpp>
static inline size_t get_program_size(const std::string &path) {
  ELFIO::elfio reader;
  ERR_COND_EXIT_MSG(!reader.load(path), FAIL_TO_READ_FILE,
                    fmt::format("Fail to load ELF header of '{}'", path));

  return reader.get_sections_offset() +
         (reader.get_section_entry_size() * reader.sections.size());
}

#elif defined(_PLATFORM_WIN_)
/* Info about this:
https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/pe-file-header-parser-in-c++#code
https://stackoverflow.com/questions/34684660/how-to-determine-the-size-of-an-pe-executable-file-from-headers-and-or-footers
*/
#include "Windows.h"
static inline size_t get_program_size(const std::string &path) {
  HANDLE program = CreateFileA(path.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  ERR_COND_EXIT_MSG(program == INVALID_HANDLE_VALUE, FAIL_TO_READ_FILE,
                    fmt::format("Fail to load ELF header of '{}'", path));
  size_t size;
  GET_FILE_SIZE(path, size);
  void *data = malloc(size);
  DWORD byte_read;
  ReadFile(program, data, size, &byte_read, NULL);

  PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)data;
  PIMAGES_NT_HEADERS image_NT_headers =
      (PIMAGE_NT_HEADERS)((DWORD)data + dos_header->e_lfanew);

  size_t program_size = image_NT_headers->OptionalHeader.SizeOfHeaders;

  DWORD section_location =
      (DWORD)imageNTHeaders + sizeof(DWORD) +
      (DWORD)(sizeof(IMAGE_FILE_HEADER)) +
      (DWORD)image_NT_headers->FileHeader.SizeOfOptionalHeader;

  for (int i = 0; i < imageNTHeaders->FileHeader.NumberOfSections; i++) {
    section_header = (PIMAGE_SECTION_HEADER)section_location;
    program_size += section_header->SizeOfRawData;
  }
}
#endif
// get_program_size() <--