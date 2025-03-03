#include <jni.h>
#include <string>
#include <thread>
#include <cstdint>
#include <vector>
#include "Engine/Includes.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

        ProcMap map;

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
            outPut << " " << Il2Cpp::GetClassName(propertyClass) << " " << propertyName << " { ";
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

        outPut << Il2Cpp::GetClassName(field_class) << " " << fieldName << "; // 0x" << std::hex << Il2Cpp::GetFieldOffset(field) << "\n";
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

        auto va = reinterpret_cast<uint64_t>(method);
        uint64_t offset = va - (uint64_t)map.startAddress;

        uint32_t iflags = 0;
        auto flags = Il2Cpp::GetMethodFlags(method, &iflags);
        auto args = Il2Cpp::GetMethodArgs(method);

        outPut << "\t\t// VA: 0x" << std::hex << va
               << " | Offset: 0x" << std::hex << offset << "\n\t\t"
               << getMethodModifier(flags) << " " << returnTypeName << " "
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

void Dump(){

    std::stringstream output;

    output << "Dump: \n \n \n";

    auto image = Il2Cpp::GetImageByName("Assembly-CSharp.dll");
    auto classCount = Il2Cpp::GetClassCount(image);
    for (int j = 0; j < classCount; ++j) {
        auto klass = Il2Cpp::GetClassAtCount(image, j);
        auto name = const_cast<char*>(Il2Cpp::GetClassName(const_cast<void*>(klass)));
        auto nameSpace = const_cast<char*>(Il2Cpp::GetClassNamespace(const_cast<void*>(klass)));
        auto methods = dumpMethods(const_cast<void*>(klass));
        auto fields = dumpFields(const_cast<void*>(klass));
        auto properties = dumpProperties(const_cast<void*>(klass));

        output << "\n \nnamespace " << nameSpace << "\n{\n \tclass " << name << "\n\t{" << "\n" << methods.c_str() <<"\n\n" << fields.c_str() << "\n" << properties.c_str() << "\n \t} \n }";
    }
    
    auto directory = std::string("/storage/emulated/0/Android/data/").append(
            GetPackageName()).append("/dump.cs");

    auto outputPath = std::string(directory);
    std::ofstream outStream(outputPath);
    outStream << output.str();
    outStream.close();
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
        Dump();
    }).detach();
}
