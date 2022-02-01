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

	//Random cube position
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	std::vector<Vertex> verticesVector(std::begin(vertexArray), std::end(vertexArray));
	std::vector<unsigned int> indicesVector(std::begin(indices), std::end(indices));

	RawModel testModel = RawModel(verticesVector, indicesVector);
	EntityRenderer renderer = EntityRenderer();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
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
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * (i + 1);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.5f, 0.3f, 0.5f));
			shaderProgram.SetMat4("model", model);

			renderer.Render(testModel);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	testModel.DeleteModel();

	std::cout << "Exiting" << std::endl;

	glfwTerminate();
	return 0;
}