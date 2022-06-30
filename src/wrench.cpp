#include <errno.h>
#include <limits.h>
#include <fstream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <cxxopts.hpp>
#include <wren.hpp>
#include <cpppath.hpp>

#include "log.hpp"
#include "filesystem.hpp"
#include "runtime.hpp"
#include "modules/modules.hpp"
#include "typedef.hpp"


int main(int argc, const char *argv[]){
    // Options -->
    cxxopts::Options opts("wrench", "A wren runtime packed with package manager.");
    opts.add_options()
        ("h, help", "Print usage.")
        ("i, input", "Path to input wren script.", cxxopts::value<std::string>())
        ("a, arguments", "Arguments passed into the script.", cxxopts::value<std::string>())
    ;
    opts.custom_help("[-h]");
    opts.parse_positional({"input", "arguments"});
    opts.positional_help("INPUT [ARGUMENTS...]");
    auto parsed_opts = opts.parse(argc, argv);
    if(parsed_opts.count("help")){
        fmt::print(opts.help() + std::string("\n"));
        exit(0);
    }
    // Options <--

    ERR_COND_EXIT_MSG(!parsed_opts.count("input"), 1, "No script specified.");
    char script_path[MAX_PATH_LEN];
    GET_REAL_PATH_RET(parsed_opts["input"].as<std::string>().c_str(), script_path, 1);
    std::string file_content;
    READ_APPEND_FILE_RET(script_path, file_content, 1);

    WrenVM* vm;
    GET_WREN_VM(vm);
    LOAD_MODULE;
    ((RuntimeState *)wrenGetUserData(vm))->cwd = cpppath::dirname(script_path);
    WrenInterpretResult result = wrenInterpret(vm, script_path, file_content.c_str());
    wrenFreeVM(vm);
}