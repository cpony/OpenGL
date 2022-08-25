#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out float Height;
out vec3 Position;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform sampler2D heightMap;
//vec2 dirToMap = aPos.xz*10;

layout(binding = 6) uniform sampler2D heightMap;
//uniform sampler2D heightMap;

void main()
{
    //aPos.y = texture(heightMap, aTexCoords).r;²»ÄÜÐÞ¸ÄaPos
    //Height = aPos.y;
    //Height +=texture(heightMap, aTexCoords).r * yScale - yShift;
    //Height =texture(heightMap, aTexCoords).r * (64.0f / 256.0f )- 16.0f;
    //Height =texture(heightMap, dirToMap).r * (64.0f / 256.0f )- 16.0f;
    //Height =texture(heightMap, aTexCoords).y * (64.0f /256.0f )- 16.0f;

    //Position = (view * model * vec4(aPos, 1.0)).xyz;
    //gl_Position = projection * view * model * vec4(aPos.x,Height,aPos.z, 1.0);
    //gl_Position = projection * view * model * vec4(aPos.x,aPos.y,aPos.z, 1.0);

    //___________________
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoords;

}