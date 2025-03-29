#include <jni.h>
#include <string>
#include <thread>
#include <cstdint>
#include <vector>
#include "Engine/Includes.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <array>
#include <utility>


ProcMap map;

const char* GetBetterName(const char* returnTypeName) {
    if (strcmp(returnTypeName, "Void") == 0 || strcmp(returnTypeName, "System.Void") == 0) {
        return "void";
    }
    if (strcmp(returnTypeName, "Single") == 0) {
        return "float";
    }
    if (strcmp(returnTypeName, "Int32") == 0) {
        return "int";
    }
    if (strcmp(returnTypeName, "Boolean") == 0) {
        return "bool";
    }
    return returnTypeName;
}

std::string getMethodModifier(uint32_t flags) {
    std::stringstream outPut;
    auto access = flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK;
    switch (access) {
        case METHOD_ATTRIBUTE_PRIVATE:
            outPut << "private ";
            break;
        case METHOD_ATTRIBUTE_PUBLIC:
            outPut << "public ";
            break;
        case METHOD_ATTRIBUTE_FAMILY:
            outPut << "protected ";
            break;
        case METHOD_ATTRIBUTE_ASSEM:
        case METHOD_ATTRIBUTE_FAM_AND_ASSEM:
            outPut << "internal ";
            break;
        case METHOD_ATTRIBUTE_FAM_OR_ASSEM:
            outPut << "protected internal ";
            break;
    }
    if (flags & METHOD_ATTRIBUTE_STATIC) {
        outPut << "static ";
    }
    if (flags & METHOD_ATTRIBUTE_ABSTRACT) {
        outPut << "abstract ";
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_FINAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_REUSE_SLOT) {
            outPut << "sealed override ";
        }
    } else if (flags & METHOD_ATTRIBUTE_VIRTUAL) {
        if ((flags & METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK) == METHOD_ATTRIBUTE_NEW_SLOT) {
            outPut << "virtual ";
        } else {
            outPut << "override ";
        }
    }
    if (flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
        outPut << "extern ";
    }
    return outPut.str();
}
std::string dumpProperties(void *klass){
    std::stringstream outPut;

    void *iter = nullptr;
    while (auto constProperty = Il2Cpp::GetClassProperties(klass, &iter)) {

        outPut << "\t\t";

        auto property = const_cast<void *>(constProperty);
        auto get = Il2Cpp::GetPropertyGet(property);
        auto set = Il2Cpp::GetPropertySet(property);
        auto propertyName = Il2Cpp::GetPropertyName(property);
        void *propertyClass = nullptr;
        uint32_t iflags = 0;
        if (get) {
            outPut << getMethodModifier(Il2Cpp::GetMethodFlags(get, &iflags));
            propertyClass = Il2Cpp::ClassFromType(const_cast<void*>(Il2Cpp::GetMethodReturnType(get)));
        } else if (set) {
            outPut << getMethodModifier(Il2Cpp::GetMethodFlags(set, &iflags));
            auto param = Il2Cpp::GetMethodParam(const_cast<void*>(set), 0);
            propertyClass = Il2Cpp::ClassFromType(param);
        }
        if (propertyClass) {
            auto fixname = GetBetterName(Il2Cpp::GetClassName(propertyClass));
            outPut << fixname << " " << propertyName << " { ";
            if (get) {
                outPut << "get; ";
            }
            if (set) {
                outPut << "set; ";
            }
            outPut << "}\n";
        } else {
            if (propertyName) {
                outPut << " // unknown property " << propertyName;
            }
        }
    }

    return outPut.str();
}

std::string dumpFields(void *klass){
    std::stringstream outPut;

    outPut << " ";

    void *iter = nullptr;
    while (auto field = Il2Cpp::ClassGetFields(klass, &iter)) {

        auto fieldName = Il2Cpp::GetFieldName(field);

        auto type = Il2Cpp::GetFieldType(field);


        auto isEnum = Il2Cpp::IsEnum(klass);

        auto field_class = Il2Cpp::ClassFromType(type);
        auto attrs = Il2Cpp::GetFieldFlags(field);
        auto access = attrs & FIELD_ATTRIBUTE_FIELD_ACCESS_MASK;
        switch (access) {
            case FIELD_ATTRIBUTE_PRIVATE:
                outPut << "private ";
                break;
            case FIELD_ATTRIBUTE_PUBLIC:
                outPut << "public ";
                break;
            case FIELD_ATTRIBUTE_FAMILY:
                outPut << "protected ";
                break;
            case FIELD_ATTRIBUTE_ASSEMBLY:
            case FIELD_ATTRIBUTE_FAM_AND_ASSEM:
                outPut << "internal ";
                break;
            case FIELD_ATTRIBUTE_FAM_OR_ASSEM:
                outPut << "protected internal ";
                break;
        }
        if (attrs & FIELD_ATTRIBUTE_LITERAL) {
            outPut << "const ";
        } else {
            if (attrs & FIELD_ATTRIBUTE_STATIC) {
                outPut << "static ";
            }
            if (attrs & FIELD_ATTRIBUTE_INIT_ONLY) {
                outPut << "readonly ";
            }
        }
        if (attrs & FIELD_ATTRIBUTE_LITERAL && isEnum) {
            uint64_t val = 0;
            Il2Cpp::GetStaticFieldValueMan(field, &val);
            outPut << " = " << std::dec << val;
        }

        auto fixName = GetBetterName(Il2Cpp::GetClassName(field_class));

        outPut << fixName << " " << fieldName << "; // 0x" << std::hex << Il2Cpp::GetFieldOffset(field) << "\n";
    }

    return outPut.str();
}

