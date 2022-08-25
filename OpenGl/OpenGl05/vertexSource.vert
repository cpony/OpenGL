#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
//��д��mesh����

//out vec3 FragPos;
//out vec3 Normal;
//out vec2 TexCoords;



out vec3 FragPos;
out vec2 TexCoords;
//out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out mat3 TBN;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
//uniform vec3 dirLight;


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
   // Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);//ʩ��������
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N)); //������󣬶������߿ռ����
    
    //TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}