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


### Todo

- [x] Dump all assemblies
- [x] Fix Method Offsets
- [x] Fix Formatting


### Known Issues
slight formatting problem but nothing huge, just a few spaces

### Credit

- [Zygisk Il2CppDumper](https://github.com/Perfare/Zygisk-Il2CppDumper) - Dumper I used as a reference
