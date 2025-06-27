//-----------------------------------------------------------------------------
// imconfig.h
// Configuration file for Dear ImGui
//-----------------------------------------------------------------------------

#pragma once

#include <assert.h>

// You can modify this file to customize compilation options for Dear ImGui
// This file should be included before including imgui.h in ALL compilation units
// that are involved with Dear ImGui, so the configuration is consistent.

//-----------------------------------------------------------------------------
// Configuration Flags
//-----------------------------------------------------------------------------

// Note: The following macros are now defined in CMakeLists.txt via target_compile_definitions:
// - IMGUI_DISABLE_OBSOLETE_FUNCTIONS
// - IMGUI_DISABLE_OBSOLETE_KEYIO
// - IMGUI_IMPL_OPENGL_LOADER_GLAD

//-----------------------------------------------------------------------------
// Configuration Options
//-----------------------------------------------------------------------------

// Define this to use a custom memory allocator
// #define IMGUI_USER_CONFIG "my_imgui_config.h"

// Define this to disable the default "imgui.ini" file
// #define IMGUI_DISABLE_DEMO_WINDOWS

// Define this to disable the demo windows showing ImGui features
// #define IMGUI_DISABLE_DEMO_WINDOWS

// Define this to disable the metrics/debugger window
// #define IMGUI_DISABLE_METRICS_WINDOW

// Define this to disable the debug log window
// #define IMGUI_DISABLE_DEBUG_TOOLS

//-----------------------------------------------------------------------------
// Platform/Renderer Backend Configuration
//-----------------------------------------------------------------------------

// Note: IMGUI_IMPL_OPENGL_LOADER_GLAD is defined in CMakeLists.txt

//-----------------------------------------------------------------------------
// Data Type Configuration
//-----------------------------------------------------------------------------

// Define this to use 32-bit indices for ImDrawIdx
// This is the default and recommended setting
#define ImDrawIdx unsigned int

// Define this to use 16-bit indices for ImDrawIdx (saves memory but limits number of vertices)
// #define ImDrawIdx unsigned short

//-----------------------------------------------------------------------------
// Memory Configuration
//-----------------------------------------------------------------------------

// Define this to use a custom memory allocator
// #define IMGUI_MALLOC(_SIZE) my_malloc(_SIZE)
// #define IMGUI_FREE(_PTR) my_free(_PTR)

//-----------------------------------------------------------------------------
// Style Configuration
//-----------------------------------------------------------------------------

// Define this to use a custom style
// #define IMGUI_DISABLE_STYLE_OVERRIDE

//-----------------------------------------------------------------------------
// Input Configuration
//-----------------------------------------------------------------------------

// Define this to disable mouse cursor change for ImGui
// #define IMGUI_DISABLE_MOUSE_CURSOR_CHANGE

// Define this to disable keyboard input
// #define IMGUI_DISABLE_KEYBOARD_INPUT

//-----------------------------------------------------------------------------
// Debug Configuration
//-----------------------------------------------------------------------------

// Define this to enable debug assertions
#ifdef _DEBUG
#define IM_ASSERT(_EXPR) assert(_EXPR)
#else
#define IM_ASSERT(_EXPR) ((void)0)
#endif

//-----------------------------------------------------------------------------
// End of Configuration
//----------------------------------------------------------------------------- 