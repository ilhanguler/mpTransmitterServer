cmake_minimum_required(VERSION 3.30)


# Enable C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable the vcpkg toolchain
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/zzzzilhan/programlar/vcpkg-2024.09.30/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Add executable target
add_executable(mpTransmitterServer 
    "mpTransmitterServer.cpp" 
    "mpTransmitterServer.h" 
    "Windows/Networking_W.cpp" 
    "Windows/Networking_W.h" 
    "Linux/Networking.cpp" 
    "Linux/Networking.h" 
    "Windows/HTTP_R_Handlers_W.cpp" 
    "Windows/HTTP_R_Handlers_W.h" 
    "Windows/Session_Control_W.h" 
    "Windows/Session_Control_W.cpp" 
    
)

# Find Poco components using vcpkg integration
find_package(Poco REQUIRED COMPONENTS Foundation Net Util Data DataSQLite DataODBC)

# Link Poco libraries
target_link_libraries(mpTransmitterServer PRIVATE 
    Poco::Foundation 
    Poco::Net 
    Poco::Util 
    Poco::Data 
    Poco::DataSQLite 
)

message("Your CMAKE Version is: ${CMAKE_VERSION}")
