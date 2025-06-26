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

## Dependencies

### **Core Dependencies**
- **qtbase**: Qt6 core functionality and widgets
- **qtconnectivity**: Qt6 Bluetooth connectivity module

### **Build Dependencies**
- **CMake**: 3.16 or higher
- **vcpkg**: Microsoft's C++ package manager
- **Visual Studio 2022**: With C++ workload

## Supported Devices

### Smart Light Bulbs
- **IKEA TRÅDFRI**: Zigbee over Bluetooth
- **Philips Hue**: Bluetooth Low Energy
- **Generic BLE Bulbs**: Standard BLE protocols

### Future Device Support
- CCTV cameras with Bluetooth connectivity
- Bluetooth controllers and input devices
- Other IoT devices with BLE capabilities

## Development Setup

### Prerequisites
- Windows 10/11 with Bluetooth support
- Visual Studio 2022 with C++ workload
- Git
- CMake 3.16+

### Quick Start
1. **Install Dependencies**:
   ```bash
   .\DependencyInstaller.bat
   ```

2. **Generate Visual Studio Solution**:
   ```bash
   .\VisualStudioSolutionGenerator.bat
   ```

3. **Build and Run**:
   - Open `generated-vs/bt-lumina.sln`
   - Build and run the project

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

