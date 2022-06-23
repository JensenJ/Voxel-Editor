#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Rendering/ShaderLoader.h"
#include "Rendering/RawModel.h"
#include "Rendering/EntityRenderer.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/MeshRendererComponent.h"
#include "Entity/EntityRegistry.h"

int screenWidth = 1920;
int screenHeight = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* InitialiseOpenGL();

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

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Editor", NULL, NULL);
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
	return window;
}

int main()
{
	//Create OpenGL window
	GLFWwindow* window = InitialiseOpenGL();
	if (window == nullptr) { return -1; }
	std::cout << "Created Window" << std::endl;
	
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
	0, 1, 3,   // first triangle - front top right, front bottom right, front top left
	1, 2, 3,   // second triangle - front bottom right, front bottom left, front top left
	4, 5, 7,   // first triangle - back top right, back bottom right, back top left
	5, 6, 7,   // second triangle - back bottom right, back bottom left, back top left
	0, 5, 4,   // first triangle - front top right, back bottom right, back top right
	1, 5, 0,   // second triangle - front bottom right, back bottom right, front top right
	3, 6, 7,   // first triangle - front top left, back bottom left, back top left
	2, 6, 3,   // second triangle - front bottom left, back bottom left, front top left
	7, 3, 4,   // first triangle - back top left, front top left, back top right
	4, 0, 3,   // second triangle - back top right, front top right, front top left
	6, 2, 5,   // first triangle - back bottom left, front bottom left, back bottom right
	5, 1, 2,   // second triangle - back bottom right, front bottom right, front bottom left
	};

	std::vector<Vertex> verticesVector(std::begin(vertexArray), std::end(vertexArray));
	std::vector<unsigned int> indicesVector(std::begin(indices), std::end(indices));

	RawModel testModel = RawModel(verticesVector, indicesVector);
	EntityRenderer renderer = EntityRenderer();
	EntityRegistry entityRegistry = EntityRegistry();

	//Create entities
	for (int i = 0; i < 300; i++)
	{
		Entity* entity = entityRegistry.CreateEntity();
		entity->AddComponent<TransformComponent>(glm::vec3((rand() % 20) - 10, (rand() % 20) - 10, -rand() % 50));
		entity->AddComponent<MeshRendererComponent>(&testModel);
	}
	
	std::vector<TransformComponent*> allTransforms = entityRegistry.GetAllComponentsOfType<TransformComponent>();
	std::vector<MeshRendererComponent*> allMeshRenderers = entityRegistry.GetAllComponentsOfType<MeshRendererComponent>();

	glEnable(GL_DEPTH_TEST);

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	while (!glfwWindowShouldClose(window))
	{
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

		//Input
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		//Wireframe toggle
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.05f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		shaderProgram.SetMat4("view", view);
		shaderProgram.SetMat4("projection", projection);

		shaderProgram.Use();

		for (unsigned int i = 0; i < allMeshRenderers.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
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

			//transform->AddPosition(glm::vec3(0.25f,0,0));
			transform->SetRotation(glm::vec3((float)glfwGetTime() * glm::radians((float)20 * (i + 1)) * glm::vec3(0.5f, 0.3f, 0.7f)));

			model = transform->GetTransform();
			shaderProgram.SetMat4("model", model);
			
			renderer.Render(*allMeshRenderers[i]->GetMesh());
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	testModel.DeleteModel();
	entityRegistry.Cleanup();

	std::cout << "Exiting" << std::endl;

	glfwTerminate();
	return 0;
}