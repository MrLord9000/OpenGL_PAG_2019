#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Thirdparty/ImGUI/imgui.h"
#include "Thirdparty/ImGUI/imgui_impl_glfw.h"
#include "Thirdparty/ImGUI/imgui_impl_opengl3.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <exception>

#include "main.h"
#include "Shader.h"
#include "Model.h"
#include "Material.h"
#include "GraphNode.h"

// Program constants
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

const char* glsl_version = "#version 330";

// Global time variables - maybe not the best practice

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastMouseX = 512;
float lastMouseY = 512;
bool firstMouse = true;
float fov = 80.0f;

int main()
{
	GLFWwindow* window = nullptr;

	if (initEverything(&window))
	{
		throw std::runtime_error("Program failed to initialize!");
	}

// Compiling needed shader programs;

	Shader textureUnlit("Shaders/textureUnlit.vert", "Shaders/textureUnlit.frag");
	Shader unlitColor("Shaders/unlitColor.vert", "Shaders/unlitColor.frag");
	Shader phong("Shaders/blinn-phong.vert", "Shaders/blinn-phong.frag");
	Shader gouraud("Shaders/gouraud.vert", "Shaders/gouraud.frag");


// Camera setup

	Camera mainCamera = Camera(glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// Point lights

	glm::vec3 pointLightPositions[4] =
	{
		glm::vec3(10.0f, 5.0f, 10.0f),
		glm::vec3(-10.0f, 5.0f, 10.0f),
		glm::vec3(10.0f, 5.0f, -10.0f),
		glm::vec3(-10.0f, 5.0f, -10.0f)
	};

	glm::vec3 spotLightPositions[2] =
	{
		glm::vec3(8.0f, 4.0f, 8.0f),
		glm::vec3(-8.0f, 4.0f, -8.0f)
	};

// Setup materials

	glm::mat4 model_zero = glm::mat4(1.0f);
	glm::mat4 projection;
	glm::mat4 view = glm::mat4(1.0f);

	Material houseMaterial = Material(&phong);
	houseMaterial.SetModel(&model_zero);
	houseMaterial.SetView(&view);
	houseMaterial.SetProjection(&projection);
	houseMaterial.SetViewPosition(mainCamera.GetPositionPointer());
	houseMaterial.SetFloat(128.0f, "material.shininess");
	// Directional light setup
	houseMaterial.SetVec3(glm::vec3(-1.0f, -1.0f, -1.0f), "directionalLight.direction");
	houseMaterial.SetVec3(glm::vec3(0.1f, 0.1f, 0.15f), "directionalLight.ambient");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "directionalLight.diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "directionalLight.specular");
	houseMaterial.SetFloat(1.0f, "directionalLight.intensity");
	// Point lights setup
	// 1
	houseMaterial.SetVec3(pointLightPositions[0], "pointLights[0].position");
	houseMaterial.SetVec3(glm::vec3(0.2f, 0.2f, 0.2f), "pointLights[0].ambient");
	houseMaterial.SetVec3(glm::vec3(1.0f, 0.0f, 0.0f), "pointLights[0].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "pointLights[0].specular");
	houseMaterial.SetFloat(1.0f, "pointLights[0].constant");
	houseMaterial.SetFloat(0.09f, "pointLights[0].linear");
	houseMaterial.SetFloat(0.032f, "pointLights[0].quadratic");
	houseMaterial.SetFloat(1.0f, "pointLights[0].intensity");
	// 2
	houseMaterial.SetVec3(pointLightPositions[1], "pointLights[1].position");
	houseMaterial.SetVec3(glm::vec3(0.2f, 0.2f, 0.2f), "pointLights[1].ambient");
	houseMaterial.SetVec3(glm::vec3(0.0f, 1.0f, 0.0f), "pointLights[1].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "pointLights[1].specular");
	houseMaterial.SetFloat(1.0f, "pointLights[1].constant");
	houseMaterial.SetFloat(0.09f, "pointLights[1].linear");
	houseMaterial.SetFloat(0.032f, "pointLights[1].quadratic");
	houseMaterial.SetFloat(1.0f, "pointLights[1].intensity");
	// 3
	houseMaterial.SetVec3(pointLightPositions[2], "pointLights[2].position");
	houseMaterial.SetVec3(glm::vec3(0.2f, 0.2f, 0.2f), "pointLights[2].ambient");
	houseMaterial.SetVec3(glm::vec3(0.0f, 0.0f, 1.0f), "pointLights[2].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "pointLights[2].specular");
	houseMaterial.SetFloat(1.0f, "pointLights[2].constant");
	houseMaterial.SetFloat(0.09f, "pointLights[2].linear");
	houseMaterial.SetFloat(0.032f, "pointLights[2].quadratic");
	houseMaterial.SetFloat(1.0f, "pointLights[2].intensity");
	// 4
	houseMaterial.SetVec3(pointLightPositions[3], "pointLights[3].position");
	houseMaterial.SetVec3(glm::vec3(0.2f, 0.2f, 0.2f), "pointLights[3].ambient");
	houseMaterial.SetVec3(glm::vec3(1.0f, 0.3f, 0.2f), "pointLights[3].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "pointLights[3].specular");
	houseMaterial.SetFloat(1.0f, "pointLights[3].constant");
	houseMaterial.SetFloat(0.09f, "pointLights[3].linear");
	houseMaterial.SetFloat(0.032f, "pointLights[3].quadratic");
	houseMaterial.SetFloat(1.0f, "pointLights[3].intensity");
	// Static flashlight spot light values
	houseMaterial.SetFloat(glm::cos(glm::radians(17.5f)), "spotLights[0].cutoff"); // We set the cosine of value in radians to save performance
	houseMaterial.SetFloat(glm::cos(glm::radians(22.5f)), "spotLights[0].outerCutoff");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[0].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[0].specular");
	houseMaterial.SetFloat(1.0f, "spotLights[0].constant");
	houseMaterial.SetFloat(0.09f, "spotLights[0].linear");
	houseMaterial.SetFloat(0.032f, "spotLights[0].quadratic");
	houseMaterial.SetFloat(1.0f, "spotLights[0].intensity");
	// 1
	houseMaterial.SetVec3(spotLightPositions[0], "spotLights[1].position");
	houseMaterial.SetFloat(glm::cos(glm::radians(25.0f)), "spotLights[1].cutoff"); // We set the cosine of value in radians to save performance
	houseMaterial.SetFloat(glm::cos(glm::radians(35.0f)), "spotLights[1].outerCutoff");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[1].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[1].specular");
	houseMaterial.SetFloat(1.0f, "spotLights[1].constant");
	houseMaterial.SetFloat(0.09f, "spotLights[1].linear");
	houseMaterial.SetFloat(0.032f, "spotLights[1].quadratic");
	houseMaterial.SetFloat(1.0f, "spotLights[1].intensity");
	// 2
	houseMaterial.SetVec3(spotLightPositions[1], "spotLights[2].position");
	houseMaterial.SetFloat(glm::cos(glm::radians(25.0f)), "spotLights[2].cutoff"); // We set the cosine of value in radians to save performance
	houseMaterial.SetFloat(glm::cos(glm::radians(35.0f)), "spotLights[2].outerCutoff");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[2].diffuse");
	houseMaterial.SetVec3(glm::vec3(1.0f, 1.0f, 1.0f), "spotLights[2].specular");
	houseMaterial.SetFloat(1.0f, "spotLights[2].constant");
	houseMaterial.SetFloat(0.09f, "spotLights[2].linear");
	houseMaterial.SetFloat(0.032f, "spotLights[2].quadratic");
	houseMaterial.SetFloat(1.0f, "spotLights[2].intensity");

	// 1
	Material lightUnlit0 = Material(&unlitColor);
	lightUnlit0.SetModel(&model_zero);
	lightUnlit0.SetView(&view);
	lightUnlit0.SetProjection(&projection);
	lightUnlit0.SetVec4(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "color");
	// 2
	Material lightUnlit1 = Material(&unlitColor);
	lightUnlit1.SetModel(&model_zero);
	lightUnlit1.SetView(&view);
	lightUnlit1.SetProjection(&projection);
	lightUnlit1.SetVec4(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), "color");
	// 3
	Material lightUnlit2 = Material(&unlitColor);
	lightUnlit2.SetModel(&model_zero);
	lightUnlit2.SetView(&view);
	lightUnlit2.SetProjection(&projection);
	lightUnlit2.SetVec4(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), "color");
	// 4
	Material lightUnlit3 = Material(&unlitColor);
	lightUnlit3.SetModel(&model_zero);
	lightUnlit3.SetView(&view);
	lightUnlit3.SetProjection(&projection);
	lightUnlit3.SetVec4(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "color");

	Model lightGizmo0 = Model("Models/primitives/sphere.obj", &lightUnlit0);
	Model lightGizmo1 = Model("Models/primitives/sphere.obj", &lightUnlit1);
	Model lightGizmo2 = Model("Models/primitives/sphere.obj", &lightUnlit2);
	Model lightGizmo3 = Model("Models/primitives/sphere.obj", &lightUnlit3);
	Model houseModel = Model("Models/house_chunk_01/house_chunk_01.obj", &houseMaterial);
	Model testPlanet = Model("Models/geonosis/geonosis.obj", &houseMaterial);

	// Planets & orbits =====================================================================================

	glm::mat4 matrix = glm::mat4(1.0f);

	// Root **********************************
	GraphNode rootNode = GraphNode(nullptr);

	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	// 1
	GraphNode house_01 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_01);
	// 2
	matrix = glm::translate(matrix, glm::vec3(140.0f, 0.0f, 0.0f));
	GraphNode house_02 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_02);
	// 3
	matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 140.0f));
	GraphNode house_03 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_03);
	// 4
	matrix = glm::translate(matrix, glm::vec3(-140.0f, 0.0f, 0.0f));
	GraphNode house_04 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_04);
	// 5
	matrix = glm::translate(matrix, glm::vec3(-140.0f, 0.0f, 0.0f));
	GraphNode house_05 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_05);
	// 6
	matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -140.0f));
	GraphNode house_06 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_06);
	// 7
	matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, -140.0f));
	GraphNode house_07 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_07);
	// 8
	matrix = glm::translate(matrix, glm::vec3(140.0f, 0.0f, 0.0f));
	GraphNode house_08 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_08);
	// 9
	matrix = glm::translate(matrix, glm::vec3(140.0f, 0.0f, 0.0f));
	GraphNode house_09 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_09);

	// Point light visualization
	// 0
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, pointLightPositions[0]);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode lightGizmoNode0 = GraphNode(&lightGizmo0, matrix);
	rootNode.AddChild(&lightGizmoNode0);
	// 1
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, pointLightPositions[1]);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode lightGizmoNode1 = GraphNode(&lightGizmo1, matrix);
	rootNode.AddChild(&lightGizmoNode1);
	// 2
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, pointLightPositions[2]);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode lightGizmoNode2 = GraphNode(&lightGizmo2, matrix);
	rootNode.AddChild(&lightGizmoNode2);
	// 3
	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, pointLightPositions[3]);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode lightGizmoNode3 = GraphNode(&lightGizmo3, matrix);
	rootNode.AddChild(&lightGizmoNode3);


	GLuint flashlightTexture = Model::TextureFromFile("flashlight.jpg", "Textures");
	houseMaterial.SetTextureID(flashlightTexture, "spotLight.cookie");


