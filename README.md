```shell
git submodule update --init --recursive
cmake --preset=base # change preset
```


example user preset
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