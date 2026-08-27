cmake_minimum_required(VERSION 3.20)

project(MyCppProject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB VID_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vid/*.cpp"
)

file(GLOB UTI_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp"
)

file(GLOB AUD_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/audio/*.cpp"
)

set(ALL_SOURCES
    ${SRC_FILES}
    ${VID_FILES}
    ${UTI_FILES}
    ${AUD_FILES}
)

add_executable(main ${ALL_SOURCES})


find_package(PkgConfig REQUIRED)

# VLC
pkg_check_modules(VLC REQUIRED IMPORTED_TARGET libvlc)

# SDL3
pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)

#PortAudio
pkg_check_modules(PORTAUDIO REQUIRED IMPORTED_TARGET portaudio-2.0)

#libsndfile
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)

#kissfft
pkg_check_modules(KISSFFT REQUIRED IMPORTED_TARGET kissfft-float)

target_link_libraries(main PRIVATE
    PkgConfig::VLC
    PkgConfig::SDL3
    PkgConfig::PORTAUDIO
    PkgConfig::SNDFILE
    PkgConfig::KISSFFT
    stdc++exp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(main PRIVATE
        -fdiagnostics-color=always
        -O3
        )

    set_property(TARGET main PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

endif()
