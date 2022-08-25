#version 430 core
layout (location = 0) in vec3 aPos;
in layout(location = 1) vec2 aTex;

out float Height;
out vec3 Position;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightMap;

void main()
{
    TexCoord = aTex;
    Height = aPos.y;
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //Height= texture(heightMap,in_texcoord).r;
    //Height= Height* 64.0f/256.0f*1000f - 16.0f;
    gl_Position = projection * view * model * vec4(aPos.x,Height*10,aPos.z, 1.0);
}
