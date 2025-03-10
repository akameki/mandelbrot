# cmake_minimum_required(VERSION 3.15)
# project(Mandelbrot)

# # set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
# # set(CMAKE_CXX_STANDARD 17)

# find_package(glfw3 REQUIRED)
# find_package(glad REQUIRED)
# # find_package(miniaudio CONFIG REQUIRED)

# find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h"
#     HINTS
#         "${CMAKE_CURRENT_SOURCE_DIR}/miniaudio" # If you have miniaudio in your source dir
#         "D:/dev/vcpkg/installed/x64-windows/include" # Replace with your vcpkg include dir
# )

# add_executable(Mandelbrot main.cpp)

# target_link_libraries(Mandelbrot PRIVATE glfw glad)
# target_include_directories(Mandelbrot PRIVATE ${MINIAUDIO_INCLUDE_DIRS})

#########

cmake_minimum_required(VERSION 3.15)
project(HelloWorld)

# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_TOOLCHAIN_FILE}" CACHE STRING "vcpkg toolchain file")
else()
    message(FATAL_ERROR "VCPKG_TOOLCHAIN_FILE environment variable not set. Please set it to the path of vcpkg.cmake, like \"...path/to/vcpkg/scripts/buildsystems/vcpkg.cmake\"")
endif()


set(SOURCES
    src/hello.cpp
    src/glad.c
)
add_executable(HelloWorld ${SOURCES})

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)


target_link_libraries(HelloWorld PRIVATE
    glfw
    OpenGL::GL
    SDL2::SDL2
    SDL2::SDL2main
)
target_link_libraries(HelloWorld PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)

target_include_directories(HelloWorld PRIVATE ${PROJECT_SOURCE_DIR}/include)