# Setup

## Installation

## Visual Studio

Das Kit Desktopentwicklung mit C++

### llvm

https://releases.llvm.org/download.html

LLVM/bin/ zur PATH Variable

### ninja

https://github.com/ninja-build/ninja/releases
PATH Variable so einstellen, dass man ninja.exe finden kann

### cmake

https://cmake.org/download/
PATH Variable so einstellen, dass man cmake.exe finden kann

### vcpkg

https://github.com/microsoft/vcpkg#quick-start-windows

Commands
```
vcpkg install boost:x64-windows
vcpkg install eigen3:x64-windows
vcpkg install openimageio:x64-windows
vcpkg install spdlog:x64-windows
```

### vscode

Extensions:

- clangd
- cmake
- cmake-tools
- C++ TestMate
- C++ Helper
- CodeLLDB
- C++ Assistant
- vscode-icons
- C/C++ Snippets Pro
- Test Explorer UI


Settings:

"<project dir>/.vscode/cmake-kits.json
```
[
{
    "name": "Clang vcpkg",
    "preferredGenerator": {
      "name": "Ninja"
    },
    "visualStudio": <visual studio id>,
    "visualStudioArchitecture": "x64",
    "toolchainFile": "...\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake",
    "compilers": {
        "C": ".../LLVM/bin/clang-cl.exe",
        "CXX": ".../LLVM/bin/clang-cl.exe"
      }
}
]
```

Wobei man die *visual studio id* herausbekommt, wenn man die exe aus führt in:
"C:\Users\<Benutzer>\.vscode\extensions\ms-vscode.cmake-tools-1.5.3\res\vswhere.exe" 

Direkt die erste Zeile. Z.B. :
```
instanceId: 1eebc4c3
```

settings.json
```
{
"cmake.cmakePath": "...CMake\\bin\\cmake.exe",

}
```
