# BT-Lumina

Bluetooth Lumina is a Windows desktop application for controlling Bluetooth smart devices, particularly smart light bulbs from brands like IKEA and Philips. The project showcases embedded programming using a mix of C++ and C, providing a graphical user interface for device discovery, connection, and control.

## Project Overview

BT-Lumina serves as a sub-module of a larger BT-Control Panel system, specifically handling all bulb-related control functionality. The architecture is designed to be extensible, with future plans to include CCTV and controller APIs.

## Architecture Breakdown

### System Architecture Overview

```mermaid
graph TB
    subgraph "Frontend Layer"
        UI[User Interface<br/>Qt Widgets/QML]
        VM[ViewModels<br/>Qt Model/View]
        FS[Frontend Services<br/>C++ Device Management]
    end
    
    subgraph "Hardware API Layer"
        BLE[Bluetooth Low Energy<br/>C++/C Module]
        PH[Protocol Handlers<br/>IKEA, Philips, etc.]
        CM[Connection Manager<br/>State Management]
        DP[Data Parser<br/>Command Generation]
    end
    
    subgraph "Bluetooth Framework"
        WR[Windows Runtime<br/>WinRT APIs]
        W32[Windows API<br/>Win32]
        QT[Qt Bluetooth<br/>Module]
        NET[.NET Bluetooth<br/>Libraries]
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
    
    DP --> WR
    DP --> W32
    DP --> QT
    DP --> NET
    
    WR --> BT
    W32 --> BT
    QT --> BT
    NET --> BT
    
    BT --> IKEA
    BT --> PHIL
    BT --> GEN
    
    style UI fill:#e1f5fe
    style BLE fill:#fff3e0
    style WR fill:#f3e5f5
    style BT fill:#e8f5e8
```

### Data Flow Diagram

```mermaid
sequenceDiagram
    participant User
    participant UI as Frontend UI
    participant API as Hardware API
    participant BLE as Bluetooth Framework
    participant Device as Smart Device
    
    User->>UI: Scan for devices
    UI->>API: Start device discovery
    API->>BLE: Initialize BLE scanning
    BLE->>Device: Broadcast discovery
    Device-->>BLE: Advertisement data
    BLE-->>API: Device found
    API-->>UI: Update device list
    UI-->>User: Display devices
    
    User->>UI: Connect to device
    UI->>API: Establish connection
    API->>BLE: Create BLE connection
    BLE->>Device: Connection request
    Device-->>BLE: Connection established
    BLE-->>API: Connection status
    API-->>UI: Update connection state
    UI-->>User: Show connected status
    
    User->>UI: Control light (brightness/color)
    UI->>API: Send control command
    API->>BLE: Transmit BLE data
    BLE->>Device: Send command packet
    Device-->>BLE: Acknowledgment
    BLE-->>API: Command result
    API-->>UI: Update device state
    UI-->>User: Show updated status
```

### Framework Selection Flow

```mermaid
flowchart TD
    Start([Project Start]) --> CppC{C++/C Constraint?}
    CppC -->|Yes| Windows{Windows Only?}
    CppC -->|No| Qt[Qt Bluetooth Module]
    
    Windows -->|Yes| Modern{Modern Windows API?}
    Windows -->|No| Qt
    
    Modern -->|Yes| WinRT[Windows Runtime<br/>WinRT Bluetooth APIs]
    Modern -->|No| Win32[Windows Bluetooth API<br/>Win32]
    
    Qt --> CrossPlatform{Cross-platform needed?}
    CrossPlatform -->|Yes| QtFinal[Use Qt Bluetooth]
    CrossPlatform -->|No| Performance{Performance critical?}
    
    Performance -->|Yes| Win32Final[Use Win32 API]
    Performance -->|No| WinRTFinal[Use WinRT APIs]
    
    WinRT --> WinRTFinal
    Win32 --> Win32Final
    
    WinRTFinal --> End([Recommended: WinRT])
    Win32Final --> End2([Alternative: Win32])
    QtFinal --> End3([Alternative: Qt])
    
    style WinRTFinal fill:#90EE90
    style Win32Final fill:#FFB6C1
    style QtFinal fill:#FFB6C1
```

### Component Interaction Architecture

