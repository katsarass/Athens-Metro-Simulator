# Athens Metro Simulator

Interactive Athens metro map written in C++ with the Simple Game Graphics (SGG) library. Select a starting station and destination, then calculate a route using Dijkstra's algorithm.

## Features

- Metro map with three lines and clickable stations.
- Start and destination selection.
- Route calculation and highlighting.
- **FIND ROUTE** and **RESET** controls.

## Build and run on Windows

1. Install Visual Studio 2022 with **Desktop development with C++**, the v143 toolset, and a Windows 10 SDK.
2. Open `Stage 1/AthensMetroSim.sln`.
3. Select **Debug | x64** or **Release | x64** and build the solution. The Win32 configurations do not contain the same dependency settings.
4. The required runtime DLLs are included in `Stage 1/bin/`. Keep them beside the generated executable.
5. Run from Visual Studio with `Stage 1/Stage1/` as the working directory so that `assets/segoeuib.ttf` can be found. The application also tries the Windows system font directory.

The build instructions were checked against the supplied project configuration; a fresh build has not been verified.

## Repository layout

- `Stage 1/Stage1/`: application source and assets.
- `Stage 1/AthensMetroSim.sln`: Visual Studio solution.
- `Stage 1/include/`, `Stage 1/lib/`, `Stage 1/bin/`: bundled SGG headers, libraries, and runtime dependencies.
- `sgg-main/`: supplied SGG library sources, documentation, and dependencies.

Generated executables, intermediate build files, debug symbols, and local Visual Studio settings are excluded.

## Dependencies and attribution

This project uses **Simple Game Graphics (SGG)**. See the bundled `sgg-main/README.md` and upstream documentation at https://cgaueb.github.io/sgg/index.html for attribution and library information. Third-party libraries, fonts, and assets retain their respective owners' rights. No new license is assigned by this repository preparation.
