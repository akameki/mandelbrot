cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_TOOLCHAIN_FILE}" CACHE STRING "vcpkg toolchain file")
else()
    message(FATAL_ERROR "VCPKG_TOOLCHAIN_FILE environment variable not set. Please set it to the path of vcpkg.cmake, like \"...path/to/vcpkg/scripts/buildsystems/vcpkg.cmake\"")
endif()

project(mandelbrot)

add_executable(mandelbrot
    src/main.cpp
    src/shader.cpp
    src/framebuffer.cpp
    src/palettes/palette.cpp
    src/settings.cpp
    external/glad/glad.c
    assets/resources.rc
)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)
# find_package(SDL2 CONFIG REQUIRED)
# find_package(SDL2_mixer CONFIG REQUIRED)

target_link_libraries(mandelbrot PRIVATE
    OpenGL::GL
    glm::glm
    glfw
    imgui::imgui
    implot::implot
    # SDL2::SDL2
    # SDL2::SDL2main
)
# target_link_libraries(mandelbrot PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)

# do not create a console when running as .exe
if(WIN32 AND CMAKE_BUILD_TYPE MATCHES "Release" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(mandelbrot PRIVATE "-Wl,-subsystem,windows")
elseif(WIN32 AND CMAKE_BUILD_TYPE MATCHES "Release" AND CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_options(mandelbrot PRIVATE "/SUBSYSTEM:WINDOWS")
endif()

target_include_directories(mandelbrot PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(mandelbrot PRIVATE ${PROJECT_SOURCE_DIR}/external)
