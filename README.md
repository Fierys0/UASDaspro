# Ather

Ather is a terminal-based RPG exploration and battle system written in C, using ncurses (Linux/macOS) or PDCurses (Windows). This project is structured for portability and cross¿platform builds, with optional auto¿compilation of PDCurses on Windows if it has not been built yet.

![last-commit](https://img.shields.io/github/last-commit/Fierys0/UASDaspro?label=Last%20Update)
![repo-size](https://img.shields.io/github/repo-size/Fierys0/UASDaspro)
![license](https://img.shields.io/github/license/Fierys0/UASDaspro)

## Features
- Overworld movement system with tile¿based terrain.
- Grass encounter events with battle transitions.
- Color coded map tiles (grass, dirt, water).
- ncurses/PDCurses¿based UI.
- Debug logging overlay.
- Battle animations.

## Build Requirements
### Linux / macOS
- gcc or clang  
- CMake >= 3.10  
- ncurses (development headers)

Install ncurses:
```
sudo apt install libncurses-dev
```
or
```
brew install ncurses
```

### Windows
- MinGW-w64 or MSVC  
- CMake >= 3.10  
- PDCurses source code (included in `external/pdcurses`)  

If PDCurses is not compiled, the CMake build will attempt to build it automatically.

## Building

### Linux / macOS (using ncurses)
```
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows (using PDCurses, auto-build)
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

if the program doesn't work on Windows.
You can use [cygwin](https://www.cygwin.com/)
and compile it the same way as Linux

### The resulting executable will be located in:
```
build/Ather
```

