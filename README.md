# BT-Lumina

Bluetooth Lumina is a Windows desktop application for controlling Bluetooth smart devices, particularly smart light bulbs from brands like IKEA and Philips. The project showcases embedded programming using Qt6 framework with C++, providing a graphical user interface for device discovery, connection, and control.

## Project Overview

BT-Lumina serves as a sub-module of a larger BT-Control Panel system, specifically handling all bulb-related control functionality. The architecture is designed to be extensible, with future plans to include CCTV and controller APIs.

## Technology Stack

### **Selected Technology Stack**
- **Frontend Framework**: Qt6 Widgets (C++)
- **Bluetooth Communication**: Qt6 Connectivity Module
- **Build System**: CMake with vcpkg
- **Language**: C++17
- **Platform**: Windows 10/11

### **Why Qt6?**
- **Unified Technology Stack**: Both GUI and Bluetooth communication use Qt6
- **Cross-platform Potential**: Future expansion to Linux/macOS
- **Modern C++**: Excellent C++17 support with Qt6
- **Embedded Programming**: Suitable for embedded system development
- **Rich Ecosystem**: Comprehensive documentation and community support

## Architecture Breakdown

### System Architecture Overview

```mermaid
graph TB
    subgraph "Frontend Layer"
        UI[Qt6 Widgets<br/>User Interface]
        VM[Qt Models<br/>Model/View Pattern]
        FS[Qt Services<br/>Device Management]
    end
    
    subgraph "Bluetooth Layer"
        BLE[Qt6 Connectivity<br/>Bluetooth Module]
        PH[Protocol Handlers<br/>IKEA, Philips, etc.]
        CM[Connection Manager<br/>State Management]
        DP[Data Parser<br/>Command Generation]
    end
    
    subgraph "Hardware Layer"
        BT[Bluetooth Adapter<br/>BLE Support]
        IKEA[IKEA TRÅDFRI<br/>Smart Bulbs]
        PHIL[Philips Hue<br/>Smart Bulbs]
        GEN[Generic BLE<br/>Devices]
    end
    
    UI --> VM
    VM --> FS
    FS --> BLE
    BLE --> PH
    PH --> CM
    CM --> DP
    
    DP --> BT
    BT --> IKEA
    BT --> PHIL
    BT --> GEN
    
    style UI fill:#e1f5fe
    style BLE fill:#fff3e0
    style BT fill:#e8f5e8
```

### Data Flow Diagram

```mermaid
sequenceDiagram
    participant User
    participant UI as Qt6 UI
    participant BLE as Qt6 Connectivity
    participant Device as Smart Device
    
    User->>UI: Scan for devices
    UI->>BLE: Start device discovery
    BLE->>Device: Broadcast discovery
    Device-->>BLE: Advertisement data
    BLE-->>UI: Device found
    UI-->>User: Display devices
    
    User->>UI: Connect to device
    UI->>BLE: Establish connection
    BLE->>Device: Connection request
    Device-->>BLE: Connection established
    BLE-->>UI: Connection status
    UI-->>User: Show connected status
    
    User->>UI: Control light (brightness/color)
    UI->>BLE: Send control command
    BLE->>Device: Send command packet
    Device-->>BLE: Acknowledgment
    BLE-->>UI: Command result
    UI-->>User: Show updated status
```

### Component Interaction Architecture

```mermaid
graph TB
    subgraph "Application Layer"
        Main[Main Application<br/>Qt6 C++ Entry Point]
        Config[Configuration<br/>Qt6 Settings Manager]
    end
    
    subgraph "Presentation Layer"
        Views[Qt6 Widgets<br/>User Interface]
        ViewModels[Qt6 Models<br/>Model/View Pattern]
        Commands[Qt6 Commands<br/>User Actions]
    end
    
    subgraph "Business Logic Layer"
        DeviceManager[Device Manager<br/>Qt6 Service]
        ProtocolManager[Protocol Manager<br/>C++]
        StateManager[State Manager<br/>Connection States]
    end
    
    subgraph "Bluetooth Layer"
        BLEInterface[Qt6 Connectivity<br/>Bluetooth Interface]
        ProtocolHandlers[Protocol Handlers<br/>IKEA, Philips]
        DataParser[Data Parser<br/>Command/Response]
    end
    
    Main --> Config
    Main --> Views
    Views --> ViewModels
    ViewModels --> Commands
    Commands --> DeviceManager
    DeviceManager --> ProtocolManager
    ProtocolManager --> StateManager
    StateManager --> BLEInterface
    BLEInterface --> ProtocolHandlers
    ProtocolHandlers --> DataParser
    
    style Main fill:#e3f2fd
    style Views fill:#f3e5f5
    style DeviceManager fill:#fff3e0
    style BLEInterface fill:#e8f5e8
```

