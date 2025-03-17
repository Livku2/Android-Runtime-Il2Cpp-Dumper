//
// Created by aimar on 12/28/2019.
//

#include "API.h"
#include <locale>
#include <codecvt>
#include <Logger.h>
#include <sstream>

namespace {
    const void *(*il2cpp_assembly_get_image)(const void *assembly);

    void *(*il2cpp_domain_get)();

    size_t (*il2cpp_image_get_class_count)(const void *image);

    const void *(*il2cpp_image_get_class)(const void* image, size_t);

    int (*il2cpp_method_get_param_count)(const void* method);

    void **(*il2cpp_domain_get_assemblies)(const void *domain, size_t *size);

    const char *(*il2cpp_image_get_name)(const void *image);

    void (*il2cpp_field_get_value)(void *obj, void* field, void* value);

    const char *(*il2cpp_class_get_namespace)(void *klass);

    void *(*il2cpp_class_from_name)(const void *image, const char *namespaze, const char *name);

    void *(*il2cpp_class_get_property_from_name)(void *klass, const char *name);

    void *(*il2cpp_class_get_field_from_name)(void *klass, const char *name);

    void *(*il2cpp_class_get_method_from_name)(void *klass, const char *name, int argsCount);

    void *(*il2cpp_runtime_invoke)(void* method, void* obj, void** params, void* exec);

    void *(*il2cpp_property_get_get_method)(void *prop);

    void *(*il2cpp_property_get_set_method)(void *prop);

    void *(*il2cpp_class_from_type)(const void* type);

    size_t (*il2cpp_field_get_offset)(void *field);

    void *(*il2cpp_field_get_type)(void *field);

    void (*il2cpp_field_static_get_value)(void *field, void *value);

    void (*il2cpp_field_static_set_value)(void *field, void *value);

    void *(*il2cpp_array_new)(void *elementTypeInfo, size_t length);

    int32_t (*il2cpp_string_length)(void *str);

    char16_t *(*il2cpp_string_chars)(void *str);

    Il2CppString *(*il2cpp_string_new)(const char *);

    Il2CppString *(*il2cpp_string_new_utf16)(const wchar_t *, size_t len);

    char *(*il2cpp_type_get_name)(void *type);

    void* (*il2cpp_method_get_param)(void *method, uint32_t index);

    void* (*il2cpp_class_get_methods)(void *klass, void* *iter);

    void* (*il2cpp_class_get_fields)(void *klass, void* *iter);

    const char* (*il2cpp_method_get_name)(void *method);

    const char* (*il2cpp_field_get_name)(void *field);

    void *(*il2cpp_class_get_interfaces)(void *klass, void **iter);

    const char* (*il2cpp_class_get_name)(void *klass);

    void* (*il2cpp_object_get_class)(void *obj);

    int (*il2cpp_class_get_type)(void *);

    void *(*il2cpp_class_get_parent)(void *);

    void *(*il2cpp_class_get_declaring_type)(void *);

    void *(*il2cpp_class_get_nested_types)(void *, void **);

    void *(*il2cpp_object_new)(void *);

    const void *(*il2cpp_method_get_return_type)(const void* method);

    const void *(*il2cpp_class_get_properties)(void *klass, void* *iter);

    const char *(*il2cpp_property_get_name)(void* prop);

    uint32_t (*il2cpp_method_get_flags)(const void *method, uint32_t * iflags);

    int (*il2cpp_field_get_flags)(void * field);

    bool (*il2cpp_class_is_enum)(const void *klass);
}

// ================================================================================================================ //
void *getExportFunction(void *handle, const char *name)
{
    void *fn = dlsym(handle, name);
    if (fn) {
        return fn;
    }
    return nullptr;
}
// ========================================================================================================================================== //
typedef unsigned short UTF16;
typedef wchar_t UTF32;
typedef char UTF8;

int is_surrogate(UTF16 uc) {
    return (uc - 0xd800u) < 2048u;
}

