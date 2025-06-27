#include "imconfig.h"
#include <windows.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>

// OpenGL function declarations for Windows
extern "C" {
    void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
    void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void glClear(GLbitfield mask);
}

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "BT-Lumina - Bluetooth Controller", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        ImGui::Begin("BT-Lumina", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        if (ImGui::BeginTabBar("MainTabs"))
        {
            if (ImGui::BeginTabItem("Device Discovery"))
            {
                ImGui::Text("Bluetooth Device Discovery");
                ImGui::Separator();
                
                if (ImGui::Button("Scan for Devices", ImVec2(120, 0)))
                {
                    // TODO: Add Bluetooth scanning
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Stop Scan", ImVec2(120, 0)))
                {
                    // TODO: Stop scanning
                }
                
                ImGui::Separator();
                ImGui::Text("Discovered Devices:");
                ImGui::BeginChild("DeviceList", ImVec2(400, 200), true);
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
                
                if (ImGui::Button("Apply Settings", ImVec2(120, 0)))
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
                if (ImGui::Button("Save Settings", ImVec2(120, 0)))
                {
                    // TODO: Save settings
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Reset to Defaults", ImVec2(120, 0)))
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