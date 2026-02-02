#include "InputManager.h"
#include <GLFW\glfw3.h>

void InputManager::BindNewKey(int key, int action, int mods, std::function<void()> actionToExecute)
{
	//Search actionkeys to see if keyData already exists
	auto keyData = actionKeys.find(key);

	//If we found functions bound to this action
	if (keyData != actionKeys.end())
	{
		//For every action bound to this key
		for (int i = 0; i < keyData->second.size(); i++)
		{
			//If this is the key, action and modifier we are after
			if (keyData->second[i].first[0] == action && keyData->second[i].first[1] == mods)
			{
				//Add new function to this key action/modifier
				keyData->second[i].second.push_back(actionToExecute);
				return;
			}
		}
		//If we have not found the action or mod on this key (key found, but not with action/mod), add it
		std::vector<int> actionMod = {action, mods};
		std::vector<std::function<void()>> functionVector = std::vector<std::function<void()>>();
		functionVector.push_back(actionToExecute);
		keyData->second.push_back(std::make_pair(actionMod, functionVector));
	}
	else //No functions found (key not found)
	{
		//Create data to populate new key data
		std::vector<std::pair<std::vector<int>, std::vector<std::function<void()>>>> newKeyData = std::vector < std::pair<std::vector<int>, std::vector<std::function<void()>>>>();
		std::vector<int> actionMod = { action, mods };
		std::vector<std::function<void()>> functionVector = std::vector<std::function<void()>>();
		functionVector.push_back(actionToExecute);
		newKeyData.push_back(std::make_pair(actionMod, functionVector));

		//Emplace new keydata to this key
		actionKeys.emplace(key, newKeyData);
	}
}

//When a key is pressed
void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto keyData = actionKeys.find(key);
	//If we found keyData bound to this key
	if (keyData != actionKeys.end())
	{
		//for every key action on this key
		for (int i = 0; i < keyData->second.size(); i++)
		{
			//If this is the correct action/modifiers
			if (keyData->second[i].first[0] == action && keyData->second[i].first[1] == mods)
			{
				//Execute all functions attached to it
				for (int j = 0; j < keyData->second[i].second.size(); j++)
				{
					keyData->second[i].second[j]();
				}
			}
		}
	}
}

//Function to check if a key is down in this frame, typically will be used in OnUpdate of components
bool InputManager::IsKeyDown(struct GLFWwindow* window, int key)
{
	if (window == nullptr)
	{ 
		std::cout << "ERROR: Window not found in IsKeyDown() event." << std::endl;
		return false; 
	}
	return glfwGetKey(window, key) == GLFW_PRESS;
}

void InputManager::Cleanup()
{
	actionKeys.clear();
}