int is_high_surrogate(UTF16 uc) {
    return (uc & 0xfffffc00) == 0xd800;
}

int is_low_surrogate(UTF16 uc) {
    return (uc & 0xfffffc00) == 0xdc00;
}

UTF32 surrogate_to_utf32(UTF16 high, UTF16 low) {
    return (high << 10) + low - 0x35fdc00;
}

const char* utf16_to_utf8(const UTF16* source, size_t len) {
    std::u16string s(source, source + len);
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(s).c_str();
}

const wchar_t* utf16_to_utf32(const UTF16* source, size_t len) {
    UTF32* output = new UTF32[len + 1];

    for (int i = 0; i < len; i++) {
        const UTF16 uc = source[i];
        if (!is_surrogate(uc)) {
            output[i] = uc;
        }
        else {
            if (is_high_surrogate(uc) && is_low_surrogate(source[i]))
                output[i] = surrogate_to_utf32(uc, source[i]);
            else
                output[i] = L'?';
        }
    }

    output[len] = L'\0';
    return output;
}
// ========================================================================================================================================== //
const char* Il2CppString::CString() {
    return utf16_to_utf8(&this->start_char, this->length);
}
// ========================================================================================================================================== //
const wchar_t* Il2CppString::WCString() {
    return utf16_to_utf32(&this->start_char, this->length);
}

