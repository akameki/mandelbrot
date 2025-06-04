# Mandelbrot

Mandelbrot set explorer.

![Screenshot](assets/screenshot-1.png)
![2025-06-02-161609-036](https://github.com/user-attachments/assets/52229904-a0a5-44a9-9251-d10f71e68332)
![2025-06-02-162234-599](https://github.com/user-attachments/assets/7b95342d-2ad5-4435-87fb-1164e8f86cb5)


### Controls
- WASD to move camera
- Q/E to zoom in/out
- Hold space to move/zoom faster
- Tab to toggle options
- Esc to quit

## Building

The project can be built with CMake and vcpkg (included as a submodule).

Example:
```
git clone --recurse-submodules https://github.com/akameki/mandelbrot.git
cd mandelbrot
./vcpkg/bootstrap-vcpkg.sh
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/mandelbrot
```
(Windows devs would instead run .\vcpkg\bootstrap-vcpkg.bat)

Also, check github artifacts for latest dev builds.

## License
MIT