// Runtime rendering properties (mainly for fun)

	bool show_demo_window = false, wireframeMode = false, visualizeNormals = false;
	int sides = 3;
	float height = 3.0f, radius = 1.0f;
	ImVec4 shape_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 background_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	bool ortographic = false;
	float rotationSpeed = 1.0f;
	float baseRotation[3] = { 0.0f, 0.0f, 0.0f };
	float baseTranslation[3] = { 0.0f, 0.0f, 5.0f };
	float scale = 1.0f;

// Light control
	bool pointEnabled = true, spot0Enabled = true, spot1Enabled = true, directionalEnabled = true, rainbow = true;
	float pointColor[3] = { 0.0f, 0.0f, 0.0f };
	float spot0Color[3] = { 0.0f, 0.0f, 0.0f };
	float spot1Color[3] = { 0.0f, 0.0f, 0.0f };
	float directionalColor[3] = { 0.0f, 0.0f, 0.0f };

	float spot0Direction[3] = { -1.0f, -1.0f, -1.0f };
	float spot1Direction[3] = { 1.0f, -1.0f, 1.0f };

// ===================================================================================================================================================
// Render Loop =======================================================================================================================================

	while (!glfwWindowShouldClose(window))
	{
		// Handle time and deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle input ++++++++++++++
		processInput(window, mainCamera);
		mainCamera.Update();
#pragma region ImGUI
		// ImGUI operations ++++++++++++++

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Begin("Options");

		ImGui::Checkbox("Demo Window", &show_demo_window);

		ImGui::Text("Point light");
		ImGui::Checkbox("Point Enabled", &pointEnabled);
		ImGui::Checkbox("Rainbow", &rainbow);
		ImGui::SliderFloat3("Color", pointColor, 0.0f, 1.0f);
	
		ImGui::Text("Spot light A");
		ImGui::Checkbox("Spot A Enabled", &spot0Enabled);
		ImGui::SliderFloat3("Spot A Color", spot0Color, 0.0f, 1.0f);
		ImGui::SliderFloat3(" Spot A Direction", spot0Direction, -1.0f, 1.0f);

		ImGui::Text("Spot light B");
		ImGui::Checkbox("Spot B Enabled", &spot1Enabled);
		ImGui::SliderFloat3("Spot B Color", spot1Color, 0.0f, 1.0f);
		ImGui::SliderFloat3("Spot B Direction", spot1Direction, -1.0f, 1.0f);

		ImGui::Text("Directional light");
		ImGui::Checkbox("Directional Enabled", &directionalEnabled);
		ImGui::SliderFloat3("Directional Color", directionalColor, 0.0f, 1.0f);

		ImGui::Text("Camera Controls");
		//if (ImGui::Button("Ortographic")) ortographic = !ortographic;
		if (ImGui::Button("Toggle Wireframe")) wireframeMode = !wireframeMode;
		//if (ImGui::Button("Visualize Normals")) visualizeNormals = !visualizeNormals;

		// A pretty cool thing for debugging really
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		
#pragma endregion

		// Process rendering commands +++++++++++
		
		glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Remember to clear the depth buffer if you use depth testing
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW); // GL_CW - clockwise; GL_CCW - counter-clockwise
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_FRAMEBUFFER_SRGB);

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		// VIEW
		view = Camera::viewMatrix;

		// PROJECTION
		if (!ortographic)
			projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 1000.0f);
		else
			projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 100.0f);

		// Perform per-frame transformations on objects
		if (!pointEnabled)
		{
			houseMaterial.SetFloat(0.0f, "pointLights[3].intensity");
			lightUnlit3.SetVec4(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "color");
		}
		else
		{
			houseMaterial.SetFloat(1.0f, "pointLights[3].intensity");
			// Rainbow light
			if (rainbow)
			{
				houseMaterial.SetVec3(glm::vec3(glm::abs(glm::sin(currentFrame)), glm::abs(glm::sin(currentFrame + glm::radians(45.0f))), glm::abs(glm::sin(currentFrame + glm::radians(90.0f)))), "pointLights[3].diffuse");
				lightUnlit3.SetVec4(glm::vec4(glm::abs(glm::sin(currentFrame)), glm::abs(glm::sin(currentFrame + glm::radians(45.0f))), glm::abs(glm::sin(currentFrame + glm::radians(90.0f))), 1.0f), "color");
			}
			else
			{
				houseMaterial.SetVec3(glm::vec3(pointColor[0], pointColor[1], pointColor[2]), "pointLights[3].diffuse");
				lightUnlit3.SetVec4(glm::vec4(pointColor[0], pointColor[1], pointColor[2], 1.0f), "color");
			}
		}

		if (!spot0Enabled)
		{
			houseMaterial.SetFloat(0.0f, "spotLights[1].intensity");
		}
		else
		{
			houseMaterial.SetFloat(1.0f, "spotLights[1].intensity");
			houseMaterial.SetVec3(glm::vec3(spot0Direction[0], spot0Direction[1], spot0Direction[2]), "spotLights[1].direction");
			houseMaterial.SetVec3(glm::vec3(spot0Color[0], spot0Color[1], spot0Color[2]), "spotLights[1].diffuse");
		}

		if (!spot1Enabled)
		{
			houseMaterial.SetFloat(0.0f, "spotLights[2].intensity");
		}
		else
		{
			houseMaterial.SetFloat(1.0f, "spotLights[2].intensity");
			houseMaterial.SetVec3(glm::vec3(spot1Direction[0], spot1Direction[1], spot1Direction[2]), "spotLights[2].direction");
			houseMaterial.SetVec3(glm::vec3(spot1Color[0], spot1Color[1], spot1Color[2]), "spotLights[2].diffuse");
		}


		if (!directionalEnabled)
		{
			houseMaterial.SetFloat(0.0f, "directionalLight.intensity");
		}
		else
		{
			houseMaterial.SetFloat(1.0f, "directionalLight.intensity");
		}
		// Flashlight transform
		houseMaterial.SetVec3(mainCamera.GetPosition(), "spotLights[0].position");
		houseMaterial.SetVec3(mainCamera.GetFront(), "spotLights[0].direction");

		
		lightGizmoNode3.SetLocalPosition(glm::vec3(glm::cos(currentFrame) * 7.0f, 4.0f, glm::sin(currentFrame) * 7.0f));
		houseMaterial.SetVec3(glm::vec3(glm::cos(currentFrame) * 7.0f, 4.0f, glm::sin(currentFrame) * 7.0f), "pointLights[3].position");

		glm::mat4 rootTransform = glm::mat4(1.0f);
		rootNode.Render(rootTransform);

		// ImGUI Render ++++++++++++
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glEnable(GL_FRAMEBUFFER_SRGB);
		// Check and call events, swap buffers ++++++++++
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

