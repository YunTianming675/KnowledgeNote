# 01. 基本配置

## 01.1 基本工程

几基本工程指单文件工程，仅有一个 C/C++ 文件。CMake 指令支持大写、小写和大小写混合，但**优先考虑使用小写指令**

- `cmake_minimum_required()`: 指定 CMake 的最小版本，应该在 CMakeLists.txt 的开头第一行指令
- `project()`: 每个工程都必须配置，且紧随在 `cmake_minimum_required()` 之后，用于配置工程的名字。此外，还可以指定其它项目级别的信息，如语言和版本号。配置的名称被存储于变量 `PROJECT_NAME` 中
- `add_executable()`: 告诉 CMake 通过哪些源文件来创建可执行文件

工程目录结构如下：

```
$ tree .
.
├── CMakeLists.txt
├── step1_build
└── tutorial.cxx
```

CMakeLists.txt 文件如下：

```cmake
cmake_minimum_required(VERSION 3.0)
project(step1)
add_executable(${PROJECT_NAME} tutorial.cxx)
```

tutorial.cxx 文件内容如下：

```c++
// A simple program that computes the square root of a number
#include <cmath>
#include <cstdlib> // TODO 5: Remove this line
#include <iostream>
#include <string>
#include "TutorialConfig.h"

// TODO 11: Include TutorialConfig.h

int main(int argc, char* argv[])
{
  if (argc < 2) {
    // TODO 12: Create a print statement using Tutorial_VERSION_MAJOR
    //          and Tutorial_VERSION_MINOR
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }

  // convert input to double
  // TODO 4: Replace atof(argv[1]) with std::stod(argv[1])
  // const double inputValue = atof(argv[1]);
  const double inputValue = std::stod(argv[1]);

  // calculate square root
  const double outputValue = sqrt(inputValue);
  std::cout << "The square root of " << inputValue << " is " << outputValue
            << std::endl;
  return 0;
}
```



- `step1_build` 存放构建过程文件及生成的程序。在执行 `cmake` 命令前，应该先通过如 cmake-gui 等程序对工程进行配置，这个配置主要控制生成的 Makefile 属于哪个编译平台下，执行配置完成后的工程目录如下：

```
$ tree .
.
├── CMakeLists.txt
├── step1_build
│   ├── CMakeCache.txt
│   └── CMakeFiles
│       ├── 3.17.0-rc3
│       │   ├── CMakeCCompiler.cmake
│       │   ├── CMakeCXXCompiler.cmake
│       │   ├── CMakeDetermineCompilerABI_C.bin
│       │   ├── CMakeDetermineCompilerABI_CXX.bin
│       │   ├── CMakeRCCompiler.cmake
│       │   ├── CMakeSystem.cmake
│       │   ├── CompilerIdC
│       │   │   ├── a.exe
│       │   │   ├── CMakeCCompilerId.c
│       │   │   └── tmp
│       │   └── CompilerIdCXX
│       │       ├── a.exe
│       │       ├── CMakeCXXCompilerId.cpp
│       │       └── tmp
│       ├── cmake.check_cache
│       ├── CMakeOutput.log
│       └── CMakeTmp
└── tutorial.cxx
```

- 之后在 `step1_build` 目录中执行 `cmake ..` 生成 Makefile，再通过 `cmake --build .` 或 `make` 指令生成可执行程序

## 01.2 指定 C/C++ 标准

有时候需要指定 C/C++ 的标准以确保编译器正确使用新特性

- `CMAKE_CXX_STANDARD`: 该变量指定所需的 C++ 标准等级
- `CMAKE_CXX_STANDARD_REQUIRED`: 该变量声明所需的 C++ 标准等级是否需要

> 以 `CMAKE_` 开头的变量代表 CMake 内部所使用的变量，如果需要自定义变量使用，应避免以 `CMAKE_` 开头

- 添加 C/C++ 标准后的 CMakeLists.txt 如下：

```cmake
cmake_minimum_required(VERSION 3.0)
project(step1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} tutorial.cxx)
```

> 注意：标准的设定必须先于 `add_executable()` 指令