int init(void *handle)
{
    il2cpp_assembly_get_image = (const void *(*)(const void *)) getExportFunction(handle, "il2cpp_assembly_get_image");

    il2cpp_domain_get = (void *(*)()) getExportFunction(handle, "il2cpp_domain_get");

    il2cpp_field_get_value = (void (*)(void*,void*,void*)) getExportFunction(handle, "il2cpp_field_get_value");

    il2cpp_object_get_class = (void *(*)(void*)) getExportFunction(handle, "il2cpp_object_get_class");

    il2cpp_class_is_enum = (bool (*)(const void*)) getExportFunction(handle, "il2cpp_class_is_enum");

    il2cpp_field_get_flags = (int (*)(void *)) getExportFunction(handle, "il2cpp_field_get_flags");

    il2cpp_method_get_flags = (uint32_t (*)(const void *, uint32_t *)) getExportFunction(handle, "il2cpp_method_get_flags");

    il2cpp_property_get_name = (const char *(*)(void *)) getExportFunction(handle, "il2cpp_property_get_name");

    il2cpp_class_get_properties = (const void *(*)(void*, void**)) getExportFunction(handle, "il2cpp_class_get_properties");

    il2cpp_class_from_type = (void *(*)(const void*)) getExportFunction(handle, "il2cpp_class_from_type");

    il2cpp_method_get_return_type = (const void *(*)(const void *)) getExportFunction(handle, "il2cpp_method_get_return_type");

    il2cpp_method_get_param_count = (int (*)(const void*)) getExportFunction(handle, "il2cpp_method_get_param_count");

    il2cpp_image_get_class_count = (size_t (*)(const void*)) getExportFunction(handle, "il2cpp_image_get_class_count");

    il2cpp_image_get_class = (const void* (*)(const void*, size_t)) getExportFunction(handle, "il2cpp_image_get_class");

    il2cpp_domain_get_assemblies = (void **(*)(const void* , size_t*)) getExportFunction(handle, "il2cpp_domain_get_assemblies");

    il2cpp_image_get_name = (const char *(*)(const void *)) getExportFunction(handle, "il2cpp_image_get_name");

    il2cpp_class_get_namespace = (const char *(*)(void *)) getExportFunction(handle, "il2cpp_class_get_namespace");

    il2cpp_class_from_name = (void* (*)(const void*, const char*, const char *)) getExportFunction(handle, "il2cpp_class_from_name");

    il2cpp_class_get_property_from_name = (void* (*)(void *, const char *)) getExportFunction(handle, "il2cpp_class_get_property_from_name");;

    il2cpp_class_get_field_from_name = (void* (*)(void*, const char *)) getExportFunction(handle, "il2cpp_class_get_field_from_name");;

    il2cpp_class_get_method_from_name = (void* (*)(void *, const char*, int)) getExportFunction(handle, "il2cpp_class_get_method_from_name");

    il2cpp_runtime_invoke = (void *(*)(void* , void* , void** , void*)) getExportFunction(handle, "il2cpp_runtime_invoke");

    il2cpp_property_get_get_method = (void* (*)(void *)) getExportFunction(handle, "il2cpp_property_get_get_method");;

    il2cpp_property_get_set_method = (void* (*)(void *)) getExportFunction(handle, "il2cpp_property_get_set_method");;

    il2cpp_field_get_offset = (size_t (*)(void *)) getExportFunction(handle, "il2cpp_field_get_offset");;

    il2cpp_field_static_get_value = (void (*)(void*, void *)) getExportFunction(handle, "il2cpp_field_static_get_value");;

    il2cpp_field_static_set_value = (void (*)(void*, void *)) getExportFunction(handle, "il2cpp_field_static_set_value");;

    il2cpp_array_new = (void *(*)(void*, size_t)) getExportFunction(handle, "il2cpp_array_new");;

    il2cpp_string_chars = (char16_t *(*)(void*)) getExportFunction(handle, "il2cpp_string_chars");;

    il2cpp_string_length = (int32_t (*)(void*)) getExportFunction(handle, "il2cpp_string_length");;

    il2cpp_type_get_name = (char *(*)(void *)) getExportFunction(handle, "il2cpp_type_get_name");;

    il2cpp_method_get_param = (void *(*)(void *, uint32_t)) getExportFunction(handle, "il2cpp_method_get_param");;

    il2cpp_field_get_type = (void *(*)(void *)) getExportFunction(handle, "il2cpp_field_get_type");;

    il2cpp_class_get_methods = (void *(*)(void *, void **)) getExportFunction(handle, "il2cpp_class_get_methods");;

    il2cpp_class_get_fields = (void *(*)(void *, void **)) getExportFunction(handle, "il2cpp_class_get_fields");;

    il2cpp_method_get_name = (const char *(*)(void *)) getExportFunction(handle, "il2cpp_method_get_name");;

    il2cpp_field_get_name = (const char *(*)(void *)) getExportFunction(handle, "il2cpp_field_get_name");;

    il2cpp_class_get_interfaces = (void *(*)(void *, void **)) getExportFunction(handle, "il2cpp_class_get_interfaces");;

    il2cpp_class_get_name = (const char *(*)(void *)) getExportFunction(handle, "il2cpp_class_get_name");

    il2cpp_class_get_type = (int (*)(void *)) getExportFunction(handle, "il2cpp_class_get_type");

    il2cpp_class_get_parent = (void *(*)(void *)) getExportFunction(handle, "il2cpp_class_get_parent");

    il2cpp_class_get_declaring_type = (void *(*)(void *)) getExportFunction(handle, "il2cpp_class_get_declaring_type");

    il2cpp_class_get_nested_types = (void *(*)(void *, void **)) getExportFunction(handle, "il2cpp_class_get_nested_types");

    il2cpp_string_new = (Il2CppString *(*)(const char *)) getExportFunction(handle, "il2cpp_string_new");

    il2cpp_string_new_utf16 = (Il2CppString *(*)(const wchar_t *, size_t)) getExportFunction(handle, "il2cpp_string_new_utf16");

    return 0;
}
// ================================================================================================================ //
int Il2Cpp::Init(const char *lib) {
    if(!lib) return -1;

    return init(dlopen(lib, 4));

}
// ================================================================================================================ //
int Il2Cpp::Init(void *handle) {
    if(!handle) return -1;

    return init(handle);

}

void *Il2Cpp::GetDomain() {
    return il2cpp_domain_get();
}

bool Il2Cpp::IsEnum(const void *klass){
    return il2cpp_class_is_enum(klass);
}

void **Il2Cpp::GetAssemblies(size_t size) {
    return il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
}