// ===================================================================================================================================================

	// Cleanup after ImGUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Properly clean/delete all of GLFW's resources
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// Process input function to easily handle key input
void processInput(GLFWwindow* window, Camera &camera)
{
	// Handle application exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		Camera::enabled = false;
		//glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Camera::enabled = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// Handle WSAD input
	glm::vec3 targetVec = glm::vec3(0.0f);
	float cameraSpeed = camera.GetSpeed() * deltaTime;
	// W key
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		targetVec += cameraSpeed * camera.GetFront();
	}
	// S key
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		targetVec -= cameraSpeed * camera.GetFront();
	}
	// A key
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		targetVec -= glm::normalize(glm::cross(camera.GetFront(), camera.GetUp())) * cameraSpeed;
	}
	// D key
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		targetVec += glm::normalize(glm::cross(camera.GetFront(), camera.GetUp())) * cameraSpeed;
	}
	// Q key
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		targetVec -= camera.GetUp() * cameraSpeed;
	}
	// E key
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		targetVec += camera.GetUp() * cameraSpeed;
	}
	// Shift key
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		camera.SetSpeed(5.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		camera.SetSpeed(2.0f);
	}

	targetVec += camera.GetPosition();
	camera.SetPosition(targetVec);

	// Handle mouse input
	glm::vec3 directionVec = glm::vec3(0.0f);
	directionVec.x = cos(glm::radians(Camera::pitch)) * cos(glm::radians(Camera::yaw));
	directionVec.y = sin(glm::radians(Camera::pitch));
	directionVec.z = cos(glm::radians(Camera::pitch)) * sin(glm::radians(Camera::yaw));
	directionVec = glm::normalize(directionVec);
	camera.SetFront(directionVec);
}

