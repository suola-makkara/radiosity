#ifndef GUI_HPP
#define GUI_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

class GUI
{
public:
	static void initialize(GLFWwindow *window);

	static void destruct();
	
	static void update(GLFWwindow *window);

	static void render();

	static void enable();
	static void disable();

	static bool isActive();

	static bool isPopupOpen();

	static void openEditor();

	friend void windowCloseCallback(GLFWwindow *window);
private:
	static bool showRenderSettings;
	static bool showEditor;
	static bool showRenderingProgress;
	static bool showHelpWindow;
	static bool showQuitWindow;
	static bool showSaveWindow;

	static bool enabled;

	static void mainMenuBar();
	
	static void renderSettings();
	static void editor();
	static void renderingProgress();
	static void helpWindow();
	static void quitWindow(GLFWwindow *window);
	static void saveWindow();
};

#endif // GUI_HPP
