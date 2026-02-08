#pragma once

// STL
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// OpenGL
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// ImGui
#include <imgui.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Voxel Editor
#include <Voxel/Application.h>
#include <Voxel/ECS/Entity.h>
#include <Voxel/ECS/EntityRegistry.h>
#include <Voxel/InputManager.h>
#include <Voxel/Log/Log.h>
