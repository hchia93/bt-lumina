#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <windows.h>

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "LuminaMainWindow.h"

// OpenGL function declarations for Windows
extern "C"
{
	void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void glClear(GLbitfield mask);
}

#ifdef _WIN32
#include <windows.h>
extern int main(int argc, char** argv);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif

int main(int argc, char** argv)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW!\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1200, 720, "Lumina", NULL, NULL);
	if (!window)
	{
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

	LuminaMainWindow mainWindow;
	mainWindow.ApplyImGuiStyle();

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		mainWindow.Render();

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

	// Cleanup - ensure proper order
	try
	{
		// Cleanup ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		
		// Cleanup GLFW
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch (...)
	{
		// Handle any cleanup errors
		fprintf(stderr, "Error during cleanup\n");
	}
	
	return 0;
}