// Callback function for handling mouse input
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastMouseX;
	float yOffset = lastMouseY - ypos;

	lastMouseX = xpos;
	lastMouseY = ypos;

	float sensitivity = 1.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	Camera::yaw += xOffset;
	Camera::pitch += yOffset;

	// Constrain the pitch value
	if (Camera::pitch > 89.0f)
		Camera::pitch = 89.0f;
	if (Camera::pitch < -89.0f)
		Camera::pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 90.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 90.0f)
		fov = 90.0f;
}

// Callback function that gets called each time the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

int initEverything(GLFWwindow** window)
{
// GLFW init ============================================================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	// Creating a glfw window
	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL PAG 1", nullptr, nullptr);
	if (*window == nullptr)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(*window);
	glfwSetWindowTitle(*window, "OpenGL is the best XD");
	glfwSwapInterval(0);
// GLAD init ============================================================================

	// GLFW gives us the correct function based on which OS we're compiling for
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return -2;
	}

// OpenGL setup ==========================================================================

	// Setting the size of the rendering window for OpenGL
	// First two parameters: location of the lower left corner of the window
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Set the callback function to call on every window resize
	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
	glfwSetCursorPosCallback(*window, mouse_callback);
	glfwSetScrollCallback(*window, scroll_callback);
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

// ImGui setup ===========================================================================

	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(*window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return 0;
}
