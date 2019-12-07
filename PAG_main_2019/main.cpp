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
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 1024;

const char* glsl_version = "#version 330";

int main()
{
	std::cout << "Wariant: " << 216835 % 4 + 1 << "\n";

	GLFWwindow* window = nullptr;

	if (initEverything(&window))
	{
		throw std::runtime_error("Program failed to initialize!");
	}

	//// Additional check how many shader attributes the graphics card allows (usually max 16)
	//int nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Max vertex attribs supported: " << nrAttributes << "\n";
	int maxVertices;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxVertices);
	std::cout << "Max geometry output: " << maxVertices << "\n";

// Compiling needed shader programs;

	Shader textureUnlit("Shaders/textureUnlit.vert", "Shaders/textureUnlit.frag");
	Shader geometryUnlit("Shaders/geometryUnlit.vert", "Shaders/geometryUnlit.frag", "Shaders/proceduralCylinder.geom");
	Shader orbitUnlit("Shaders/geometryUnlit.vert", "Shaders/geometryUnlit.frag", "Shaders/proceduralOrbit.geom");
	Shader normalVisualization("Shaders/normalVisualization.vert", "Shaders/normalVisualization.frag", "Shaders/normalVisualization.geom");

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

// Setup materials

	glm::mat4 model_zero = glm::mat4(1.0f);
	glm::mat4 projection;
	glm::mat4 view = glm::mat4(1.0f);

	glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	Material sun_mat = Material(&textureUnlit);
	sun_mat.SetVec4(&white, "ourColor");
	sun_mat.SetView(&view);
	sun_mat.SetProjection(&projection);
	Material geonosis_mat = Material(&textureUnlit);
	geonosis_mat.SetVec4(&white, "ourColor");
	geonosis_mat.SetView(&view);
	geonosis_mat.SetProjection(&projection);
	Material coruscant_mat = Material(&textureUnlit);
	coruscant_mat.SetVec4(&white, "ourColor");
	coruscant_mat.SetView(&view);
	coruscant_mat.SetProjection(&projection);
	Material coruscant_mat_blue = Material(&textureUnlit);
	coruscant_mat_blue.SetVec4(&blue, "ourColor");
	coruscant_mat_blue.SetView(&view);
	coruscant_mat_blue.SetProjection(&projection);
	Material moon_mat = Material(&textureUnlit);
	moon_mat.SetVec4(&white, "ourColor");
	moon_mat.SetView(&view);
	moon_mat.SetProjection(&projection);
	Material cylinder_mat_white = Material(&geometryUnlit);
	cylinder_mat_white.SetVec4(&white, "color");
	cylinder_mat_white.SetView(&view);
	cylinder_mat_white.SetProjection(&projection);
	cylinder_mat_white.SetFloat(&height, "height");
	cylinder_mat_white.SetFloat(&radius, "radius");
	cylinder_mat_white.SetInt(&sides, "sides");
	Material cylinder_mat_blue = Material(&geometryUnlit);
	cylinder_mat_blue.SetVec4(&blue, "color");
	cylinder_mat_blue.SetView(&view);
	cylinder_mat_blue.SetProjection(&projection);
	cylinder_mat_blue.SetFloat(&height, "height");
	cylinder_mat_blue.SetFloat(&radius, "radius");
	cylinder_mat_blue.SetInt(&sides, "sides");
	Material orbit_mat = Material(&orbitUnlit);
	orbit_mat.SetVec4(&white, "color");
	orbit_mat.SetModel(&model_zero);
	orbit_mat.SetView(&view);
	orbit_mat.SetProjection(&projection);
	float orbit_radius = 10.0f;
	int orbit_sides = 64;
	orbit_mat.SetFloat(&orbit_radius, "radius");
	orbit_mat.SetInt(&orbit_sides, "sides");
	Material orbit_mat_blue = Material(&orbitUnlit);
	orbit_mat_blue.SetVec4(&blue, "color");
	orbit_mat_blue.SetModel(&model_zero);
	orbit_mat_blue.SetView(&view);
	orbit_mat_blue.SetProjection(&projection);
	orbit_mat_blue.SetFloat(&orbit_radius, "radius");
	orbit_mat_blue.SetInt(&orbit_sides, "sides");

	Model sun("Models/sun/13913_Sun_v2_l3.obj", &sun_mat);
	Model geonosis("Models/geonosis/geonosis.obj", &geonosis_mat);
	Model cylinder(nullptr, &cylinder_mat_blue, true);
	Model orbit(nullptr, &orbit_mat, true);
	Model orbit_blue(nullptr, &orbit_mat_blue, true);

	// Planets & orbits =====================================================================================

	glm::mat4 matrix = glm::mat4(1.0f);

	// Root **********************************
	matrix = glm::mat4(1.0f);
	matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GraphNode rootNode = GraphNode(nullptr, matrix);

	// Sun **********************************
	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(0.0005f, 0.0005f, 0.0005f));
	GraphNode sun_node(&sun, matrix);

	rootNode.AddChild(&sun_node);

	// +++++++++++++++++++ GEONOSIS ++++++++++++++++++++++
	// Geonosis pivot **********************************
	matrix = glm::mat4(1.0f);
	GraphNode geonosis_pivot(nullptr, matrix);

	sun_node.AddChild(&geonosis_pivot);

	// Geonosis orbit **********************************
	
	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(300.0f, 300.0f, 300.0f));
	matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GraphNode geonosis_orbit = GraphNode(&orbit, matrix);
	sun_node.AddChild(&geonosis_orbit);

	// Geonosis **********************************
	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(3.0f, 3.0f, 3.0f));
	matrix = glm::translate(matrix, glm::vec3(1000.0f, 0.0f, 0.0f));
	GraphNode geonosis_node(&geonosis, matrix);

	geonosis_pivot.AddChild(&geonosis_node);

		// +++++++++++++++ CYLINDER 0 +++++++++++++++++++++++++
		// Cylinder 0 pivot **********************************	
		matrix = glm::mat4(1.0f);
		GraphNode cylinder_0_pivot(nullptr, matrix);

		geonosis_node.AddChild(&cylinder_0_pivot);

		// Cylinder 0 orbit **********************************
		matrix = glm::mat4(1.0f);
		matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		matrix = glm::scale(matrix, glm::vec3(30.0f, 30.0f, 30.0f));
		GraphNode cylinder_0_orbit = GraphNode(&orbit_blue, matrix);
		geonosis_node.AddChild(&cylinder_0_orbit);

		// Cylinder 0 *****************************************
		matrix = glm::mat4(1.0f);
		matrix = glm::scale(matrix, glm::vec3(100.0f, 100.0f, 100.0f));
		matrix = glm::translate(matrix, glm::vec3(3.0f, 0.0f, 0.0f));
		GraphNode cylinder_0(&cylinder, matrix);

		cylinder_0_pivot.AddChild(&cylinder_0);

	// +++++++++++++++++++ CORUSCANT ++++++++++++++++++++++
	// Coruscant pivot **********************************
	matrix = glm::mat4(1.0f);
	GraphNode coruscant_pivot(nullptr, matrix);

	sun_node.AddChild(&coruscant_pivot);

	// Coruscant orbit **********************************

	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(180.0f, 180.0f, 180.0f));
	matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GraphNode coruscant_orbit = GraphNode(&orbit, matrix);
	sun_node.AddChild(&coruscant_orbit);

	// Coruscant **********************************
	matrix = glm::mat4(1.0f);
	matrix = glm::scale(matrix, glm::vec3(3.0f, 3.0f, 3.0f));
	matrix = glm::translate(matrix, glm::vec3(600.0f, 0.0f, 0.0f));
	GraphNode coruscant_node(&geonosis, matrix);

	coruscant_pivot.AddChild(&coruscant_node);