const void *Il2Cpp::GetImage(const void *assembly) {
    return il2cpp_assembly_get_image(assembly);
}

void* Il2Cpp::InvokeMethod(void* instance, const char* methodName, const char* arg) {
    if (!instance || !methodName) {
        LOGE("Instance or method name is null.");
        return nullptr;
    }

    // Get the class of the instance
    void* klass = il2cpp_object_get_class(instance);
    if (!klass) {
        LOGE("Failed to get class for object.");
        return nullptr;
    }

    // Find the method in the class (assuming 1 parameter for now)
    void* method = il2cpp_class_get_method_from_name(klass, methodName, 1);
    if (!method) {
        LOGE("Method not found: %s", methodName);
        return nullptr;
    }

    // Convert the string argument into an Il2CppString*
    Il2CppString* il2cppStrArg = il2cpp_string_new(arg);
    if (!il2cppStrArg) {
        LOGE("Failed to create Il2CppString for argument.");
        return nullptr;
    }

    // Prepare parameters for the method call
    void* params[1] = { il2cppStrArg };

    // Prepare exception holder
    void* exception = nullptr;

    // Invoke the method
    void* result = il2cpp_runtime_invoke(method, instance, params, &exception);

    if (exception) {
        LOGE("Exception thrown while invoking method: %s", methodName);
        return nullptr;
    }

    return result;
}

void* Il2Cpp::InvokeMethod(void* instance, const char* methodName) {
    if (!instance || !methodName) {
        LOGE("Instance or method name is null.");
        return nullptr;
    }

    // Get the class of the instance
    void* klass = il2cpp_object_get_class(instance);
    if (!klass) {
        LOGE("Failed to get class for object.");
        return nullptr;
    }

    // Find the method in the class (assuming 1 parameter for now)
    void* method = il2cpp_class_get_method_from_name(klass, methodName, 1);
    if (!method) {
        LOGE("Method not found: %s", methodName);
        return nullptr;
    }

    // Prepare parameters for the method call
    void* params[0] = {  };

    // Prepare exception holder
    void* exception = nullptr;

    // Invoke the method
    void* result = il2cpp_runtime_invoke(method, instance, params, &exception);

    if (exception) {
        LOGE("Exception thrown while invoking method: %s", methodName);
        return nullptr;
    }

    return result;
}


void* Il2Cpp::GetFieldValue(void* instance, const char* fieldName) {
    if (!instance || !fieldName) {
        LOGE("Instance or field name is null.");
        return nullptr;
    }

    // Get the class of the instance
    void* klass = il2cpp_object_get_class(instance);
    if (!klass) {
        LOGE("Failed to get class for object.");
        return nullptr;
    }

    // Find the field in the class
    void* field = il2cpp_class_get_field_from_name(klass, fieldName);
    if (!field) {
        LOGE("Field not found: %s", fieldName);
        return nullptr;
    }

    // Retrieve the field value
    void* fieldValue = nullptr;
    il2cpp_field_get_value(instance, field, &fieldValue);

    // Return the field value
    return fieldValue;
}


const char *Il2Cpp::GetImageName(const void *image) {
    return il2cpp_image_get_name(image);
}

const void *Il2Cpp::GetClassAtCount(const void *image, size_t index) {
    return il2cpp_image_get_class(image, index);
}

const void *Il2Cpp::GetMethodReturnType(const void *method) {
    return il2cpp_method_get_return_type(method);
}

size_t Il2Cpp::GetClassCount(const void *image) {
    return il2cpp_image_get_class_count(image);
}

uint32_t Il2Cpp::GetMethodFlags(const void *method, uint32_t * iflags) {
    return il2cpp_method_get_flags(method, iflags);
}

int Il2Cpp::GetFieldFlags(void *field) {
    return il2cpp_field_get_flags(field);
}

const char* Il2Cpp::GetPropertyName(void* prop){
    return il2cpp_property_get_name(prop);
}

void Il2Cpp::GetStaticFieldValueMan(void *field, void *value){
    il2cpp_field_static_get_value(field, value);
}

