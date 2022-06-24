#include "modules/foreign_db.hpp"


namespace ForeignDB{

/* key format: [~]{class_name}:{signature}
If key start with `~`, it is a static method. */
static std::map<std::string, WrenForeignMethodFn> methods;

void bind_method(const std::string& class_name, bool is_static, const std::string& signature, WrenForeignMethodFn method){
    methods[GET_METHOD_KEY(class_name, is_static, signature)] = method;
}

void unbind_method(const std::string& key){
    methods.erase(key);
}

WrenForeignMethodFn get_method(const std::string& class_name, bool is_static, const std::string& signature) {
    std::string key(GET_METHOD_KEY(class_name, is_static, signature));
    return methods.count(key)?methods[key]:nullptr;
}

}// namespace ForeignDB