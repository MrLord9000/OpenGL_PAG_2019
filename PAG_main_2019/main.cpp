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
	Shader phong("Shaders/phong_world_space.vert", "Shaders/phong_world_space.frag");
	Shader gouraud("Shaders/gouraud.vert", "Shaders/gouraud.frag");

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

// Camera setup

	Camera mainCamera = Camera(glm::vec3(-5.0f, 5.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// Setup materials

	glm::mat4 model_zero = glm::mat4(1.0f);
	glm::mat4 projection;
	glm::mat4 view = glm::mat4(1.0f);

	Material houseMaterial = Material(&phong);
	houseMaterial.SetModel(&model_zero);
	houseMaterial.SetView(&view);
	houseMaterial.SetProjection(&projection);
	houseMaterial.SetViewPosition(mainCamera.GetPositionPointer());
	houseMaterial.SetVec4(glm::vec4(0.2f, 0.2f, 0.25f, 1.0f), "ambient_color");
	//houseMaterial.SetVec4(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), "directional_color");
	houseMaterial.SetVec3(glm::vec3(-1.0f, 1.0f, -1.0f), "directional_vector");
	houseMaterial.SetVec4(glm::vec4(1.0f, 0.8f, 0.7f, 1.0f), "point_0_color");
	houseMaterial.SetVec3(glm::vec3(10.0f, 5.0f, 10.0f), "point_0_position");
	houseMaterial.SetFloat(1.0f, "specular_intensity");

	Material lightUnlit = Material(&unlitColor);
	lightUnlit.SetModel(&model_zero);
	lightUnlit.SetView(&view);
	lightUnlit.SetProjection(&projection);
	lightUnlit.SetVec4(glm::vec4(0.2f, 0.1f, 0.1f, 1.0f), "color");
	
	Material specularTest = Material(&phong);
	specularTest.SetModel(&model_zero);
	specularTest.SetView(&view);
	specularTest.SetProjection(&projection);
	specularTest.SetVec4(glm::vec4(0.2f, 0.2f, 0.25f, 1.0f), "ambient_color");
	specularTest.SetVec4(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "color");
	specularTest.SetVec4(glm::vec4(0.2f, 0.1f, 0.1f, 1.0f), "point_0_color");
	specularTest.SetFloat(10.0f, "specular_intensity");


	Model lightGizmo = Model("Models/primitives/sphere.obj", &lightUnlit);
	Model specularTestModel = Model("Models/primitives/sphere.obj", &specularTest);
	Model houseModel = Model("Models/house_chunk_01/house_chunk_01.obj", &houseMaterial);
	Model testPlanet = Model("Models/geonosis/geonosis.obj", &houseMaterial);

	// Planets & orbits =====================================================================================

	glm::mat4 matrix = glm::mat4(1.0f);

	// Root **********************************
	GraphNode rootNode = GraphNode(nullptr);

	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode house_01 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_01);
	matrix = glm::translate(matrix, glm::vec3(140.0f, 0.0f, 0.0f));
	GraphNode house_02 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_02);
	matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 140.0f));
	GraphNode house_03 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_03);
	matrix = glm::translate(matrix, glm::vec3(-140.0f, 0.0f, 0.0f));
	GraphNode house_04 = GraphNode(&houseModel, matrix);
	rootNode.AddChild(&house_04);

	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, glm::vec3(10.0f, 5.0f, 10.0f));
	matrix = glm::scale(matrix, glm::vec3(0.1f, 0.1f, 0.1f));
	GraphNode lightGizmoNode = GraphNode(&lightGizmo, matrix);
	rootNode.AddChild(&lightGizmoNode);

	matrix = glm::mat4(1.0f);
	matrix = glm::translate(matrix, glm::vec3(8.0f, 8.0f, 2.0f));
	matrix = glm::scale(matrix, glm::vec3(0.25f, 0.25f, 0.25f));
	GraphNode specularTestNode = GraphNode(&specularTestModel, matrix);
	rootNode.AddChild(&specularTestNode);


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

		ImGui::Text("Cylinder geometry control");
		ImGui::SliderInt("Sides", &sides, 3, 64);
		ImGui::SliderFloat("Height", &height, 0.01f, 10.0f);
		ImGui::SliderFloat("Radius", &radius, 0.01f, 10.0f);
		ImGui::Text("Camera Controls");
		ImGui::SliderFloat3("Base rotation", baseRotation, -180.0f, 180.0f);
		ImGui::SliderFloat3("Base translation", baseTranslation, -5.0f, 30.0f);
		ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 10.0f);
		ImGui::ColorEdit3("Model color", (float*)&shape_color);
		ImGui::ColorEdit3("Background color", (float*)&background_color);
		if (ImGui::Button("Ortographic")) ortographic = !ortographic;
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
		glm::mat4 rootTransform = glm::mat4(1.0f);
		rootNode.Render(rootTransform);


		// ImGUI Render ++++++++++++
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
		//glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
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
