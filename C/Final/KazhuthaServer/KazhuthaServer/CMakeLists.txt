# CMakeList.txt : CMake project for KazhuthaServer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (KazhuthaServer "KazhuthaServer.c" "KazhuthaServer.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET KazhuthaServer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
# Add custom FindSDL2 module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Find SDL2 using custom FindSDL2.cmake
find_package(SDL2 REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(KazhuthaServer PRIVATE 
    SDL2::SDL2 SDL2::SDL2main 
    SDL2_net::SDL2_net 
    SDL2_ttf::SDL2_ttf
)