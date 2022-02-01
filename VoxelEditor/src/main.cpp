#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Rendering/ShaderLoader.h"
#include "Rendering/RawModel.h"
#include "Rendering/EntityRenderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
GLFWwindow* InitialiseOpenGL();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLFWwindow* InitialiseOpenGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Editor", NULL, NULL);
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

	glViewport(0, 0, 800, 600);
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
		{glm::vec<3, double>(0.5,  0.5, 0.0),		glm::vec3(1.0f, 0.0f, 0.0f)},
		{glm::vec<3, double>(0.5, -0.5, 0.0),		glm::vec3(0.0f, 1.0f, 0.0f)},
		{glm::vec<3, double>(-0.5, -0.5, 0.0),		glm::vec3(0.0f, 0.0f, 1.0f)},
		{glm::vec<3, double>(-0.5,  0.5, 0.0),		glm::vec3(1.0f, 1.0f, 1.0f)},
	};

	unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
	};

	std::vector<Vertex> verticesVector(std::begin(vertexArray), std::end(vertexArray));
	std::vector<unsigned int> indicesVector(std::begin(indices), std::end(indices));

	RawModel testModel = RawModel(verticesVector, indicesVector);
	EntityRenderer renderer = EntityRenderer();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


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
		glClear(GL_COLOR_BUFFER_BIT);

		//Draw triangle
		shaderProgram.Use();
		renderer.Render(testModel);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	testModel.DeleteModel();

	std::cout << "Exiting" << std::endl;

	glfwTerminate();
	return 0;
}