## 01.3 变量传递

有时候让在 CMakeLists.txt 中定义的变量在头文件中可用是有意义的

- `project()`: 除设置工程名之外，还能设置版本信息
- `<PROJECT-NAME>_VERSION_MAJOR`: 设置的版本信息的第一个数字
- `<PROJECT-NAME>_VERSION_MINOR`: 设置的版本信息的第二个数字
- `configure_file()`: 将输入文件复制到输出文件，会在复制过程中将输入文件的 `@VAR@` 或 `${VAR}` 进行替换
- `target_include_directories()`: 添加目标的头文件包含目录，需要在 `add_executable()` **之后调用**

在此节中，添加了新文件 TutorialConfig.h.in，添加后的目录及文件结构如下：

```
$ tree .
.
├── CMakeLists.txt
├── step1_build
├── TutorialConfig.h.in
└── tutorial.cxx
```

- 同样，在执行 cmake 命令前，需要先使用如 cmake-gui 等工具进行配置

同样是 01. 中提到的目录及文件结构，修改后的 CMakeLists.txt 如下：

```cmake
cmake_minimum_required(VERSION 3.0)
project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

add_executable(${PROJECT_NAME} tutorial.cxx)
target_include_directories(Tutorial PUBLIC "${PROJECT_BINARY_DIR}")
```

- `configure_file()` 的第一个参数是输入文件，第二个文件是输出文件位置，默认生成于执行 cmake 指令时所在的目录，在本例中，为 `step1_build`

添加的 TutorialConfig.h.in 文件内容如下：

```c
// the configured options and settings for Tutorial
// TODO 10: Define Tutorial_VERSION_MAJOR and Tutorial_VERSION_MINOR
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
```

生成的 TutorialConfig.h 文件如下：

```c++
// the configured options and settings for Tutorial
// TODO 10: Define Tutorial_VERSION_MAJOR and Tutorial_VERSION_MINOR
#define Tutorial_VERSION_MAJOR 1
#define Tutorial_VERSION_MINOR 0
```

同时，还需要对 tutorial.cxx 文件的内容进行一些修改，修改如下：

```c++
// A simple program that computes the square root of a number
#include <cmath>
#include <cstdlib> // TODO 5: Remove this line
#include <iostream>
#include <string>
#include "TutorialConfig.h"

// TODO 11: Include TutorialConfig.h

int main(int argc, char* argv[])
{
  if (argc < 2) {
    // TODO 12: Create a print statement using Tutorial_VERSION_MAJOR
    //          and Tutorial_VERSION_MINOR
	std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "."
              << Tutorial_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }

  // convert input to double
  // TODO 4: Replace atof(argv[1]) with std::stod(argv[1])
  // const double inputValue = atof(argv[1]);
  const double inputValue = std::stod(argv[1]);

  // calculate square root
  const double outputValue = sqrt(inputValue);
  std::cout << "The square root of " << inputValue << " is " << outputValue
            << std::endl;
  return 0;
}
```

# 02. 添加库

本节介绍如何创建和使用库，并介绍如何让使用库成为可选项

## 02.1 创建库

- `add_library()`: 该指令用于从指定的源文件中创建库

- `add_subdirectory()`: 添加一个生成的子目录

本工程中建立了一个名为 MathFunctions 的文件夹来创建库文件，并在其中创建了一个子 CMakeLists.txt，其初始目录和文件树如下：

```
$ tree .
.
├── build
├── CMakeLists.txt
├── MathFunctions
│   ├── CMakeLists.txt
│   ├── MathFunctions.cxx
│   ├── MathFunctions.h
│   ├── mysqrt.cxx
│   └── mysqrt.h
├── TutorialConfig.h.in
└── tutorial.cxx
```

- 文件 CMakeLists.txt 的内容

```cmake
cmake_minimum_required(VERSION 3.0)
project(step2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

add_subdirectory(MathFunctions)
add_executable(${PROJECT_NAME} tutorial.cxx)

target_link_libraries(step2 PUBLIC MathFunctions)
target_include_directories(step2 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/MathFunctions"
)
```