// ===================================================================================================================================================
// Render Loop =======================================================================================================================================

	while (!glfwWindowShouldClose(window))
	{
		// Handle input ++++++++++++++
		processInput(window);

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
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CW); // GL_CW - clockwise; GL_CCW - counter-clockwise
		glEnable(GL_DEPTH_TEST);

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		// VIEW
		view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(-baseTranslation[0], -baseTranslation[1], -baseTranslation[2])); // Note that we're translating in the opposite direction we want our camera to move
		view = glm::rotate(view, glm::radians(baseRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(baseRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		view = glm::rotate(view, glm::radians(baseRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		// PROJECTION
		if (!ortographic)
			projection = glm::perspective(glm::radians(45.0f), (float)(WINDOW_WIDTH / WINDOW_HEIGHT), 0.1f, 100.0f);
		else
			projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 100.0f);

		// Perform per-frame transformations on objects
		geonosis_pivot.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(glfwGetTime() / 10.0f));
		geonosis_node.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(glfwGetTime() * 3.0f));
		coruscant_pivot.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), -glm::radians(glfwGetTime()));
		coruscant_node.Rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(glfwGetTime() * 3.0f));
		glm::mat4 rootTransform = glm::mat4(1.0f);
		rootNode.Render(rootTransform);

		/*if (visualizeNormals)
		{
			normalVisualization.Use();
			normalVisualization.SetMat4("projection", projection);
			normalVisualization.SetMat4("view", view);
			rootNode.Render(rootTransform, normalVisualization);
		}*/


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

