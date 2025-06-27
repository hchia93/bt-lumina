# BT-Lumina

BT-Lumina is an ImGui-based Bluetooth Smart Device Controller. This project provides a lightweight, cross-platform GUI application for managing Bluetooth devices using ImGui + GLFW3 + OpenGL, eliminating the need for heavy Qt dependencies.

## Features

- Cross-platform GUI using ImGui + GLFW3 + OpenGL
- Lightweight and easy to deploy (no Qt DLLs required)
- Modern C++17 implementation
- CMake-based build system with vcpkg dependency management

## Prerequisites

To build and run this project, external dependencies are required. The `DependencyInstaller.bat` script will install them accordingly.

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
├── DependencyInstaller.bat # Installing dependencies
├── README.md              # This file
├── src/                   # Source code
│   └── main.cpp           # Main application entry point
└── resources/             # Resource files (if any)
```

## Instructions

### Step 1: Install Required Dependencies

Execute the following file to install relevant dependencies:

```cmd
DependencyInstaller.bat
```

### Step 2: Configure and Build the Project

You can build the project using CMake directly:

```cmd
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>/scripts/buildsystems/vcpkg.cmake" -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

### Step 3: Run the Application

After building, run the executable:

```cmd
.\build\Debug\bt-lumina.exe
```

## Alternative Build Methods

### Using Visual Studio Solution Generator

If you prefer using Visual Studio, you can generate a solution:

```cmd
VisualStudioSolutionGenerator.bat
```

Then open the generated solution and press **F5** to build and run.

> **⚠️ Important First-Time Setup**
> The first time you open the solution, you may need to manually set the startup project.
> 1. In the **Solution Explorer**, **right-click** on `bt-lumina`.
> 2. Select **"Set as StartUp Project"** from the menu.
>
> You only need to do this once.

### Manual vcpkg Installation

If you prefer to install dependencies manually:

```cmd
vcpkg install imgui glfw3
```

## Troubleshooting

### Common Issues

1. **vcpkg not found:**
   - Ensure vcpkg is installed and added to your PATH
   - Check that `CMAKE_TOOLCHAIN_FILE` points to the correct vcpkg installation

2. **GLFW/ImGui not found:**
   - Ensure vcpkg is properly set up
   - Verify that dependencies are installed: `vcpkg list`

3. **Build errors:**
   - Ensure you have a C++17 compatible compiler
   - Check that CMake version is >= 3.16
   - Verify all dependencies are properly installed

### Debug Mode

To build in debug mode:

```cmd
cmake --build build --config Debug
```

## Configuration

The application can be configured by modifying the source code in `src/main.cpp`:

- Window dimensions and properties
- ImGui styling and configuration
- Bluetooth device settings

## Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests.

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Notes

> **⚠️ Migration from Qt6**
> This project has been migrated from Qt6 to ImGui + GLFW3 + OpenGL for better performance and reduced dependencies. The new implementation provides a more lightweight and portable solution.

## Dependencies

This project uses the following external libraries managed by vcpkg:

- **ImGui**: Immediate mode GUI library for rendering UI elements
- **GLFW3**: Cross-platform library for creating windows, contexts, and handling input
- **OpenGL**: Graphics API for rendering (provided by the system)

## License

This project is open source. Please check the LICENSE file for details.

---

For more information about the technologies used:
- [ImGui Documentation](https://github.com/ocornut/imgui)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [vcpkg Documentation](https://github.com/microsoft/vcpkg)

