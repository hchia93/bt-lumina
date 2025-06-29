# BT-Lumina

BT-Lumina is an ImGui-based Bluetooth Smart Device Controller. This project provides a lightweight, cross-platform GUI application for managing Bluetooth devices using ImGui + GLFW3 + OpenGL, eliminating the need for heavy Qt dependencies.

![Preview](preview.png)S

## Features

- Cross-platform GUI using ImGui + GLFW3 + OpenGL
- Lightweight and easy to deploy (no Qt DLLs required)
- Modern C++17 implementation
- CMake-based build system with vcpkg dependency management

## Prerequisites

| Dependency | Version/Details | Source |
|------------|----------------|--------|
| **ImGui**  | Latest version | https://github.com/ocornut/imgui |
| **GLFW3**  | Version 3.4 or higher | https://www.glfw.org/ |
| **CMake**  | >= 3.16 | https://cmake.org/ |
| **vcpkg**  | Latest version | https://github.com/microsoft/vcpkg |
| **Compiler** | C++17 compatible (MSVC 2022 or newer recommended) | - |

## Project Structure

```
bt-lumina/
├── CMakeLists.txt         # CMake build configuration
├── vcpkg.json             # Dependency manifest
├── generate.bat           # One-step dependency install & solution generator
├── README.md              # This file
├── src/                   # Source code
│   └── main.cpp           # Main application entry point
└── resources/             # Resource files (if any)
```

## Setup Instructions

### 1. Install Dependencies & Generate Visual Studio Solution

Run the following script from the project root:

```cmd
generate.bat
```

This will:
- Check for required tools (Git, vcpkg, CMake)
- Install all dependencies via vcpkg
- Generate a Visual Studio solution in the `generated-vs` directory

### 2. Open and Build in Visual Studio

1. Open `generated-vs/bt-lumina.sln` in Visual Studio 2022 or newer.
2. Build the solution (Ctrl+Shift+B).
3. Run the application (F5 or Ctrl+F5).

> **Note:** The first time you open the solution, you may need to set `bt-lumina` as the startup project:
> - In Solution Explorer, right-click `bt-lumina` and select **Set as StartUp Project**.

### 3. Run the Application

After building, run the executable:

```cmd
.\generated-vs\Debug\bt-lumina.exe
```

## References
For more information about the technologies used:
- [ImGui Documentation](https://github.com/ocornut/imgui)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [vcpkg Documentation](https://github.com/microsoft/vcpkg)

