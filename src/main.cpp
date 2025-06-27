#include "imconfig.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "BT-Lumina", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize glad!\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    IMGUI_CHECKVERSION();
    if (!ImGui::CreateContext()) {
        fprintf(stderr, "Failed to create ImGui context!\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        fprintf(stderr, "Failed to initialize ImGui_ImplGlfw!\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        fprintf(stderr, "Failed to initialize ImGui_ImplOpenGL3!\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create main window with tabs
        ImGui::Begin("BT-Lumina Main Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        if (ImGui::BeginTabBar("MainTabs"))
        {
            if (ImGui::BeginTabItem("Device Discovery"))
            {
                ImGui::Text("Bluetooth Device Discovery");
                ImGui::Separator();
                
                if (ImGui::Button("Scan for Devices"))
                {
                    // TODO: Implement device scanning
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Stop Scanning"))
                {
                    // TODO: Implement stop scanning
                }
                
                ImGui::Separator();
                ImGui::Text("Discovered Devices:");
                ImGui::BeginChild("DeviceList", ImVec2(0, 200), true);
                ImGui::Text("No devices found");
                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Device Control"))
            {
                ImGui::Text("Device Control Panel");
                ImGui::Separator();
                
                ImGui::Text("Connected Device: None");
                ImGui::Separator();
                
                ImGui::Text("Light Control:");
                static float brightness = 0.5f;
                ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f);
                
                static float color[3] = {1.0f, 1.0f, 1.0f};
                ImGui::ColorEdit3("Color", color);
                
                if (ImGui::Button("Apply Settings"))
                {
                    // TODO: Send commands to device
                }
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Settings"))
            {
                ImGui::Text("Application Settings");
                ImGui::Separator();
                
                static bool auto_connect = false;
                ImGui::Checkbox("Auto-connect to last device", &auto_connect);
                
                static int scan_timeout = 10;
                ImGui::SliderInt("Scan Timeout (seconds)", &scan_timeout, 5, 30);
                
                ImGui::Separator();
                if (ImGui::Button("Save Settings"))
                {
                    // TODO: Save settings
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Reset to Defaults"))
                {
                    // TODO: Reset settings
                }
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("About"))
            {
                ImGui::Text("BT-Lumina v1.0.0");
                ImGui::Text("Bluetooth Smart Device Controller");
                ImGui::Separator();
                ImGui::Text("Built with:");
                ImGui::BulletText("ImGui + GLFW3 + OpenGL");
                ImGui::BulletText("C++17");
                ImGui::BulletText("CMake + vcpkg");
                ImGui::Separator();
                ImGui::Text("A lightweight alternative to Qt-based solutions");
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
} 