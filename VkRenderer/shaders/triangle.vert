#version 460
#extension GL_KHR_vulkan_glsl : enable


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(push_constant) uniform Push {
    mat4 transform; // p * v * m
    mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(5.0, 0.0, 0.0)); // in world space
const float AMBINET = 0.02f;
void main() {
//    gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
    gl_Position = push.transform * vec4(position, 1.0);

    vec3 normalWorldMatrix = normalize(mat3(push.normalMatrix) * normal);

    float lightIntensity = AMBINET + max(dot(normalWorldMatrix, DIRECTION_TO_LIGHT), 0);

    fragColor = lightIntensity * color;
}