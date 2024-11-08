#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform DynamicUniformBufferObject
{
    mat4 transform;
} object;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;



void main()
{
    gl_Position = ubo.proj * ubo.view * object.transform * ubo.model * vec4(inPosition, 1.0);
    outColor = inColor;
}