std::string dumpMethods(void *klass) {
    std::stringstream outPut;
    void *iter = nullptr;

    while (auto method = Il2Cpp::ClassGetMethods(klass, &iter)) {
        auto methodName = Il2Cpp::GetMethodName(method);
        auto returnType = Il2Cpp::GetMethodReturnType(method);
        auto classFromType = Il2Cpp::ClassFromType(const_cast<void *>(returnType));
        auto returnTypeName = Il2Cpp::GetClassName(classFromType);
        auto fixedName = GetBetterName(returnTypeName);

        auto methodPointer = *(void**) method;

        auto va = reinterpret_cast<uint64_t>(methodPointer);
        uint64_t offset = va - (uint64_t)map.startAddress;

        uint32_t iflags = 0;
        auto flags = Il2Cpp::GetMethodFlags(method, &iflags);
        auto args = Il2Cpp::GetMethodArgs(method);

        outPut << "\t\t// VA: 0x" << std::hex << va
               << " | RVA: 0x" << std::hex << offset << "\n\t\t"
               << getMethodModifier(flags) << fixedName << " "
               << methodName << "(" << args << ")" << "\n\t\t{ \n \n \t\t} \n \n";
    }

    return outPut.str();
}

const char *GetPackageName() {
    char *application_id[256];
    FILE *fp = fopen("proc/self/cmdline", "r");
    if (fp) {
        fread(application_id, sizeof(application_id), 1, fp);
        fclose(fp);
    }
    return (const char *) application_id;
}
string dumpImage(/*const char* imageName*/const void* image){
    stringstream output;
    //auto image = Il2Cpp::GetImageByName(imageName);
    auto classCount = Il2Cpp::GetClassCount(image);
    LOGI("CLASS COUNT: %i", classCount);
    for (int j = 0; j < classCount; ++j) {
        auto klass = Il2Cpp::GetClassAtCount(image, j);
        LOGI("CLASS COUNT: %p", klass);
        auto name = const_cast<char *>(Il2Cpp::GetClassName(const_cast<void *>(klass)));
        LOGI("CLASS NAME: %s", name);
        auto nameSpace = const_cast<char *>(Il2Cpp::GetClassNamespace(const_cast<void *>(klass)));
        LOGI("CLASS NAMESPACE: %s", nameSpace);
        auto methods = dumpMethods(const_cast<void *>(klass));
        auto fields = dumpFields(const_cast<void *>(klass));
        auto properties = dumpProperties(const_cast<void *>(klass));

        output << "\n \nnamespace " << nameSpace << "\n{\n \tclass " << name << "\n\t{" << "\n"
               << methods.c_str() << "\n\n" << fields.c_str() << "\n" << properties.c_str()
               << "\n \t} \n }";
    }

    output << "\n\n";
    return output.str();
}

vector<const void*> images;

std::stringstream output;

void Dump() {

    output << "Dump: \n \n \n";

    /*LOGI("Dumping %zu images...", images.size());
    for (int i = 0; i < images.size(); ++i) {
        LOGI("Image %d pointer: %p", i, images[i]);
        output << dumpClass(images2[i]);
    }*/

    auto directory = std::string("/storage/emulated/0/Android/data/").append(
            GetPackageName()).append("/dump.cs");

    auto outputPath = std::string(directory);
    std::ofstream outStream(outputPath);
    outStream << output.str();
    outStream.close();
}

void* (*orig_il2cpp_class_from_name)(const void* image, const char* ns, const char* name);

void* my_il2cpp_class_from_name(const void* image, const char* ns, const char* name) {
    LOGI("il2cpp_class_from_name called! image: %p, ns: %s, class: %s", image, ns, name);

    if (!std::count(images.begin(), images.end(), image)) {
        images.push_back(image);
        output << dumpImage(image);
    }

    return orig_il2cpp_class_from_name(image, ns, name);
}

__attribute__ ((constructor))
void lib_main() {
    std::thread([]() {
        do {
            map = KittyMemory::getElfBaseMap("libil2cpp.so");
            sleep(1);
        } while (!map.isValid() && !map.isValidELF());

        LOGI("Got Base Start address: %p End address %p Offset: %p",(void *)map.startAddress, (void *)map.endAddress, (void *)map.offset);

        if (Il2Cpp::Init("libil2cpp.so") == -1) {
            LOGE("Il2Cpp::Init Failed!");
            return;
        }

        void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
        if (handle) {
            void* target_func = dlsym(handle, "il2cpp_class_from_name");
            if (target_func) {
                DobbyHook(target_func, reinterpret_cast<dobby_dummy_func_t>(my_il2cpp_class_from_name), reinterpret_cast<dobby_dummy_func_t*>(&orig_il2cpp_class_from_name));
                LOGI("Hooked il2cpp_class_from_name successfully!");
            } else {
                LOGI("Failed to find il2cpp_class_from_name symbol");
            }
        } else {
            LOGI("Failed to load libil2cpp.so");
        }


        sleep(10);
        Dump();
    }).detach();
}
