cmake_minimum_required(VERSION 3.3.0)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/macros)

project(pcsxr)

set(PCSXR_VERSION_MAJOR "1")
set(PCSXR_VERSION_MINOR "9")
set(PCSXR_VERSION_PATCH "94")
add_definitions(-DPACKAGE_VERSION="${PCSXR_VERSION_MAJOR}.${PCSXR_VERSION_MINOR}.${PCSXR_VERSION_PATCH}")
add_definitions(-DPACKAGE_NAME="PCSXr")
add_definitions(-DPACKAGE_STRING="PCSXr ${PCSXR_VERSION_MAJOR}.${PCSXR_VERSION_MINOR}.${PCSXR_VERSION_PATCH}")


include(CheckCCompilerFlag)
include(GNUInstallDirs)

include_directories(${CMAKE_SOURCE_DIR})

add_definitions(-DENABLE_NLS -DGETTEXT_PACKAGE="pcsxr")

#WARNING! this is required for dynarec to work!
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-pie")
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    CHECK_C_COMPILER_FLAG("-no-pie" NO_PIE_UPSTREAM)
    if(NO_PIE_UPSTREAM)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    endif()
    CHECK_C_COMPILER_FLAG("-nopie" NO_PIE_PATCHED)
    if(NO_PIE_PATCHED)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nopie")
    endif()
endif()

#components
add_subdirectory(libpcsxcore)
add_subdirectory(gui)
add_subdirectory(plugins)
add_subdirectory(doc)