- 文件 MathFunctions/CMakeLists.txt 的内容

```cmake
add_library(MathFunctions mysqrt.cxx)
```

> 注意：创建库的操作由子 CMakeLists.txt 完成

- 文件 MathFunctions/MathFunctions.cxx 的内容

```c++
#include "MathFunctions.h"
#include "mysqrt.h"

namespace mathfunctions {
double sqrt(double x)
{
  return detail::mysqrt(x);
}
}
```

- 文件 MathFunctions/MathFunctions.h 的内容

```c++
#pragma once

namespace mathfunctions {
double sqrt(double x);
}
```

- 文件 MathFunctions/mysqrt.cxx 的内容

```c++
#include "mysqrt.h"

#include <iostream>

namespace mathfunctions {
namespace detail {
// a hack square root calculation using simple operations
double mysqrt(double x)
{
  if (x <= 0) {
    return 0;
  }

  double result = x;

  // do ten iterations
  for (int i = 0; i < 10; ++i) {
    if (result <= 0) {
      result = 0.1;
    }
    double delta = x - (result * result);
    result = result + 0.5 * delta / result;
    std::cout << "Computing sqrt of " << x << " to be " << result << std::endl;
  }
  return result;
}
}
}
```

- 文件 MathFunctions/mysqrt.h 中的内容

```c++
#pragma once

namespace mathfunctions {
namespace detail {
double mysqrt(double x);
}
}
```

- 文件 TutorialConfig.h,in 的内容

```c++
#define Tutorial_VERSION_MAJOR @step2_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @step2_VERSION_MINOR@
```

- 文件 tutorial.cxx 的内容

```c++
#include <cmath>
#include <iostream>
#include <string>

#include "TutorialConfig.h"
#include "MathFunctions.h"
#include "mysqrt.h"

int main(int argc, char* argv[])
{
  if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }

  // const double inputValue = std::stod(argv[1]);
  const double inputValue = std::stod(argv[1]);

  const double outputValue = mathfunctions::detail::mysqrt(inputValue);
  std::cout << "The square root of " << inputValue << " is " << outputValue << std::endl;
  return 0;
}
```

执行命令：

```
cd build
cmake ..
cmake --build .
```

- 构建的库位于 `./build/MathFunctions/libMathFunctions.a` 

## 02.2 让创建库可选

对于大型工程来说，需要创建一个库，虽然在本示例中，创建库是没有必要的

- `option()`: 提供一个布尔类型的变量以为后续操作进行选择，布尔值为 `ON` 或 `OFF`，注意，变量的值会存储到 CMake 缓存中，每次构建都会使用缓存中的值，除非修改值后再次使用 `cmake` 指令生成
- `list()`: 列表操作，该指令具备一些子命令
- `if()`: 条件判断，格式如下

```cmake
if(<condition>)
  <commands>
elseif(<condition>) # optional block, can be repeated
  <commands>
else()              # optional block
  <commands>
endif()
```

- `#cmakedefine`: 配合 `configure_file()` 一起使用，在输入文件中的 `#cmakedefine VAR ...` 会在输出文件中被替换为 `#define VAR ...`

工程目录与上一小节相同，操作如下：

1. 在顶层的 CMakeLists.txt 中添加 `option()` 指令：

```cmake
option(USE_MYMATH "Use tutorial provided math implementation" ON)
```

- 第一个参数为变量名称，第二个参数为帮助信息，第三个参数为变量的值

2. 在顶层的 CMakeLists.txt 中对变量值进行判断：

```cmake
if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
  list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif()
```

- 这里使用了 `list()` 来控制库列表和包含列表，这样做的好处是对于工程来说，可以将可选内容加进这个列表，随后再通过这个列表统一进行构建

3. 由于使用了变量来控制库列表，所以在 `target_link_libraries()` 中需要修改参数，将直接指定改为由变量指定

```cmake
target_link_libraries(step2 PUBLIC ${EXTRA_LIBS})
```

4. 同样，修改 `target_include_directories()`

```cmake
target_include_directories(step2 PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          ${EXTRA_INCLUDES}
)
```

