# CMakeList.txt : CMake project for mpTransmitterServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (mpTransmitterServer "mpTransmitterServer.cpp" "mpTransmitterServer.h" )

# TODO: Add tests and install targets if needed.

include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()