const char* Il2Cpp::GetClassNamespace(void* klass){
    return il2cpp_class_get_namespace(klass);
}

const char* Il2Cpp::GetClassName(void *klass) {
    return il2cpp_class_get_name(klass);
}

const char* Il2Cpp::GetFieldName(void *field) {
    return il2cpp_field_get_name(field);
}

void* Il2Cpp::ClassFromType(void *type) {
    return il2cpp_class_from_type(type);
}

const void* Il2Cpp::GetClassProperties(void *klass, void* *iter){
    return il2cpp_class_get_properties(klass, iter);
}

const void* Il2Cpp::GetPropertyGet(void *prop){
    return il2cpp_property_get_get_method(prop);
}

const void* Il2Cpp::GetPropertySet(void *prop){
    return il2cpp_property_get_set_method(prop);
}

uint64_t Il2Cpp::GetBase() {
    Dl_info dlInfo;
    if (dladdr((void *) il2cpp_domain_get_assemblies, &dlInfo)) {
        if (dlInfo.dli_fbase) {
            return reinterpret_cast<uint64_t>(dlInfo.dli_fbase);
        }
    }
    return 0;
}

void *Il2Cpp::GetFieldType(void* field){
    return il2cpp_field_get_type(field);
}

char* Il2Cpp::GetTypeName(void *type) {
    return il2cpp_type_get_name(type);
}

void* Il2Cpp::ClassGetMethods(void *klass, void* *iter) {
    return il2cpp_class_get_methods(klass, iter);
}

void* Il2Cpp::ClassGetFields(void *klass, void **iter) {
    return il2cpp_class_get_fields(klass, iter);
}

void* Il2Cpp::GetMethodParam(void *method, uint32_t index){
    return il2cpp_method_get_param(method, index);
}

size_t Il2Cpp::GetFieldOffset(void *field){
    return il2cpp_field_get_offset(field);
}

std::string Il2Cpp::GetMethodArgs(const void* method) {
    std::stringstream outPut;

    int paramCount = il2cpp_method_get_param_count(method);
    for (int i = 0; i < paramCount; i++){
        void* parameterType = il2cpp_method_get_param(const_cast<void *>(method), i);

        char* parameterTypeName = il2cpp_type_get_name(parameterType);
        outPut << parameterTypeName << ", ";
    }

    return outPut.str();
}

// ================================================================================================================ //
void *Il2Cpp::GetImageByName(const char *image) {
    size_t size;
    void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);
    for(int i = 0; i < size; ++i)
    {
        void *img = (void *)il2cpp_assembly_get_image(assemblies[i]);

        const char *img_name = il2cpp_image_get_name(img);

        if(strcmp(img_name, image) == 0)
        {
            return img;
        }
    }
    return nullptr;
}
// ================================================================================================================ //
void Il2Cpp::GetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void *output) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return;
    }
    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s for field %s!", clazz, name);
        return;
    }

    void *field = il2cpp_class_get_field_from_name(klass, name);
    if(!field) {
        LOGI("Can't find field %s in class %s!", name, clazz);
        return;
    }

    il2cpp_field_static_get_value(field, output);
}
// ================================================================================================================ //
void Il2Cpp::SetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void* value) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return;
    }
    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s for field %s!", clazz, name);
        return;
    }

    void *field = il2cpp_class_get_field_from_name(klass, name);
    if(!field) {
        LOGI("Can't find field %s in class %s!", name, clazz);
        return;
    }

    il2cpp_field_static_set_value(field, value);
}
const int Il2Cpp::GetClassType2(void *clazz) {
    return  il2cpp_class_get_type(clazz);
}

