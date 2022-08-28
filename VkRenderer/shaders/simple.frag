#version 460
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
}ubo;

layout (set = 1, binding = 1) uniform sampler2D diffuseMap;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 normalMatrix;
} push;


void main() {
    vec3 directionToLight = ubo.lightPosition - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);
    vec3 color = texture(diffuseMap, fragUv).xyz;
    outColor = vec4((diffuseLight + ambientLight) * color, 1.0);
}