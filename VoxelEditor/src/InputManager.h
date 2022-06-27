#pragma once
#include <map>
#include <vector>
#include <iostream>
#include <utility>
#include <functional>
//#include <GLFW\glfw3.h>

class InputManager
{
public:
	//For action events (1 time key presses or releases)
	void BindNewKey(int key, int action, int mods, std::function<void()> actionToExecute);

	//Callback to link to GLFW key callback function
	void KeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);

	static bool IsKeyDown(struct GLFWwindow* window, int key);

	void Cleanup();

private:
	//key id pointing to vector of pairs of int vector (action, mods) and vector of functions
	std::map<int, std::vector<std::pair<std::vector<int>, std::vector<std::function<void()>>>>> actionKeys;
};