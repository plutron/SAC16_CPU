cmake_minimum_required(VERSION 3.16)
project(SAC16 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# 1. تعیین مسیر خروجی (باید قبل از add_executable باشد)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 2. اضافه کردن پوشه include تا فایل‌های هدر پیدا شوند
include_directories(${CMAKE_SOURCE_DIR}/include)

# 3. جمع آوری خودکار تمام فایل های .c در ریشه و داخل پوشه src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.c")

# 4. جمع آوری خودکار فایل های .h برای نمایش بهتر در VS Code
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/include/*.h")

# 5. ساختن فایل اجرایی
add_executable(sac16 pass1.c ${SOURCES} ${HEADERS})

# 6. فعال کردن هشدارها
target_compile_options(sac16 PRIVATE -Wall -Wextra -Wpedantic)