```mermaid
graph TB
    subgraph "Application Layer"
        Main[Main Application<br/>Qt C++ Entry Point]
        Config[Configuration<br/>Qt Settings Manager]
    end
    
    subgraph "Presentation Layer"
        Views[Qt UI Views<br/>Widgets/QML]
        ViewModels[Qt Models<br/>Model/View Pattern]
        Commands[Qt Commands<br/>User Actions]
    end
    
    subgraph "Business Logic Layer"
        DeviceManager[Device Manager<br/>C++ Service]
        ProtocolManager[Protocol Manager<br/>C++/C]
        StateManager[State Manager<br/>Connection States]
    end
    
    subgraph "Hardware Abstraction Layer"
        BLEInterface[BLE Interface<br/>C++/C]
        ProtocolHandlers[Protocol Handlers<br/>IKEA, Philips]
        DataParser[Data Parser<br/>Command/Response]
    end
    
    subgraph "Framework Layer"
        WinRT[WinRT APIs<br/>Bluetooth]
        Win32[Win32 APIs<br/>Alternative]
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
    DataParser --> WinRT
    DataParser --> Win32
    
    style Main fill:#e3f2fd
    style Views fill:#f3e5f5
    style DeviceManager fill:#fff3e0
    style BLEInterface fill:#e8f5e8
    style WinRT fill:#fce4ec
```

### Frontend (Windows Desktop Application)

The frontend is a native Windows desktop application with multiple technology options:

#### **Option 1: C++/C Native Frontend** *(Recommended for C++/C Constraint)*

- **Framework**: Qt Widgets or Qt Quick (QML)
- **Language**: C++ with Qt framework
- **UI Components**:
  - Device discovery and scanning interface
  - Device connection management
  - Light bulb control panel (brightness, color, on/off)
  - Device status monitoring
  - Settings and configuration panel

**Pros**: Pure C++/C stack, no language interop needed, better performance
**Cons**: More complex UI development, larger Qt dependency

#### **Option 2: C# .NET Frontend** *(Alternative)*

- **Framework**: Windows Presentation Foundation (WPF) or Windows Forms (WinForms)
- **Language**: C# with .NET Framework
- **UI Components**: Same as above

**Pros**: Easier UI development, rich ecosystem, modern XAML
**Cons**: Requires C++/C interop, managed code overhead

#### **Option 3: Hybrid Approach** *(Flexible)*

- **Framework**: Qt for UI + C++/C for business logic
- **Language**: C++ with Qt + C for hardware API
- **Architecture**: Pure C++/C stack with Qt UI layer

**Pros**: Best of both worlds, consistent C++/C codebase
**Cons**: Qt learning curve, larger application size

## **Frontend Technology Comparison**

| Technology | Language | UI Framework | C++/C Integration | Performance | Development Speed |
|------------|----------|--------------|-------------------|-------------|-------------------|
| **Qt Widgets** | C++ | Qt | Native | Excellent | Moderate |
| **Qt Quick** | C++/QML | Qt | Native | Excellent | Fast |
| **WPF** | C# | XAML | Interop Required | Good | Fast |
| **WinForms** | C# | Windows Forms | Interop Required | Good | Very Fast |
| **Native Win32** | C++ | Win32 API | Native | Excellent | Slow |

#### **Recommended Approach for BT-Lumina:**

Given the C++/C constraint and embedded programming showcase requirement:

1. **Primary Choice**: Qt Widgets/Quick with C++
   - Pure C++/C stack throughout
   - Excellent performance and control
   - Rich UI capabilities
   - Cross-platform potential

2. **Alternative**: C# WPF with C++/C interop
   - Faster UI development
   - Modern XAML interface
   - Requires P/Invoke or COM interop

#### Frontend Responsibilities:
- Provide intuitive GUI for device interaction
- Handle user input and display device states
- Manage device discovery and connection UI
- Display real-time device status and feedback
- Handle application settings and preferences

### Hardware API Layer

The hardware API layer serves as the bridge between the frontend and Bluetooth devices:

- **Language**: C++ with C interop
- **Architecture**: Modular design with clear separation of concerns
- **Components**:
  - Bluetooth Low Energy (BLE) communication module
  - Device protocol handlers (IKEA, Philips, etc.)
  - Connection management and state handling
  - Data parsing and command generation

#### Hardware API Responsibilities:
- Bluetooth device discovery and scanning
- BLE connection establishment and management
- Device-specific protocol implementation
- Command encoding and data transmission
- Response parsing and error handling
- Connection state management and recovery

## Recommended Bluetooth Frameworks

### Framework Implementation Strategy

The Bluetooth framework implementation is **not** a "4 pick 1" situation. Instead, we recommend a **layered approach** that combines multiple frameworks for optimal functionality:

#### Primary Implementation: Windows Runtime (WinRT) + Fallback Strategy

```mermaid
graph TD
    subgraph "Primary Path"
        WinRT[Windows Runtime<br/>WinRT Bluetooth APIs]
        BLE[BLE Communication<br/>Device Control]
    end
    
    subgraph "Fallback Path"
        Win32[Windows API<br/>Win32 Bluetooth]
        Legacy[Legacy Device Support<br/>Older Protocols]
    end
    
    subgraph "Alternative Path"
        Qt[Qt Bluetooth Module]
        Cross[Cross-platform Support<br/>Future Expansion]
    end
    
    App[Application] --> WinRT
    WinRT --> BLE
    
    WinRT -->|Fallback| Win32
    Win32 --> Legacy
    
    WinRT -->|Alternative| Qt
    Qt --> Cross
    
    style WinRT fill:#90EE90
    style Win32 fill:#FFB6C1
    style Qt fill:#FFB6C1
```

