cmake_minimum_required(VERSION 3.13)

if (CMAKE_HOST_UNIX)
    set(CMAKE_SYSTEM_NAME Windows)
    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
    # cross compilers to use for C, C++ and RC
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
    set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

    # modify default behavior of FIND_XXX() commands
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(PKG_CONFIG_EXECUTABLE ${TOOLCHAIN_PREFIX}-pkg-config)
endif ()

set(MY_PROJECT_NAME MineSweep)
set(MY_TARGET_NAME MineSweep)

project(${MY_PROJECT_NAME} LANGUAGES CXX)

file(GLOB DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/MineSweep/*.cpp)

message("DEMO_SOURCE: ${DEMO_SOURCE}")

add_executable(${MY_TARGET_NAME} ${DEMO_SOURCE})

target_include_directories(${MY_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ege/include)

# 设置静态库搜索路径

if (MSVC)
    target_compile_options(${MY_TARGET_NAME} PRIVATE /source-charset:utf-8 /MP /std:c++17 /D_USE_MATH_DEFINES /D__STDC_LIMIT_MACROS "$<$<CONFIG:DEBUG>:/DDEBUG>" "$<$<CONFIG:RELEASE>:/DNDEBUG>" "$<$<CONFIG:RELWITHDEBINFO>:/DNDEBUG>" "$<$<CONFIG:MINSIZEREL>:/DNDEBUG>" "$<IF:$<CONFIG:Debug>,/MDd,/MD>")

    if (MSVC_VERSION GREATER_EQUAL 1920)
        # vs2019 以上, 静态库是兼容的.
        if (CMAKE_CL_64)
            set(osLibDir "vs2019/x64")
        else ()
            set(osLibDir "vs2019/x86")
        endif ()
    elseif (MSVC_VERSION GREATER_EQUAL 1910)
        # vs2017
        if (CMAKE_CL_64)
            set(osLibDir "vs2017/x64")
        else ()
            set(osLibDir "vs2017/x86")
        endif ()
    elseif (MSVC_VERSION GREATER_EQUAL 1900)
        # vs2015
        if (CMAKE_CL_64)
            set(osLibDir "vs2015/amd64")
        else ()
            set(osLibDir "vs2015")
        endif ()
    else ()
        message(FATAL_ERROR "你的 MSVC 版本太老了, 请使用 vs2015 或更新版本的 MSVC 编译器. 与时俱进吧, 不要一直使用十年前的编译器.")
    endif ()
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_HOST_APPLE)
        set(osLibDir "macOS")
    else ()
        set(osLibDir "mingw-w64-debian")
    endif ()
    target_compile_options(${MY_TARGET_NAME} PRIVATE -D_FORTIFY_SOURCE=0)
    target_link_options(${MY_TARGET_NAME} PRIVATE -mwindows -static -static-libgcc -static-libstdc++)
    target_link_libraries(${MY_TARGET_NAME} PRIVATE graphics64 gdiplus gdi32 imm32 msimg32 ole32 oleaut32 winmm uuid)
endif ()

if (DEFINED osLibDir)
    message(STATUS "osLibDir: ${osLibDir}")
    target_link_directories(${MY_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ege/lib/${osLibDir})
endif ()

file(GLOB GAME_RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/MineSweep/*.bmp
        ${CMAKE_CURRENT_SOURCE_DIR}/MineSweep/*.mp3
        ${CMAKE_CURRENT_SOURCE_DIR}/MineSweep/*.wav
)

foreach (RES_FILE ${GAME_RESOURCES})
add_custom_command(TARGET MineSweep POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RES_FILE} $<TARGET_FILE_DIR:MineSweep>
        COMMENT "Copying: ${RES_FILE} to $<TARGET_FILE_DIR:MineSweep>"
)
endforeach ()