5. 修改 tutorial.cxx，使用条件编译

```c++
#ifdef USE_MYMATH
  #include "MathFunctions.h"
  #include "mysqrt.h"
#endif
```

6. 同样在 tutorial.cxx 中，通过条件编译选择使用的函数

```c++
#ifdef USE_MYMATH
  const double outputValue = mathfunctions::detail::mysqrt(inputValue);
#else
  const double outputValue = sqrt(inputValue);
#endif
```

7. 由于使用了 `configure_file()` 来控制文件，需要修改 TutorialConfig.h.in 的内容，添加宏

```c++
#cmakedefine USE_MYMATH
```

可以通过命令行修改变量的值：

- 将变量改为 ON: `cmake .. -DUSE_MYMATH=ON`
- 将变量改为 OFF: `cmake .. -DUSE_MYMATH=OFF`
- cmake 指令后的第一个参数代表顶层 CMakeLists.txt 相对于执行命令时终端所在的目录

`USE_MYMATH=ON` 时生成的 TutorialConfig.h

```c++
#define Tutorial_VERSION_MAJOR 1
#define Tutorial_VERSION_MINOR 0
#define USE_MYMATH
```

`USE_MYMATH=OFF` 时生成的 TutorialConfig.h

```c++
#define Tutorial_VERSION_MAJOR 1
#define Tutorial_VERSION_MINOR 0
/* #undef USE_MYMATH */
```

### 完整的文件内容

- 仅列出同上一小节相比改动的文件
- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.0)
project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_MYMATH "Use tutorial provided math implementation" ON)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
  list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif()

add_executable(${PROJECT_NAME} tutorial.cxx)

target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})
target_include_directories(Tutorial PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          ${EXTRA_INCLUDES}
)
```

- tutorial.cxx

```c++
#include <cmath>
#include <iostream>
#include <string>

#include "TutorialConfig.h"
#ifdef USE_MYMATH
  #include "MathFunctions.h"
  #include "mysqrt.h"
#endif

int main(int argc, char* argv[])
{
  if (argc < 2) {
    // report version
    std::cout << argv[0] << " Version " << Tutorial_VERSION_MAJOR << "." << Tutorial_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " number" << std::endl;
    return 1;
  }

  // const double inputValue = std::stod(argv[1]);
  const double inputValue = std::stod(argv[1]);

#ifdef USE_MYMATH
  const double outputValue = mathfunctions::detail::mysqrt(inputValue);
#else
  const double outputValue = sqrt(inputValue);
#endif
  std::cout << "The square root of " << inputValue << " is " << outputValue << std::endl;
  return 0;
}
```

- TutorialConfig.h.in

```c++
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
#cmakedefine USE_MYMATH
```

# 03. 添加库的使用要求

通过定义库的使用要求，在顶层 CMakeLists.txt 中只需要链接至库而不需要额外添加任何目录。在 02 中介绍的添加库的方法为经典方法，本节介绍的方法更加现代化，因为工程中的库多了之后，手动指定每个库的依赖是相当困难和麻烦的

## 03.1 操作步骤

- 在 02.2 的基础上进行

1. 在 MathFunctions/CMakeLists.txt 中添加如下内容

```cmake
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
```

- `INTERFACE` 关键字表示将后面的目录加入 `INTERFACE_INCLUDE_DIRECTORIES` 变量中，该变量表示库的包含目录列表

2. 在 CMakeLists.txt 中移除 `EXTRA_INCLUDES` 变量

```cmake
if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()
```

3. 在 CMakeLists.txt 中移除 `target_include_directories()` 对 `EXTRA_INCLUDES` 变量的使用

```cmake
target_include_directories(step3 PUBLIC
                          "${PROJECT_BINARY_DIR}"
)
```

### 完成的文件内容

- 仅列出修改的文件
- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.0)
project(Tutorial VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_MYMATH "Use tutorial provided math implementation" ON)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()

add_executable(${PROJECT_NAME} tutorial.cxx)

target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})
target_include_directories(Tutorial PUBLIC
                          "${PROJECT_BINARY_DIR}"
)
```

- MathFunctions/CMakeLists.txt