#### Hybrid Implementation Benefits:

| Layer | Primary Framework | Fallback Framework | Use Case |
|-------|------------------|-------------------|----------|
| **Device Discovery** | WinRT APIs | Win32 APIs | Comprehensive device scanning |
| **BLE Communication** | WinRT BLE | Win32 BLE | Modern smart device support |
| **Legacy Support** | Win32 APIs | Qt Bluetooth | Older device compatibility |
| **Cross-platform** | Qt Bluetooth | Platform-specific | Future expansion |

### For Windows Desktop Development:

#### 1. **Windows Runtime (WinRT) Bluetooth APIs** *(Primary Choice)*

| Aspect | Details |
|--------|---------|
| **Pros** | Native Windows support, comprehensive BLE functionality, modern API design |
| **Cons** | Windows 10+ only, requires UWP or WinUI 3 for some features |
| **Best for** | Modern Windows applications with full BLE support |
| **Implementation** | Primary framework for most functionality |

#### 2. **Windows Bluetooth API (Win32)** *(Fallback & Legacy)*

| Aspect | Details |
|--------|---------|
| **Pros** | Direct Windows API access, maximum control, no additional dependencies |
| **Cons** | More complex API, requires more boilerplate code |
| **Best for** | Low-level control and performance-critical applications |
| **Implementation** | Fallback for unsupported devices, legacy protocol support |

#### 3. **Qt Bluetooth Module** *(Alternative & Cross-platform)*

| Aspect | Details |
|--------|---------|
| **Pros** | Cross-platform, mature framework, good C++ integration |
| **Cons** | Larger runtime, licensing considerations |
| **Best for** | Applications requiring cross-platform compatibility |
| **Implementation** | Alternative implementation, future cross-platform expansion |

#### 4. **.NET Bluetooth Libraries** *(Frontend Integration)*

| Aspect | Details |
|--------|---------|
| **Pros** | Easy integration with C# frontend, managed code benefits |
| **Cons** | Limited low-level control, potential performance overhead |
| **Best for** | Rapid development with managed code approach |
| **Implementation** | Frontend service layer, high-level device management |

### Recommended Implementation Approach for BT-Lumina:

**Core Architecture: WinRT + Win32 Hybrid**

1. **Primary Implementation**: Windows Runtime (WinRT) Bluetooth APIs
   - Use C++/WinRT for modern Bluetooth functionality
   - Provides excellent BLE support for smart devices
   - Native Windows integration

2. **Fallback Implementation**: Windows Bluetooth API (Win32)
   - For devices not supported by WinRT
   - Legacy protocol support
   - Maximum control when needed

3. **Alternative Implementation**: Qt Bluetooth Module
   - For cross-platform future expansion
   - When WinRT/Win32 limitations are encountered

4. **Frontend Integration**: .NET Bluetooth Libraries
   - High-level device management in C# frontend
   - Simplified API for common operations

#### Implementation Priority:

```mermaid
graph LR
    subgraph "Phase 1: Core"
        WinRT[WinRT Implementation<br/>Primary BLE Support]
    end
    
    subgraph "Phase 2: Fallback"
        Win32[Win32 Fallback<br/>Legacy Support]
    end
    
    subgraph "Phase 3: Enhancement"
        Qt[Qt Alternative<br/>Cross-platform]
    end
    
    subgraph "Phase 4: Integration"
        NET[.NET Integration<br/>Frontend Services]
    end
    
    WinRT --> Win32
    Win32 --> Qt
    Qt --> NET
    
    style WinRT fill:#90EE90
    style Win32 fill:#FFB6C1
    style Qt fill:#FFB6C1
    style NET fill:#FFB6C1
```

## Project Structure

```
bt-lumina/
├── src/
│   ├── frontend/           # C# WPF/WinForms application
│   │   ├── UI/            # User interface components
│   │   ├── ViewModels/    # MVVM pattern implementation
│   │   └── Services/      # Frontend services
│   ├── hardware-api/      # C++/C hardware interface
│   │   ├── bluetooth/     # Bluetooth communication
│   │   ├── protocols/     # Device-specific protocols
│   │   └── utils/         # Utility functions
│   └── shared/            # Shared components
├── include/               # Header files
├── libs/                  # External libraries
├── docs/                  # Documentation
└── tests/                 # Unit and integration tests
```

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
- Visual Studio 2019/2022 with C++ and C# workloads
- Windows 10 SDK (for Bluetooth APIs)
- .NET Framework 4.8 or .NET 6+

### Build Requirements
- CMake 3.15+
- Git
- Bluetooth adapter with BLE support

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

