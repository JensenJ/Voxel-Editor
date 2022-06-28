#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_internal.h"
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
#include "Rendering/FrameBuffer.h"

int screenWidth = 2560;
int screenHeight = 1440;

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

void RenderUI();

GLFWwindow* window;

void ToggleMouseCursor();
void ToggleWireframeMode();
void CloseWindow();

FrameBuffer* sceneBuffer;

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
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	//Center window
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowPos(window, (mode->width - screenWidth) / 2, (mode->height - screenHeight) / 2);
	glfwMaximizeWindow(window);

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
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

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

int main()
{
	//Initialise OpenGL and create window
	GLFWwindow* window = InitialiseOpenGL();
	if (window == nullptr) { return -1; }
	std::cout << "Created Window" << std::endl;

	//Initialise ImGui
	InitialiseImGui(window);

	//Load shaders
	bool shaderSuccess = false;
	std::string vertexPath = std::filesystem::current_path().string() + "\\src\\Shaders\\vertexShader.txt";
	std::string fragmentPath = std::filesystem::current_path().string() + "\\src\\Shaders\\fragmentShader.txt";
	Shader shaderProgram = ShaderLoader::CreateShaderProgram(vertexPath.c_str(), fragmentPath.c_str(), shaderSuccess);

	//If shader compilation/linking failed
	if (!shaderSuccess)
	{
		std::cout << "Failed to create shaders" << std::endl;
		glfwTerminate();
		return -2;
	}
	std::cout << "Created Shaders" << std::endl;

	Vertex vertexArray[] = {
		//Positions							//Colours
		{glm::vec3(0.5f,  0.5f, 0.5f),		glm::vec3(0.0f, 0.0f, 0.0f)}, //Front Face top right
		{glm::vec3(0.5f, -0.5f, 0.5f),		glm::vec3(1.0f, 0.0f, 0.0f)}, //Front face bottom right
		{glm::vec3(-0.5f, -0.5f, 0.5f),		glm::vec3(0.0f, 1.0f, 0.0f)}, //Front face bottom left
		{glm::vec3(-0.5f,  0.5f, 0.5f),		glm::vec3(0.0f, 0.0f, 1.0f)}, //Front face top left
				 
		{glm::vec3(0.5f,  0.5f, -0.5f),		glm::vec3(1.0f, 1.0f, 0.0f)}, //Back Face top right
		{glm::vec3(0.5f, -0.5f, -0.5f),		glm::vec3(0.0f, 1.0f, 1.0f)}, //Back face bottom right
		{glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(1.0f, 0.0f, 1.0f)}, //Back face bottom left
		{glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(1.0f, 1.0f, 1.0f)} //Back face top left
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
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 2; y++)
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

	sceneBuffer = new FrameBuffer(screenWidth, screenHeight);

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

		shaderProgram.Use();

		//Draw wireframe if enabled
		if (wireframeMode) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

		//Rendering
		sceneBuffer->Bind();
		glEnable(GL_DEPTH_TEST); //Enable depth testing for screen-space quad

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

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

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		sceneBuffer->Unbind();
		glDisable(GL_DEPTH_TEST); //Disable depth test so screen-space quad isnt discarded

		//Render over previous frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		//Render ImGui stuff
		RenderUI();

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

//Render ImGui UI
void RenderUI()
{
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dockspace", nullptr, windowFlags);
	ImGui::PopStyleVar(2);

	// Create dockspace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("Dockspace");
	//check if this layout exists (if defined in imgui.ini)
	static bool firstTimeLoading = ImGui::DockBuilderGetNode(dockspace_id) == nullptr;
	 //Create new dockspace with this id (loads layout if one exists)
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

	//Specify layout of viewports if this is the first time loading
	if (firstTimeLoading)
	{
		//Prevent layout being reapplied next frame
		firstTimeLoading = false;

		//Clear previous dockspace layout
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

		//Create new node on the right of this dockspace
		auto dockIdRight = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);
		auto dockIdRightTop = ImGui::DockBuilderSplitNode(dockIdRight, ImGuiDir_Up, 0.25f, nullptr, &dockIdRight);

		//Dock the windows in the correct place
		ImGui::DockBuilderDockWindow("Properties", dockIdRight);
		ImGui::DockBuilderDockWindow("Hierarchy", dockIdRightTop);
		ImGui::DockBuilderFinish(dockspace_id);
	}

	//Menu bars (menus at top)
	if (ImGui::BeginMenuBar())
	{
		//File menu
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New", "Ctrl+N");
			ImGui::MenuItem("Open", "Ctrl+N");
			ImGui::Separator();
			ImGui::MenuItem("Save", "Ctrl+S");
			ImGui::MenuItem("Save As", "Ctrl+Shift+S");
			ImGui::EndMenu();
		}

		//Edit menu
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Undo", "Ctrl+Z");
			ImGui::MenuItem("Redo", "Ctrl+Shift+Z");
			ImGui::EndMenu();
		}
		
		ImGui::EndMenuBar();

	}
	ImGui::End();

	//Dock main viewport to dockspace
	ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);

	//Render viewport with openGL frame buffer
	ImGui::Begin("Viewport");
	{
		ImGui::BeginChild("View");
		screenWidth = (int)ImGui::GetContentRegionAvail().x;
		screenHeight = (int)ImGui::GetContentRegionAvail().y;

		//Disable a warning which cant be resolved
		#pragma warning( push )
		#pragma warning (disable : 4312)
		ImGui::Image(ImTextureID(sceneBuffer->GetFrameTexture()), ImGui::GetContentRegionMax(), ImVec2(0, 1), ImVec2(1, 0));
		#pragma warning( pop )
		ImGui::EndChild();
	}

	ImGui::End();

	ImGui::Begin("Hierarchy");
	{
		ImGui::Text("TestLabel");
	}
	ImGui::End();

	ImGui::Begin("Properties");
	{
		ImGui::Text("TestLabel");
	}
	ImGui::End();

	//Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//Render other viewports
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	sceneBuffer->RescaleFrameBuffer(width, height);
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
	wireframeMode = !wireframeMode;
}

void CloseWindow()
{
	glfwSetWindowShouldClose(window, true);
}