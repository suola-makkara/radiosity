#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <iostream>
#include <vector>
#include <cstdlib>

#include "camera.hpp"
#include "shader.hpp"
#include "render_settings.hpp"
#include "plane.hpp"
#include "matrix_operations.hpp"

#define WIDTH 800
#define HEIGHT 800

int main()
{
	// Create window and initialize OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Radiosity",
			NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create window" << '\n';
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << '\n';
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);


	// Create camera
	Camera camera(window, glm::vec3(0.0f, 0.0f, 0.0f));
	camera.disable(window);


	// Define materials
	Material whiteWall { { 0.9f, 0.9f, 0.9f }, { 0.0f, 0.0f, 0.0f } };
	Material redWall   { { 0.9f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
	Material greenWall { { 0.0f, 0.9f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
	Material whiteLamp { { 0.1f, 0.1f, 0.1f }, { 5.0f, 5.0f, 5.0f } };

	// Define scene objects
	unsigned int planeCount = 14;
	Plane planes[planeCount] {
		// Lamp
		{ { 0.25, 0.998, 0.25 }, { -0.25, 0.998, 0.25 },
			{ 0.25, 0.998, -0.25 }, whiteLamp },
		{ { 0.25, 0.999, 0.25 }, { 0.25, 0.999, -0.25 },
			{ -0.25, 0.999, 0.25 }, whiteWall },
		// Walls
		{ { -1, -1,  1 }, { -1, -1, -1 }, { -1,  1,  1 }, redWall },
		{ {  1, -1,  1 }, { -1, -1,  1 }, {  1,  1,  1 }, whiteWall },
		{ {  1, -1, -1 }, {  1, -1,  1 }, {  1,  1, -1 }, greenWall },
		{ { -1, -1, -1 }, {  1, -1, -1 }, { -1,  1, -1 }, whiteWall },
		{ { -1, -1, -1 }, { -1, -1,  1 }, {  1, -1, -1 }, whiteWall },
		{ {  1,  1,  1 }, { -1,  1,  1 }, {  1,  1, -1 }, whiteWall },
		// Box
		{ { 0.25, -0.999, -0.25 }, { 0.75, -0.999, -0.25 },
			{ 0.25,  0.100, -0.25 }, whiteWall },
		{ { 0.75, -0.999, -0.25 }, { 0.75, -0.999, -0.75 },
			{ 0.75,  0.100, -0.25 }, whiteWall },
		{ { 0.75, -0.999, -0.75 }, { 0.25, -0.999, -0.75 },
			{ 0.75,  0.100, -0.75 }, whiteWall },
		{ { 0.25, -0.999, -0.75 }, { 0.25, -0.999, -0.25 },
			{ 0.25,  0.100, -0.75 }, whiteWall },
		{ { 0.25,  0.100, -0.25 }, { 0.75,  0.100, -0.25 },
			{ 0.25,  0.100, -0.75 }, whiteWall },
		{ { 0.25, -0.999, -0.25 }, { 0.25, -0.999, -0.75 },
			{ 0.75, -0.999, -0.25 }, whiteWall }
	};


	// Create element index help table
	unsigned int planeIndices[planeCount];
	unsigned int index = 0;
	for (int i = 0; i < planeCount; i++)
	{
		planeIndices[i] = index;
		index += planes[i].elements();
	}
	
	unsigned int elements = index;
	

	// Test for memory required for matrices
	if (elements * (unsigned long long)elements * 16 > RAM / sizeof(float))
	{
		std::cout << "Not enough memory" << '\n';
		glfwTerminate();
		return -1;
	}


	// Form factor matrix F
	float **formFactorMatrix;
	formFactorMatrix = new float*[elements];
	for (int j = 0; j < elements; j++)
	{
		formFactorMatrix[j] = new float[elements];
		for (int k = 0; k < elements; k++)
			formFactorMatrix[j][k] = 0.0f;
	}

	// Evaluate form factor matrix F
	std::cout << "Evaluating form factor matrix: " << elements << "x" << elements << '\n';
	switch (FORM_FACTOR_EVALUATION)
	{
	case DIFF_AREA_TO_AREA_MONTE_CARLO:
		for (int i = 0; i < planeCount; i++)
		{
			glm::vec3 normal = planes[i].getNormal();
			glm::vec3 tangent;

			glm::vec3 unNormalizedTangent = glm::cross(normal,
					glm::vec3(1.0f, 0.0f, 0.0f));
			if (glm::length(unNormalizedTangent) > 0.001f)
				tangent = glm::normalize(unNormalizedTangent);
			else
				tangent = glm::normalize(glm::cross(normal,
							glm::vec3(0.0, 1.0f, 0.0f)));

			glm::vec3 bitangent = glm::cross(normal, tangent);
	
			glm::mat3 planeNormalTransform(
					tangent.x, tangent.y, tangent.z,
					normal.x, normal.y, normal.z,
					bitangent.x, bitangent.y, bitangent.z);

			unsigned int width = planes[i].getWidth();
			unsigned int height = planes[i].getHeight();

			float dx = 1.0f / width;
			float dy = 1.0f / height;

			float u = dx / 2.0f;
			for (int x = 0; x < width; x++)
			{
				float v = dy / 2.0f;
				for (int y = 0; y < height; y++)
				{
					Ray ray;
					ray.origin = planes[i].getPointUV(u, v);
					for (int s = 0; s < SAMPLES; s++)
					{
						float rx, rz;
						while (true)
						{
							rx = 2.0f * std::rand() /
								(float)RAND_MAX - 1.0f;
							rz = 2.0f * std::rand() /
								(float)RAND_MAX - 1.0f;
							if (std::pow(rx, 2) + std::pow(rz, 2) < 1.0f)
								break;
						}

						float ry = std::sqrt(1.0f -
								std::pow(rx, 2) - std::pow(rz, 2));

						glm::vec3 direction =
							glm::normalize(glm::vec3(rx, ry, rz));
						ray.direction = planeNormalTransform * direction;

						float iu, iv, it;
						float minT = -1;
						float minU, minV;
						unsigned int minI;
						for (int j = 0; j < planeCount; j++)
						{
							if (i == j) continue;

							if (planes[j].intersectsRay(ray, iu, iv, it))
							{
								if (it <= 0) continue;
								if (minT == -1 || it < minT)
								{
									minT = it;
									minU = iu;
									minV = iv;
									minI = j;
								}
							}
						}

						if (minT == -1)
							continue;

						formFactorMatrix[planeIndices[minI] +
							planes[minI].getElementIndex(minU, minV)]
							[planeIndices[i] + x + y * width] +=
								1.0f / SAMPLES;
					}
					v += dy;
				}
				u += dx;
			}
		}
		break;
	}


	// Solve matrix equation KB = E	
	std::cout << "Evaluating inverse matrix: " << elements << "x"
		<< elements << '\n';

	// Create emission(E) and radiosity(B) vectors
	float *emissionVector[3];
	float *radiosityVector[3];
	for (int i = 0; i < 3; i++)
	{
		emissionVector[i] = new float[elements];
		radiosityVector[i] = new float[elements];
	}

	// Fill emission vector E
	for (int i = 0; i < planeCount; i++)
		for (int j = 0; j < planes[i].elements(); j++)
		{
			emissionVector[0][planeIndices[i] + j] =
				planes[i].getMaterial().emission.x;
			emissionVector[1][planeIndices[i] + j] =
				planes[i].getMaterial().emission.y;
			emissionVector[2][planeIndices[i] + j] =
				planes[i].getMaterial().emission.z;
		}

	switch (MATRIX_EVALUATION)
	{
	case LU_DECOMPOSITION:
		{
		float **kMatrix = genMatrix(elements);
		float **lMatrix = genMatrix(elements);
		float **uMatrix = genMatrix(elements);
		for (int color = 0; color < 3; color++)
		{
			for (int p = 0; p < planeCount; p++)
				for (int e = 0; e < planes[p].elements(); e++)
					for (int x = 0; x < elements; x++)
					{
						int y = planeIndices[p] + e;
						kMatrix[x][y] = x == y ? 1.0f : 0.0f;
						kMatrix[x][y] -= formFactorMatrix[x][y] *
							planes[p].getMaterial().color[color];
					}

			LUDecompose(kMatrix, lMatrix, uMatrix, elements);
			LUSolve(radiosityVector[color], emissionVector[color],
					lMatrix, uMatrix, elements);
		}
		}
		break;
	}

	for (int i = 0; i < planeCount; i++)
	{
		glm::vec3 *planeRadiosities = planes[i].getRadiosities();

		for (int j = 0; j < planes[i].elements(); j++)
		{
			glm::vec3 radiosity(0.0f);
			for (int color = 0; color < 3; color++)
				radiosity[color] =
					radiosityVector[color][planeIndices[i] + j];
			planeRadiosities[j] = radiosity;
		}

		planes[i].generateGrid();
	}

	
	// Create shader program
	ShaderProgram shaderProgram("test.vs", "test.fs");


	// Set OpenGL varriables
	if (WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (CULL_FACE) glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);


	// Set imgui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	bool open = true;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		camera.update(window, 1.0f / 60);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		glm::mat4 proj = glm::perspective(glm::radians(45.0f),
				(float)WIDTH / HEIGHT, 0.1f, 100.0f);

		shaderProgram.setMat4fv("view", camera.getViewMat());
		shaderProgram.setMat4fv("proj", proj);

		for (int i = 0; i < planeCount; i++)
			planes[i].draw(shaderProgram);
        

		// Draw imgui
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		if (open)
			ImGui::ShowDemoWindow(&open);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
