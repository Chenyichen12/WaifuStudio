### WaifuStudio

一个Live2D编辑器，使用Qt6框架制作

现已完成基本的网格编辑，开始变形器的制作

```shell
git submodule update --init --recursive
cmake --preset=base # change preset
```


CMakeUserPresets示例
```json
{
 "version": 3,
 "configurePresets": [
  {
   "name": "my-windows",
   "inherits": [ "windows" ],
   "cacheVariables": { "CMAKE_TOOLCHAIN_FILE": "D:/vcpkg/scripts/buildsystems/vcpkg.cmake",
   "CMAKE_C_COMPILER":"cl.exe",
   "CMAKE_CXX_COMPILER":"cl.exe"}
  },{
    "name": "windows-mingw",
    "inherits":["my-windows"],
    "environment": {
      "QT_PATH": "C:/Qt/6.6.3/mingw_64"
    },
    "cacheVariables": {
      "CMAKE_C_COMPILER": "C:/msys64/mingw64/bin/gcc.exe",
      "CMAKE_CXX_COMPILER": "C:/msys64/mingw64/bin/g++.exe",
      "VCPKG_TARGET_TRIPLET": "x64-mingw-dynamic",
      "CMAKE_PREFIX_PATH":"C:/Qt/6.6.3/mingw_64"
    }
  }
 ]
}
```