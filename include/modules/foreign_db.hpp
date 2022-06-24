#ifndef _FOREIGN_DB_HPP_
#define _FOREIGN_DB_HPP_

#include <map>
#include <string>
#include <fmt/core.h>

#include <wren.hpp>

#ifndef GET_METHOD_KEY
#define GET_METHOD_KEY(class_name, is_static, signature) \
    fmt::format("{}{}:{}", (is_static?"~":""), class_name, signature)
#endif

namespace ForeignDB
{
    void bind_method(const std::string& class_name, bool is_static, const std::string& signature, WrenForeignMethodFn method);
    void unbind_method(const std::string& key);
    WrenForeignMethodFn get_method(const std::string& class_name, bool is_static, const std::string& signature);
} // namespace ForeignDB

#endif//_FOREIGN_DB_HPP_