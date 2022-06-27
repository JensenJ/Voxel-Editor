#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include "Rendering/ShaderLoader.h"
#include "Rendering/RawModel.h"
#include "Rendering/EntityRenderer.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/MeshRendererComponent.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/EntityRegistry.h"
#include "InputManager.h"

int screenWidth = 1920;
int screenHeight = 1080;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool mouseLocked = true;
bool wireframeMode = false;

const char* glsl_version = "#version 410";

Entity* camera;
float lastMouseX = screenWidth / 2.0f;
float lastMouseY = screenHeight / 2.0f;
bool firstMouse = true;

InputManager inputManager;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* InitialiseOpenGL();
void InitialiseImGui(GLFWwindow* window);

GLFWwindow* window;

void ToggleMouseCursor();
void ToggleWireframeMode();
void CloseWindow();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, width, height);
}

GLFWwindow* InitialiseOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Editor", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}

void InitialiseImGui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

GLFWwindow* GetWindow()
{
	return nullptr;
}

int main()
{
	//Create OpenGL window
	GLFWwindow* window = InitialiseOpenGL();
	if (window == nullptr) { return -1; }
	std::cout << "Created Window" << std::endl;
	
	//Initialise window
	InitialiseImGui(window);

	//Load shaders
	bool success = false;
	std::string vertexPath = std::filesystem::current_path().string() + "\\src\\vertexShader.txt";
	std::string fragmentPath = std::filesystem::current_path().string() + "\\src\\fragmentShader.txt";
	Shader shaderProgram = ShaderLoader::CreateShaderProgram(vertexPath.c_str(), fragmentPath.c_str(), success);

	//If shader compilation/linking failed
	if (!success)
	{
		std::cout << "Failed to create shaders" << std::endl;
		glfwTerminate();
		return -2;
	}
	std::cout << "Created Shaders" << std::endl;

	Vertex vertexArray[] = {
		//Positions									//Colours
		{glm::vec<3, double>(0.5,  0.5, 0.5),		glm::vec3(0.0f, 0.0f, 0.0f)}, //Front Face top right
		{glm::vec<3, double>(0.5, -0.5, 0.5),		glm::vec3(1.0f, 0.0f, 0.0f)}, //Front face bottom right
		{glm::vec<3, double>(-0.5, -0.5, 0.5),		glm::vec3(0.0f, 1.0f, 0.0f)}, //Front face bottom left
		{glm::vec<3, double>(-0.5,  0.5, 0.5),		glm::vec3(0.0f, 0.0f, 1.0f)}, //Front face top left

		{glm::vec<3, double>(0.5,  0.5, -0.5),		glm::vec3(1.0f, 1.0f, 0.0f)}, //Back Face top right
		{glm::vec<3, double>(0.5, -0.5, -0.5),		glm::vec3(0.0f, 1.0f, 1.0f)}, //Back face bottom right
		{glm::vec<3, double>(-0.5, -0.5, -0.5),		glm::vec3(1.0f, 0.0f, 1.0f)}, //Back face bottom left
		{glm::vec<3, double>(-0.5,  0.5, -0.5),		glm::vec3(1.0f, 1.0f, 1.0f)} //Back face top left
	};

	unsigned int indices[] = {
	3, 1, 0,   // first triangle - front top right, front bottom right, front top left
	3, 2, 1,   // second triangle - front bottom right, front bottom left, front top left
	4, 5, 7,   // first triangle - back top right, back bottom right, back top left
	5, 6, 7,   // second triangle - back bottom right, back bottom left, back top left
	0, 5, 4,   // first triangle - front top right, back bottom right, back top right
	1, 5, 0,   // second triangle - front bottom right, back bottom right, front top right
	7, 6, 3,   // first triangle - front top left, back bottom left, back top left
	3, 6, 2,   // second triangle - front bottom left, back bottom left, front top left
	7, 3, 4,   // first triangle - back top left, front top left, back top right
	3, 0, 4,   // second triangle - back top right, front top right, front top left
	5, 2, 6,   // first triangle - back bottom left, front bottom left, back bottom right
	5, 1, 2,   // second triangle - back bottom right, front bottom right, front bottom left
	};

	std::vector<Vertex> verticesVector(std::begin(vertexArray), std::end(vertexArray));
	std::vector<unsigned int> indicesVector(std::begin(indices), std::end(indices));

	RawModel testModel = RawModel(verticesVector, indicesVector);
	EntityRenderer renderer = EntityRenderer();
	EntityRegistry entityRegistry = EntityRegistry();

	//Basic Input binding
	inputManager = InputManager();
	inputManager.BindNewKey(GLFW_KEY_0, GLFW_PRESS, 0, ToggleMouseCursor);
	inputManager.BindNewKey(GLFW_KEY_9, GLFW_PRESS, 0, ToggleWireframeMode);
	inputManager.BindNewKey(GLFW_KEY_ESCAPE, GLFW_PRESS, 0, CloseWindow);

	//Camera settings
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float MOVEMENTSPEED = 5.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;

	camera = entityRegistry.CreateEntity();
	camera->AddComponent<CameraComponent>(glm::vec3(0.0f, 0.0f, 0.0f), YAW, PITCH, MOVEMENTSPEED, SENSITIVITY, ZOOM);

	//Create entities
	for (int x = 0; x < 30; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			for (int z = 0; z < 100; z++)
			{
				Entity* entity = entityRegistry.CreateEntity();
				entity->AddComponent<TransformComponent>(glm::vec3(x, y, z));
				entity->AddComponent<MeshRendererComponent>(&testModel);
			}
		}
	}
	
	std::vector<TransformComponent*> allTransforms = entityRegistry.GetAllComponentsOfType<TransformComponent>();
	std::vector<MeshRendererComponent*> allMeshRenderers = entityRegistry.GetAllComponentsOfType<MeshRendererComponent>();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	while (!glfwWindowShouldClose(window))
	{
		//Calculate delta time
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Calculate FPS
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0)
		{
			std::string title = "Voxel Editor [";
			title += std::to_string(nbFrames);
			title += " FPS, ";
			title += std::to_string(1000.0 / double(nbFrames));
			title += " ms]";
			glfwSetWindowTitle(window, title.c_str());

			nbFrames = 0;
			lastTime += 1.0;
		}

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		CameraComponent* camComponent = camera->GetComponent<CameraComponent>();
		if (camera == nullptr) {
			glfwTerminate();
			std::cout << "Failed to get camera component" << std::endl;
			return -3; 
		};

		//Update all components in the entity registry
		entityRegistry.UpdateAllComponents(deltaTime);
		camComponent->ProcessInput(window);

		glm::mat4 view = camComponent->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camComponent->GetZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		shaderProgram.SetMat4("view", view);
		shaderProgram.SetMat4("projection", projection);

		shaderProgram.Use();

		for (unsigned int i = 0; i < allMeshRenderers.size(); i++)
		{
			Entity* entity = entityRegistry.GetEntityFromID(allMeshRenderers[i]->GetOwningEntityID());
			if (entity == nullptr) 
			{ 
				std::cout << "Entity had invalid id" << std::endl;
				continue; 
			}

			TransformComponent* transform = entity->GetComponent<TransformComponent>();
			if (transform == nullptr) 
			{ 
				std::cout << "Entity has invalid transform" << std::endl;
				continue;
			}

			glm::mat4 model = transform->GetTransform();
			shaderProgram.SetMat4("model", model);
			
			renderer.Render(*allMeshRenderers[i]->GetMesh());
		}

		//float f = 0.5f;

		ImGui::ShowDemoWindow();

		//ImGui::Text("Testing ImGui");
		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

		//Render UI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Render other viewports
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	inputManager.Cleanup();
	shaderProgram.Delete();
	testModel.DeleteModel();
	entityRegistry.Cleanup();

	std::cout << "Exiting" << std::endl;

	//Clear ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastMouseX;
	float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

	lastMouseX = xpos;
	lastMouseY = ypos;

	//Only process the mouse moving if the mouse is locked to the viewport
	if (mouseLocked == true)
	{
		CameraComponent* camComp = camera->GetComponent<CameraComponent>();
		if (camComp == nullptr) { return; }
		camComp->ProcessMouseMovement(xoffset, yoffset);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	inputManager.KeyCallback(window, key, scancode, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	CameraComponent* camComp = camera->GetComponent<CameraComponent>();
	if (camComp == nullptr) { return; }
	camComp->ProcessMouseScroll(static_cast<float>(yoffset));
}

void ToggleMouseCursor()
{
	if (mouseLocked)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		mouseLocked = false;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseLocked = true;
	}
}

void ToggleWireframeMode()
{
	if (wireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframeMode = false;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframeMode = true;
	}
}

void CloseWindow()
{
	glfwSetWindowShouldClose(window, true);
}