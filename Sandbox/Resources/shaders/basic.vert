#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    mat4 projectionView;
} camera;

layout(binding = 1) uniform DynamicUniformBufferObject
{
    mat4 transform;
    mat4 rotation;
} object;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;



void main()
{
    gl_Position = camera.projectionView * object.transform * object.rotation * vec4(inPosition, 1.0);
    outColor = inColor;
}