#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
// Instanced model matrix (occupies locations 2,3,4,5)
layout (location = 2) in mat4 instanceModel;

layout (location = 0) out vec3 vertexColor;

layout (location = 0) uniform mat4 view;
layout (location = 1) uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = aColor;
}