void createMenger(glm::mat4 &model, glm::vec3 cubePositions[], Shader &shaderProgram, int iteration, int iterations)
{
	if (iteration == iterations)
	{
		shaderProgram.SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		return;
	}
	else
	{
		for (size_t i = 0; i < 20; i++)
		{
			glm::mat4 model_local = glm::mat4(1.0f);
			model_local = glm::scale(model_local, glm::vec3(1.0f / 3.0f));
			model_local = glm::translate(model_local, cubePositions[i]);
			model_local = model * model_local;
			createMenger(model_local, cubePositions, shaderProgram, iteration + 1, iterations);
		}
	}
}

// Generates vertices and indices for fractal
void createCarpet(std::vector<float> &vertices, std::vector<unsigned int> &indices, int iterations)
{
	vertices.clear();
	indices.clear();
	carpetFunction(vertices, indices, 2.0f, 2.0f, 0.0f, 0.0f, 0, iterations);
}

void carpetFunction(std::vector<float> &vertices, std::vector<unsigned int> &indices, float width, float height, float x, float y, int iteration, int iterations)
{
	float wn = width / 3.0f, hn = height / 3.0f;
	createQuad2D(x, y, wn, hn, vertices, indices);
	if (iteration == iterations) return;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 1 && j == 1) continue;
			carpetFunction(vertices, indices, wn, hn, x + wn * (i - 1), y + hn * (j - 1), iteration + 1, iterations);
		}
	}
}

// Process input function to easily handle key input
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

// Callback function that gets called each time the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void createQuad2D(float x, float y, float width, float height, std::vector<float> &vertices, std::vector<unsigned int> &indices)
{
	float vertices_quad[] = 
	{
		x + width / 2.0f, y + height / 2.0f, 0.0f, // top right vertex
		x + width / 2.0f, y - height / 2.0f, 0.0f, // bottom right vertex
		x - width / 2.0f, y - height / 2.0f, 0.0f, // bottom left vertex
		x - width / 2.0f, y + height / 2.0f, 0.0f  // top left vertex
	};

	for (float vertex : vertices_quad)
	{
		vertices.push_back(vertex);
	}

	int vertices_count = vertices.size() / 3;
	float indices_quad[] =
	{
		vertices_count - 4, vertices_count - 3, vertices_count - 1,
		vertices_count - 3, vertices_count - 2, vertices_count - 1
	};

	for (float index : indices_quad)
	{
		indices.push_back(index);
	}
}

int initEverything(GLFWwindow** window)
{
// GLFW init ============================================================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creating a glfw window
	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL PAG 1", nullptr, nullptr);
	if (*window == nullptr)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(*window);

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
