#pragma once
#include <map>
#include <vector>
#include <iostream>
#include <utility>

class InputManager
{
public:
	void BindNewKey(int key, int action, int mods, void (*actionToExecute)());

	void KeyCallback(struct GLFWwindow* window, int key, int scancode, int action, int mods);

	void Cleanup();

private:
	//key id pointing to vector of pairs of int vector (action, mods) and vector of functions
	std::map<int, std::vector<std::pair<std::vector<int>, std::vector<void(*)()>>>> actionKeys;
};