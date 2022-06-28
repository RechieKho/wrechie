#ifndef _WRENCH_DYLIB_H_
#define _WRENCH_DYLIB_H_

#include <string>

/* Load
@desc
    This function will be called when the shared library 
    is loaded into wren with [Lib.load()] method.
@return
    bool success
@param
    WrenchBindForeignMethodFn bind_method_from_lib
    WrenchBindClassMethodsFn bind_class_methods_from_lib
*/

/* WrenchBindForeignMethodFn
@noreturn
@param
    const std::string& class_name
    bool is_static
    const std::string& wren_method_signature
    const std::string& c_func_symbol
*/
typedef void (*WrenchBindForeignMethodFn)(
    const std::string& class_name,
    bool is_static, 
    const std::string& wren_method_signature, 
    const std::string& c_func_symbol);

/* WrenchBindClassMethodsFn
@noreturn
@param
    const std::string& class_name
    const std::string& allocator_func_symbol
    const std::string& finalizer_func_symbol
*/
typedef void (*WrenchBindClassMethodsFn)(
    const std::string& class_name, 
    const std::string& allocator_func_symbol, 
    const std::string& finalizer_func_symbol);


#endif//_WRENCH_DYLIB_H_