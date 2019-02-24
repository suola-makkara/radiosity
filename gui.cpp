#include <climits>
#include <iostream>

#if defined __GNUC__
	#include <filesystem>
	using namespace std::filesystem;
#elif defined _MSC_VER
	#include <experimental/filesystem>
	using namespace std::experimental::filesystem::v1;
#else
	#define NO_FILESYSTEM_LIB
#endif

#include "gui.hpp"
#include "scene.hpp"
#include "editor.hpp"

void GUI::initialize(GLFWwindow *window)
{
	// Initialize ImGui Context
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void GUI::destruct()
{
	// Destruct ImGui Context
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::update(GLFWwindow *window)
{
	// Create New Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Update Frame
	mainMenuBar();
	
	renderSettings();
	editor();
	renderingProgress();
	helpWindow();
	quitWindow(window);
	saveWindow();
}

void GUI::render()
{
	// Render Frame
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool GUI::showRenderSettings = false;
bool GUI::showEditor = false;
bool GUI::showRenderingProgress = false;
bool GUI::showHelpWindow = false;
bool GUI::showQuitWindow = false;
bool GUI::showSaveWindow = false;

void GUI::renderSettings()
{
	if (!showRenderSettings) return;
	
	ImGui::SetNextWindowPos({ 10, 40 }, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize({ 320, 160 }, ImGuiCond_Appearing);

	ImGui::Begin("Render Settings", &showRenderSettings);
	
	ImGui::PushItemWidth(ImGui::GetFontSize() * -14);

	if (ImGui::CollapsingHeader("Surface Subdivision"))
	{
		static int subDiv = 1;
		ImGui::DragInt("Subdivision Level", &subDiv, 1, 1, INT_MAX);
		static float minSubdivisionSize = 0.25f;
		ImGui::DragFloat("Min Subdivision Size",
				&minSubdivisionSize, 0.025f, 0.0f, FLT_MAX);
	
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Form Factor Calculation"))
	{
		const char *algorithms[] {
			"Diff Area To Area Monte Carlo",
				"Area To Area Monte Carlo" };
		static int algorithm = 0;
		ImGui::Combo("Form Factor Algorithm",
				&algorithm, algorithms, IM_ARRAYSIZE(algorithms));

		static int samples = 1;
		ImGui::DragInt("Samples", &samples, 1, 1, INT_MAX);

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Matrix Calculation"))
	{
	}

	ImGui::End();
}

void GUI::editor()
{
	if (!showEditor) return;

	ImGui::SetNextWindowPos({ 910, 40 }, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize({ 280, 200 }, ImGuiCond_Appearing);

	ImGui::Begin("Editor", &showEditor);

	if (ImGui::Button("Add Plane"))
	{
		Scene::addObject(Plane());
		Editor::setSelectedObject(&Scene::getObjects().back());
	}

	if (Editor::getSelectedObject() != nullptr &&
			ImGui::CollapsingHeader("Object"))
	{
		ImGui::Indent(4);

		Plane *selectedObject = Editor::getSelectedObject();

		if (ImGui::CollapsingHeader("Transformation"))
		{
			glm::vec3 position = selectedObject->getPosition();
			glm::vec3 rotation = selectedObject->getRotation();
			glm::vec2 size = selectedObject->getSize();
			if (ImGui::DragFloat3("Position", &position.x, 0.1f))
				selectedObject->setPosition(position);
			if (ImGui::DragFloat3("Rotation", &rotation.x,
						1.0f, -360.0f, 360.0f))
				selectedObject->setRotation(rotation);
			if (ImGui::DragFloat2("Size", &size.x, 0.5f, 0.0f, FLT_MAX))
				selectedObject->setSize(size);

			ImGui::Spacing();
		}
		
		if (ImGui::CollapsingHeader("Material"))
		{
			glm::vec3 color = selectedObject->getColor();
			glm::vec3 emission = selectedObject->getEmission();
			if (ImGui::ColorEdit3("Color", &color.x))
				selectedObject->setColor(color);
			ImGui::ColorEdit3("Emission", &emission.x);
				selectedObject->setEmission(emission);
		}

		ImGui::Spacing();

		if (ImGui::Button("Copy"))
			Scene::addObject(*selectedObject);

		ImGui::SameLine();

		if (ImGui::Button("Remove"))
		{
			Editor::reset();
			Scene::removeObject(selectedObject);
		}
	}

	ImGui::End();
}

void GUI::renderingProgress()
{
	if (!showRenderingProgress) return;

	// TODO Implement progress bar & querrying progress

	ImGui::Begin("Rendering Progress", &showRenderingProgress);
	ImGui::Text("Rendering progress ...");
	ImGui::End();
}

void GUI::helpWindow()
{
	if (!showHelpWindow) return;

	// TODO Write help text

	ImGui::Begin("Help", &showHelpWindow);
	ImGui::Text("Help text here!");
	ImGui::End();
}

void GUI::quitWindow(GLFWwindow *window)
{
	if (!showQuitWindow) return;

	if (Scene::isSaved())
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	ImGui::OpenPopup("Quit");

	if (ImGui::BeginPopupModal("Quit", NULL,
				ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Quit without saving?");
		ImGui::Spacing();
		if (ImGui::Button("Ok"))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			showQuitWindow = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void GUI::saveWindow()
{
	if (!showSaveWindow) return;

	ImGui::OpenPopup("Save As");

	if (ImGui::BeginPopupModal("Save As", NULL,
				ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Save to ./scenes/ as:");
		ImGui::SameLine();
		static char buffer[64] = "";
		ImGui::InputText("", buffer, 64);

		if (ImGui::Button("Save"))
		{
			Scene::saveAs(std::string("scenes/") + buffer);
			showSaveWindow = false;
			ImGui::CloseCurrentPopup();
			buffer[0] = '\0';
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			showSaveWindow = false;
			ImGui::CloseCurrentPopup();
			buffer[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void GUI::mainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
				Scene::loadNew();

			if (ImGui::BeginMenu("Open"))
			{
#ifndef NO_FILESYSTEM_LIB
				for (auto &file :
						directory_iterator("scenes"))
				{
					if (file.path().extension() ==
							path(".sc"))
						if (ImGui::MenuItem(file.path()
									.filename().string().c_str()))
							Scene::load(file.path().string());
				}
#else
				ImGui::Text("Open from ./scenes/");
				static char buffer[64] = "";
				ImGui::InputText("", buffer, 64);
				if (ImGui::Button("Open"))
				{
					Scene::load(buffer);
					buffer[0] = '\0';
				}
#endif
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save", NULL, false,
						Scene::hasFileAddress()))
				Scene::save();

			if (ImGui::MenuItem("Save As"))
				showSaveWindow = true;

			ImGui::Separator();

			if (ImGui::MenuItem("Quit"))
				showQuitWindow = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Editor"))
				showEditor = true;
			if (ImGui::MenuItem("Render Settings"))
				showRenderSettings = true;
			if (ImGui::MenuItem("Rendering Progress"))
				showRenderingProgress = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering"))
		{
			ImGui::MenuItem("Render");
			ImGui::Separator();
			ImGui::MenuItem("Edit");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Help"))
				showHelpWindow = true;
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
