#include <map>

#include "modules/foreign_db.hpp"


namespace ForeignDB{

/* key format: [~]{class_name}:{signature}
If key start with `~`, it is a static method. */
static std::map<std::string, WrenForeignMethodFn> methods;
static std::map<std::string, WrenForeignClassMethods> class_methods;

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

void bind_class_methods(const std::string& class_name, WrenForeignClassMethods class_method){
    class_methods[class_name] = class_method;
}

void unbind_class_methods(const std::string& class_name){
    class_methods.erase(class_name);
}

WrenForeignClassMethods get_class_methods(const std::string& class_name){
    if(class_methods.count(class_name)) return class_methods[class_name];
    else{
        WrenForeignClassMethods empty = {0};
        return empty;
    }
}

}// namespace ForeignDB