// ================================================================================================================ //
void *Il2Cpp::GetClassType(const char *image, const char *namespaze, const char *clazz) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return nullptr;
    }

    void *klass = il2cpp_class_from_name(img, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s!", clazz);
        return nullptr;
    }
    return klass;
}
// ================================================================================================================ //
void *Il2Cpp::CreateClassInstance(const char *image, const char *namespaze, const char *clazz) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return nullptr;
    }

    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s!", clazz);
        return nullptr;
    }

    void *obj = il2cpp_object_new(klass);
    if(!obj)
    {
        LOGI("Can't create object for %s", clazz);
        return nullptr;
    }

    return obj;
}
// ================================================================================================================ //
void *Il2Cpp::GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return nullptr;
    }

    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s for method %s!", clazz, name);
        return nullptr;
    }

    void **method = (void**)il2cpp_class_get_method_from_name(klass, name, argsCount);
    if(!method) {
        LOGI("Can't find method %s in class %s!", name, clazz);
        return nullptr;
    }

    return *method;
}
// ================================================================================================================ //
void *Il2Cpp::GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, char** args, int argsCount) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return nullptr;
    }

    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s for method %s!", clazz, name);
        return nullptr;
    }

    void *iter = nullptr;

    int score = 0;

    void **method = (void**) il2cpp_class_get_methods(klass, &iter);
    while(method) {
        const char *fname = il2cpp_method_get_name(method);
        if(strcmp(fname, name) == 0) {
            for (int i = 0; i < argsCount; i++) {
                void *arg = il2cpp_method_get_param(method, i);
                if (arg) {
                    const char *tname = il2cpp_type_get_name(arg);
                    if (strcmp(tname, args[i]) == 0) {
                        score++;
                    } else {
                        LOGI("Argument at index %d didn't matched requested argument!\r\n\tRequested: %s\r\n\tActual: %s\r\nSkipping function...", i, args[i], tname);
                        score = 0;
                        goto skip;
                    }
                }
            }
        }
        skip:

        if(score == argsCount)
        {
            LOGI("Found matched function!");
            return *method;
        }

        method = (void **) il2cpp_class_get_methods(klass, &iter);
    }
    LOGI("Cannot find function %s in class %s!", name, clazz);
    return nullptr;
}

const char *Il2Cpp::GetMethodName(void* method){
    return il2cpp_method_get_name(method);
}

uintptr_t Il2Cpp::GetFieldOffset(void *clazz, const char *name){
    void *field = il2cpp_class_get_field_from_name(clazz, name);
    if(!field) {
        LOGI("Can't find field %s!", name);
        return -1;
    }
    return il2cpp_field_get_offset(field);
}
// ================================================================================================================ //
size_t Il2Cpp::GetFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return -1;
    }
    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s for field %s!", clazz, name);
        return -1;
    }

    void *field = il2cpp_class_get_field_from_name(klass, name);
    if(!field) {
        LOGI("Can't find field %s in class %s!", name, clazz);
        return -1;
    }
    return il2cpp_field_get_offset(field);
}
// ================================================================================================================ //
bool Il2Cpp::IsAssembliesLoaded() {
    size_t size;
    void **assemblies = il2cpp_domain_get_assemblies(il2cpp_domain_get(), &size);

    return size != 0 && assemblies != nullptr;
}
// ================================================================================================================ //
void* Il2Cpp::CreateArray(const char *image, const char *namespaze, const char *clazz, size_t length) {
    void *img = GetImageByName(image);
    if(!img) {
        LOGI("Can't find image %s!", image);
        return nullptr;
    }
    void *klass = GetClassType(image, namespaze, clazz);
    if(!klass) {
        LOGI("Can't find class %s!", clazz);
        return nullptr;
    }

    return il2cpp_array_new(klass, length);
}
// ================================================================================================================ //

std::string Il2Cpp::GetString(void *string) {
    if (string) {
        std::u16string str = il2cpp_string_chars(string);
        std::wstring wide_string(str.begin(), str.end());
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
        return convert.to_bytes(wide_string);
    }
    return "Object NULL!";
}

Il2CppString *Il2Cpp::CreateString(const char *s)
{
    return il2cpp_string_new(s);
}

Il2CppString *Il2Cpp::CreateString(const wchar_t *s, size_t len)
{
    return il2cpp_string_new_utf16(s, len);
}