```cmake
add_library(MathFunctions mysqrt.cxx)
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
```

# 04. 添加生成表达式

生成表达式在生成系统期间求值，以生成特定于每个生成配置的信息

## 04.1 用接口库设置 C++ 标准

- 设置接口库的 C++ 标准，对于需要使用该标准的目标则用 `target_link_libraries()` 进行链接与指定。通过这种方法可以对特定的生成目标指定生成时的 C/C++ 标准

1. 删除 CMakeLists.txt 中变量 `CMAKE_CXX_STANDARD` 和 `CMAKE_CXX_STANDARD_REQUIRED` 的设置
2. 建立接口库，命名为 `tutorial_compiler_flags`，接口库（interface library），在 CMakeLists.txt 中进行修改

```cmake
add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_11)
```

3. 将生成目标与接口库进行链接，在 CMakeLists.txt 中进行修改

```cmake
target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
```

在 MathFunctions/CMakeLists.txt 中进行修改

```cmake
target_link_libraries(MathFunctions tutorial_compiler_flags)
```

### 完成的文件内容

- 仅列出修改的文件
- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.0)
project(Tutorial VERSION 1.0)

option(USE_MYMATH "Use tutorial provided math implementation" ON)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()

add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_11)

add_executable(${PROJECT_NAME} tutorial.cxx)

target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
target_include_directories(Tutorial PUBLIC
                          "${PROJECT_BINARY_DIR}"
)
```

- MathFunctions/CMakeLists.txt

```cmake
add_library(MathFunctions mysqrt.cxx)
target_link_libraries(MathFunctions tutorial_compiler_flags)
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
```

## 04.2 在生成中添加警告信息

- **所使用的生成器表达式在 3.15 版本引入**

1. 修改所需最小 CMake 版本为 3.15

```cmake
cmake_minimum_required(VERSION 3.15)
```

2. 设置变量保存当前的编译器信息

```cmake
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
```

3. 添加项目所需要的编译器警告标志同时使用 `BUILD_INTERFACE` 关键字将警告限定为在构建过程中显示

```cmake
target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
```

### 完整的文件内容

- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(Tutorial VERSION 1.0)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

option(USE_MYMATH "Use tutorial provided math implementation" ON)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()

add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_11)

add_executable(${PROJECT_NAME} tutorial.cxx)

target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
target_include_directories(Tutorial PUBLIC
                          "${PROJECT_BINARY_DIR}"
)
```

# 05. 安装与测试

有时候仅生成可执行文件是不够的，还需要使生成结果可安装

## 05.1 设置安装规则

在上一节的基础上进行修改，目标是将生成的库安装到 `lib`，头文件安装到 `include`，可执行文件安装到 `bin` 中

1. 修改 MathFunctions/CMakeLists.txt

```cmake
set(installable_libs MathFunctions)
install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES MathFunctions.h DESTINATION include)
```

- 注：`install()` 指令需要在 `add_library()` 之后

2. 修改 CMakeLists.txt

```cmake
install(TARGETS Tutorial DESTINATION bin)
install(FILES "TutorialConfig.h"
  DESTINATION include
)
```

- 在构建后即可执行 `cmake --install .` 进行安装，但要注意，在 Linux 中需要 root 权限，可以通过 `--prefix` 选项来重新定义安装位置，例如

```bash
cmake --install . --prefix ~/Projects
```

- 即可在 ~/Projects 下看见三个文件夹：`bin`、`include`、`lib`

> CMake 使用变量 `CMAKE_INSTALL_PREFIX` 来安装时的根目录在哪，`--prefix` 选项即会重写该变量

### 完整的文件内容

- CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(Tutorial VERSION 1.0)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

option(USE_MYMATH "Use tutorial provided math implementation" ON)

configure_file(TutorialConfig.h.in ..\\TutorialConfig.h)

if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()

add_library(tutorial_compiler_flags INTERFACE)
target_compile_features(tutorial_compiler_flags INTERFACE cxx_std_11)

add_executable(${PROJECT_NAME} tutorial.cxx)

