#include <stdio.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cstring>
#include <string>
#include <vector>
#include "String.hpp"

using namespace std;

#ifndef API_H
#define API_H

namespace Il2Cpp {

    int Init(const char *lib);

    int Init(void *handle);

    void *GetImageByName(const char *image);

    bool IsEnum(const void *klass);

    void *GetDomain();

    const char* GetClassNamespace(void* klass);

    int GetFieldFlags(void *field);

    char *GetTypeName(void* type);

    void* GetMethodParam(void *method, uint32_t index);

    const void *GetClassAtCount(const void* image, size_t index);

    size_t GetClassCount(const void* image);

    void* ClassFromType(void *type);

    uint64_t GetBase();

    const void* GetClassProperties(void *klass, void* *iter);

    const void* GetPropertySet(void *prop);

    size_t GetFieldOffset(void *field);

    void GetStaticFieldValueMan(void *field, void *value);

    const void* GetPropertyGet(void *prop);

    const char* GetPropertyName(void* prop);

    uint32_t GetMethodFlags(const void *method, uint32_t * iflags);

    const void *GetMethodReturnType(const void *method);

    void **GetAssemblies(size_t size);

    void *GetImage(const void *domain, void** val);

    void *GetClassType(const char *image, const char *namespaze, const char *clazz);

    void *GetFieldType(void* field);

    void* ClassGetMethods(void *klass, void* *iter);

    void* ClassGetFields(void *klass, void* *iter);

    const void* GetImage(const void *assembly);

    const char* GetFieldName(void *field);

    string GetMethodArgs(const void* method);

    const int GetClassType2(void *clazz);

    const char *GetClassName(void* klass);

    void *CreateArray(const char *image, const char *namespaze, const char *clazz, size_t length);

    void *GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount = 0);

    void *GetMethodOffset(const char *image, const char *namespaze, const char *clazz, const char *name, char **args, int argsCount = 0);

    const char *GetMethodName(void* method);

    size_t GetFieldOffset(const char *image, const char *namespaze, const char *clazz, const char *name);

    void GetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void *output);

    void SetStaticFieldValue(const char *image, const char *namespaze, const char *clazz, const char *name, void* value);

    void *CreateClassInstance(const char *image, const char *namespaze, const char *clazz);

    bool IsAssembliesLoaded();

    Il2CppString *CreateString(const char *s);

    Il2CppString *CreateString(const wchar_t *s, size_t len);

    template <typename T>
    inline T GetMethod(uintptr_t realOffset) {
        return (T) (realOffset);
    }

    template <typename T>
    inline T GetMethod(const char *image, const char *namespaze, const char *clazz, const char *name, int argsCount) {
        auto realOffset = Il2Cpp::GetMethodOffset(image, namespaze, clazz, name, argsCount);
        return (T) (realOffset);
    }

    template <typename T>
    inline T GetField(void* obj, std::ptrdiff_t Offset) {
        if (Offset == 0)
            __android_log_print(ANDROID_LOG_INFO, "Rewtio", "GetField: Offset is 0");
        return *(T *) ((std::uint8_t *) obj + Offset);
    }

    template <typename T>
    void SetField(void* obj, std::ptrdiff_t Offset, T val)
    {
        *(T*)((std::uint8_t*)obj + Offset) = val;
    }

    std::string GetString(void *string);

    uintptr_t GetFieldOffset(void *clazz, const char *name);
};


#endif //API_H