## Project Structure

```
bt-lumina/
├── src/                    # Source files
│   └── main.cpp           # Main application entry point
├── resources/              # Qt resources
│   └── resources.qrc      # Resource file
├── CMakeLists.txt         # CMake build configuration
├── vcpkg.json            # vcpkg dependencies
├── DependencyInstaller.bat    # Dependency installation script
├── VisualStudioSolutionGenerator.bat  # VS solution generator
├── README.md              # This file
└── LICENSE                # MIT License
```

## First Setup

### Prerequisites
- Windows 10/11 with Bluetooth support
- Visual Studio 2022 with C++ workload
- Git
- CMake 3.16+

### Step 1: Install Required Dependencies

#### 1.1 Install vcpkg and Basic Dependencies (Automatic)
Run the following script to install most dependencies (if vcpkg is not installed, it will be downloaded automatically):

```cmd
DependencyInstaller.bat
```

#### 1.2 Manually Install Qt6 Dependencies (Classic Mode)
Due to known issues with vcpkg manifest mode and Qt6, you **must** manually install Qt6 dependencies using classic mode:

```cmd
C:\vcpkg\vcpkg.exe install qtbase qtbase:x64-windows --classic
C:\vcpkg\vcpkg.exe install qtconnectivity qtconnectivity:x64-windows --classic
```

> ⚠️ **Note:** If your vcpkg is not installed at `C:\vcpkg`, adjust the path accordingly.

### Step 2: Generate the Visual Studio Solution
Run the following script to generate the Visual Studio solution (this will create the `generated-vs` directory):

```cmd
VisualStudioSolutionGenerator.bat
```

### Step 3: Build and Run
Open the generated solution at:
- `generated-vs/bt-lumina.sln`

Press **F5** to build and run the application.

> **⚠️ Important First-Time Setup**
> The first time you open the solution, you may need to manually set the startup project:
> 1. In **Solution Explorer**, right-click on `bt-lumina`.
> 2. Select **"Set as StartUp Project"** from the menu.
> You only need to do this once.

### Troubleshooting
- **CMake cannot find Qt6Config.cmake?**
  - Make sure you have manually installed Qt6 dependencies in classic mode as described above.
  - When generating the solution, ensure you use these CMake parameters if building manually:
    ```
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    -DCMAKE_PREFIX_PATH=C:/vcpkg/installed/x64-windows
    ```

For more help, see the [vcpkg documentation](https://learn.microsoft.com/vcpkg/) or open an issue in this repository.

## Features

### **Current Features**
- [x] Qt6-based GUI framework
- [x] Bluetooth device discovery
- [x] Device connection management
- [x] Modern C++17 implementation
- [x] CMake build system
- [x] vcpkg dependency management

### **Planned Features**
- [ ] IKEA TRÅDFRI bulb support
- [ ] Philips Hue bulb support
- [ ] Light control interface (brightness, color)
- [ ] Device status monitoring
- [ ] Settings and configuration
- [ ] Error handling and recovery
- [ ] Performance optimization

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## Roadmap

- [ ] Basic Bluetooth device discovery
- [ ] IKEA TRÅDFRI bulb support
- [ ] Philips Hue bulb support
- [ ] Device connection management
- [ ] Light control interface
- [ ] Device status monitoring
- [ ] Settings and configuration
- [ ] Error handling and recovery
- [ ] Performance optimization
- [ ] Documentation and examples