target_compile_options(tutorial_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
target_include_directories(Tutorial PUBLIC
                          "${PROJECT_BINARY_DIR}"
)

install(TARGETS Tutorial DESTINATION bin)
install(FILES "TutorialConfig.h"
  DESTINATION include
)
```

- MathFunctions/CMakeLists.txt

```cmake
add_library(MathFunctions mysqrt.cxx)

set(installable_libs MathFunctions)

target_link_libraries(MathFunctions tutorial_compiler_flags)
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES MathFunctions.h DESTINATION include)
```

## 05.2 测试支持

CTest 提供了一个简单的方法管理工程中的测试。要添加测试可以通过 `add_test()` 进行添加。CTest 与其它测试框架如 GoogleTest 之间有很好的兼容性。

- `enable_testing()`: 为当前目录及子目录启用测试
- `add_test()`: 添加一个测试到项目中，该测试需要由 `ctest` 来运行
- `PASS_REGULAR_EXPRESSION`: 输出内容必须匹配该正则表达式才能通过测试，程序的退出代码将被忽略
- `function()`: 设置自定义的函数（指令），在设置后可以调用

1. 在 CMakeLists.txt 最后添加启用测试的指令

```cmake
enable_testing()
```

2. 启用测试后，先添加一些基本测试来验证程序是否正常工作。首先，使用 `add_test()` 创建一个测试，它运行生成的可执行文件，并传入参数 25，但是不检查结果是否正确，此测试仅为了验证应用程序是否运行，没有出现段错误等其它崩溃的情况，并且返回值为 0，这是 CTest 的基本形式

```cmake
add_test(NAME Runs COMMAND Tutorial 25)
```

3. 使用 `PASS_REGULAR_EXPRESSION` 测试属性来验证测试的输出中是否包含某些字符串。在本例中，验证当提供的参数数量不正确时是否打印消息

```cmake
add_test(NAME Usage COMMAND Tutorial)
set_tests_properties(Usage
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
)
```

4. 验证成否是否正确计算了输入数值的平方根

```cmake
add_test(NAME StandardUse COMMAND Tutorial 4)
set_tests_properties(StandardUse
  PROPERTIES PASS_REGULAR_EXPRESSION "4 is 2"
)
```

5. 如果需要添加大量测试，可以通过 `function()` 指令来避免很多重复代码

```cmake
function(do_test target arg result)
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})
  set_tests_properties(Comp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()

# do a bunch of result based tests
do_test(Tutorial 4 "4 is 2")
do_test(Tutorial 9 "9 is 3")
do_test(Tutorial 5 "5 is 2.236")
do_test(Tutorial 7 "7 is 2.645")
do_test(Tutorial 25 "25 is 5")
do_test(Tutorial -25 "-25 is (-nan|nan|0)")
do_test(Tutorial 0.0001 "0.0001 is 0.01")
```

- 最终在 `build` 目录下执行 `ctest` 来执行设置的所有测试：

```
$ ctest
Test project /home/dlans/Projects/CMake/step05/build
      Start  1: Runs
 1/10 Test  #1: Runs .............................   Passed    0.00 sec
      Start  2: Usage
 2/10 Test  #2: Usage ............................   Passed    0.00 sec
      Start  3: StandardUse
 3/10 Test  #3: StandardUse ......................   Passed    0.00 sec
      Start  4: Comp4
 4/10 Test  #4: Comp4 ............................   Passed    0.00 sec
      Start  5: Comp9
 5/10 Test  #5: Comp9 ............................   Passed    0.00 sec
      Start  6: Comp5
 6/10 Test  #6: Comp5 ............................   Passed    0.00 sec
      Start  7: Comp7
 7/10 Test  #7: Comp7 ............................   Passed    0.00 sec
      Start  8: Comp25
 8/10 Test  #8: Comp25 ...........................   Passed    0.00 sec
      Start  9: Comp-25
 9/10 Test  #9: Comp-25 ..........................   Passed    0.00 sec
      Start 10: Comp0.0001
10/10 Test #10: Comp0.0001 .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 10

Total Test time (real) =   0.01 sec
```

### 完整的文件内容

