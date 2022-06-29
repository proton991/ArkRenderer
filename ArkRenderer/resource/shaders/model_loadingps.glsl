#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
layout(binding=0) uniform sampler2D diffuseMap;
void main()
{    
    FragColor = texture(diffuseMap, TexCoords);
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}