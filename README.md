## Android Runtime Il2cpp Dumper

A runtime dumper for android

### Using the mod
You have to decompile the apk

Put the .so file in the lib/arm64-v8a folder

Then go to smali/com/unity3d/player and open UnityPlayerActivity.smali

Then go to OnCreate and add this

```java
const-string v0, "LivkuDumper"

invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```
It should look something like this

![image](https://github.com/user-attachments/assets/6c57ee92-b97d-4e31-921a-ac5fa135fe60)


### Changing the target assembly

1. Clone the repo with ``` git clone --recurse-submodules https://github.com/Livku2/Quest-Modding-Lib-Template](https://github.com/Livku2/Android-Runtime-Il2Cpp-Dumper ```
2. Open the project in Android Studio
4. Modify the string that says "Assembly-CSharp.dll" to the name of whatever assembly you want
5. Build the project
6. Copy the generated .so from `app/build/outputs/native/release/libLivkuDumper.so`


### Todo

- [ ] Dump all assemblies
- [x] Fix Method Offsets
- [ ] Fix Formatting


### Known Issues
Sometimes the lib may cause crashes, this is most likely caused by the GetImageByName function as il2cpp_domain_get_assemblies can return nullptr

I am still working on a fix for this but haven't found a work around just yet


Recently I have fixed this error, but another one came up so I am working on fixing that

### Credit

- [Zygisk Il2CppDumper](https://github.com/Perfare/Zygisk-Il2CppDumper) - Dumper I used as a reference
