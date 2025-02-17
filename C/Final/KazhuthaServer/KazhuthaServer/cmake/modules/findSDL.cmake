# FindSDL.cmake - A custom CMake module to find SDL2, SDL_net, and SDL_ttf on Windows

# This module will attempt to find SDL2, SDL_net, and SDL_ttf manually on Windows.

# Set SDL2_ROOT_PATH to a path that contains the SDL2 libraries (if needed)
# Example: C:/path/to/SDL2, or the location where SDL2 is installed

find_path(SDL2_INCLUDE_DIR SDL.h
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/include" "C:/Program Files (x86)/SDL2/include"
)

find_library(SDL2_LIBRARY NAMES SDL2 SDL2d
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/lib" "C:/Program Files (x86)/SDL2/lib"
)

find_path(SDL2_NET_INCLUDE_DIR SDL_net.h
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/include" "C:/Program Files (x86)/SDL2_net/include"
)

find_library(SDL2_NET_LIBRARY NAMES SDL2_net SDL2_netd
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/lib" "C:/Program Files (x86)/SDL2_net/lib"
)

find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/include" "C:/Program Files (x86)/SDL2_ttf/include"
)

find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf SDL2_ttfd
  HINTS ${SDL2_ROOT_PATH} "C:/vcpkg/installed/x64-windows/lib" "C:/Program Files (x86)/SDL2_ttf/lib"
)

# Check if SDL2 was found, and set the appropriate variables
if (SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
    set(SDL2_FOUND TRUE)
    message(STATUS "Found SDL2: ${SDL2_LIBRARY}")
else()
    set(SDL2_FOUND FALSE)
    message(FATAL_ERROR "SDL2 not found!")
endif()

# Check if SDL2_net was found, and set the appropriate variables
if (SDL2_NET_INCLUDE_DIR AND SDL2_NET_LIBRARY)
    set(SDL2_NET_FOUND TRUE)
    message(STATUS "Found SDL2_net: ${SDL2_NET_LIBRARY}")
else()
    set(SDL2_NET_FOUND FALSE)
    message(WARNING "SDL2_net not found!")
endif()

# Check if SDL2_ttf was found, and set the appropriate variables
if (SDL2_TTF_INCLUDE_DIR AND SDL2_TTF_LIBRARY)
    set(SDL2_TTF_FOUND TRUE)
    message(STATUS "Found SDL2_ttf: ${SDL2_TTF_LIBRARY}")
else()
    set(SDL2_TTF_FOUND FALSE)
    message(WARNING "SDL2_ttf not found!")
endif()

# Provide the results to the main CMake script
if (SDL2_FOUND)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    set(SDL2_LIBRARIES ${SDL2_LIBRARY})
endif()

if (SDL2_NET_FOUND)
    set(SDL2_NET_INCLUDE_DIRS ${SDL2_NET_INCLUDE_DIR})
    set(SDL2_NET_LIBRARIES ${SDL2_NET_LIBRARY})
endif()

if (SDL2_TTF_FOUND)
    set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})
    set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
endif()

# Optionally: If SDL2 is not found, display an error message
if (NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 libraries not found!")
endif()