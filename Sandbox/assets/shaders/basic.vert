#version 450
#extension GL_KHR_vulkan_glsl: enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    mat4 projectionView;
} camera;

struct ModelMatrix
{
    mat4 transform;
    mat4 rotation;
};

layout(std140, binding = 1) readonly buffer StorageBuffer
{
    ModelMatrix model[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;



void main()
{
    gl_Position = camera.projectionView * model[gl_InstanceIndex].transform * model[gl_InstanceIndex].rotation * vec4(inPosition, 1.0);
    outColor = inColor;
}