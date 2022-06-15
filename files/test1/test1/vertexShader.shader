#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColour;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vNormal;

uniform mat4 world, view, projection;
uniform sampler2D heightmap;

out vec3 colour;
out vec2 uv;
out vec3 normal;
out vec4 worldPixel;
                            
void main() 
{
    worldPixel =  world * vec4(vPos, 1.0f);

    vec4 diffuseColor = texture(heightmap, vUV);
    //vec4 diffuseColor = vec4(vUV, 1.0, 1.0);
    worldPixel.y += diffuseColor.r * 50;

    gl_Position = projection * view * worldPixel;
    colour = vColour;
    uv = vUV;
    normal = mat3(world)*vNormal;
}