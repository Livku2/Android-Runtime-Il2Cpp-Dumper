## Android Runtime Il2cpp Dumper

A runtime dumper for android

### Using the mod
You have to decompile the apk, then go to smali/com/unity3d/player and open UnityPlayerActivity.smali

then go to OnCreate and add this

```java
const-string v0, "LivkuDumper"

invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```
it should look something like this

![image](https://github.com/user-attachments/assets/cecc47d5-5905-4da5-aad4-1a4b285bb363)

### Changing the target assembly

1. Clone the repo with ``` git clone --recurse-submodules https://github.com/Livku2/Quest-Modding-Lib-Template](https://github.com/Livku2/Android-Runtime-Il2Cpp-Dumper ```
2. Open the project in Android Studio
4. Modify the string that says "Assembly-CSharp.dll" to the name of whatever assembly you want
5. Build the project
6. Copy the generated .so from `app/build/outputs/native/{debug/release}/lib{template}.so`


### Todo

- [ ] Dump all assemblies
- [ ] Fix Method Offsets
- [ ] Fix Formatting

### Credit

- [Zygisk Il2CppDumper](https://github.com/Perfare/Zygisk-Il2CppDumper) - Dumper I used as a reference
