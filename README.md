# Mandelbrot
https://github.com/user-attachments/assets/85dee219-182b-4e84-b2f0-3c573fbb88c5

Random screenshots:
[One](https://github.com/user-attachments/assets/c400bb76-f1c2-41b9-a55d-53423aacc650)
[Two](https://github.com/user-attachments/assets/9670775b-2c6f-49f5-aff6-e43599183904)
[Three](https://github.com/user-attachments/assets/e1704578-a8a4-4511-b863-be475403b4cf)
[Four](https://github.com/user-attachments/assets/fdd754e4-051f-4630-bbd7-3bde62ce57c9)
[Five](https://github.com/user-attachments/assets/0c0d48a3-01dc-4d7a-8fb2-4c691260d795)
[Six](https://github.com/user-attachments/assets/072b5b0a-5a1b-4ddb-a80f-67729bd7acdd)

## About

OpenGL Mandelbrot set rendering with fine palette control. SSAA and smooth continous coloring can be toggled. This project is designed around setting wallpapers.


### Controls
- Hold MB1 (or WASD) to pan
- Scroll (or Q/E) to zoom
- Tab to toggle options panel
- Esc to quit

## Building and Running
For precompiled binaries, check releases tab, or GitHub Actions for dev builds.

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
(Windows devs should run `.\vcpkg\bootstrap-vcpkg.bat` instead)

## License
MIT :)
