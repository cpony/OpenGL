#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "Material.h"
#include "LightDirectional.h"
#include "LightPoint.h"
#include "LightSpot.h"

#include "Mesh.h";
#include "Model.h";

//最开始的盒子贴图小人

#pragma region Model Data

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};
glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

#pragma endregion

#pragma region Camera Declare
//Camera camera(glm::vec3(0, 0, 3.0f), glm::vec3(0, -1.0f, 0), glm::vec3(0, 1.0f, 0));//原来的方法
Camera camera(glm::vec3(0, 0, 3.0f), glm::radians(-15.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

#pragma endregion

#pragma region Light Declare
//LightSpot light = LightSpot(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(glm::radians(90.0f), 0, 0),
//    glm::vec3(1.0f,1.0f,1.0f));

LightDirectional lightD = LightDirectional(glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightP0 = LightPoint(glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightP1 = LightPoint(glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
    glm::vec3(1.0f, 0, 0));

LightPoint lightP2 = LightPoint(glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
    glm::vec3(1.0f, 0, 1.0f));

LightSpot lightS = LightSpot(glm::vec3(0, 19.0f, 0),
    glm::vec3(glm::radians(90.0f), glm::radians(0.0f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

#pragma endregion


#pragma region Input Declare



float lastX;
float lastY;
bool firstMouse = true;


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//按esc键退出窗口
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.speedZ=1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.speedZ=-1.0f;
    }
    else {
        camera.speedZ = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.speedX = 1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.speedX = -1.0f;
    }
    else {
        camera.speedX = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.speedY = -1.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.speedY = 1.0f;
    }
    else {
        camera.speedY = 0;
    }

    camera.UpdateCameraPos();//相机按键控制
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse == true)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float deltaX, deltaY;
    deltaX = xPos - lastX;
    deltaY = yPos - lastY;

    lastX = xPos;
    lastY = yPos;
    
    //printf("%f \n", deltaX);
    camera.ProcessMouseMovement(deltaX, deltaY);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)//视口改变显示也改变
{
    glViewport(0, 0, width, height);
}

#pragma endregion

unsigned int LoadImageToGPU(const char* filename,GLint internalFormat,GLenum format,int textureSlot) {
    unsigned int TexBuffer;
    glGenTextures(1, &TexBuffer);
    glActiveTexture(GL_TEXTURE0+ textureSlot);
    glBindTexture(GL_TEXTURE_2D, TexBuffer);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannel;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannel, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("load image failed.");
    }
    stbi_image_free(data);
    return TexBuffer;
}

int main(int argc, char* argv[]) {
    std::string exePath = argv[0];
    //std::cout<<exePath.substr(0,exePath.find_last_of('\\'))+"\\model\\****.obj"<<std::endl;

#pragma region Open a Window


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用可编程化流水线
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);apple系统需要此步骤

    //open
    GLFWwindow* window = glfwCreateWindow(800,600, "MyOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Open window failed");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //init
    glewExperimental = true; //固定要写
    if (glewInit() != GLEW_OK)
    {
        printf("Init GLEW failed");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600);//渲染框
    //glEnable(GL_CULL_FACE);背面剔除
    //glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线框化

    glEnable(GL_DEPTH_TEST);

#pragma endregion

#pragma region Init Shader Program

    Shader* testShader = new Shader("vertexSource.vert", "fragmentSource.frag");
#pragma endregion

#pragma region Init Material
    Material* myMaterial = new Material(testShader,
        LoadImageToGPU("container2.png", GL_RGBA, GL_RGBA, Shader::DIFFUSE),
        LoadImageToGPU("container2_specular.png", GL_RGBA, GL_RGBA, Shader::SPECULAR),
        //1.0f,
        //1.0f,

        glm::vec3(1.0f,1.0f,1.0f),
        32.0f
    );
#pragma endregion


#pragma region trash OLD Init and Load Models to VAO, VBO



    //unsigned int VAO;
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    //unsigned int VBO;
    //glGenBuffers(1, &VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ////unsigned int EBO;
    ////glGenBuffers(1, &EBO);
    ////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    ////glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(6);

    //glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(7);
    //glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(8);
#pragma endregion

#pragma region Init and Load Models to VAO, VBO
    //Mesh cube(vertices);  //实例化一个叫cube的材质
    Model model(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\nanosuit.obj");
#pragma endregion

 #pragma region Init and Load Textures
   /* unsigned int TexBufferA;
    TexBufferA = LoadImageToGPU("container.jpg", GL_RGB, GL_RGB, 0);
    unsigned int TexBufferB;
    TexBufferB = LoadImageToGPU("awesomeface.png", GL_RGBA, GL_RGBA, 3);*/
#pragma endregion

#pragma region Prepare MVP matrices

    //glm::mat4 trans;
    //trans = glm::mat4(1.0f);
    ////trans = glm::translate(trans, glm::vec3(1.0f, 0, 0));
    ////trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0, 0, 1.0f));
    ////trans = glm::scale(trans, glm::vec3(2.0f,2.0f,2.0f));练习用

    glm::mat4 modelMat;
    modelMat = glm::mat4(1.0f);
    //modelMat = glm::rotate(modelMat, glm::radians(-55.0f), glm::vec3(1.0f, 1.0f, 0));
    glm::mat4 viewMat;
    viewMat = glm::mat4(1.0f);
    //viewMat = glm::translate(viewMat, glm::vec3(0, 0, -3.0f));
    //viewMat = camera.GetViewMatrix();
    glm::mat4 projMat;
    projMat = glm::mat4(1.0f);
    projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

#pragma endregion

    while (!glfwWindowShouldClose(window))// rendering loop
    {

        // Processing Input
        processInput(window);//自定义操作事件

        // Clear Screen
        glClearColor(0.0f, 0.0f, 0.0, 1.0f);//绘制
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        //float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //glUseProgram(shaderProgram);
        //glUniform4f(vertexColorLocation, 0, greenValue, 0, 1.0f);之前练习用

        viewMat = camera.GetViewMatrix();

        for (int i = 0; i < 1; i++) {
        //Set Model Matrix
        modelMat = glm::translate(glm::mat4(1.0f), cubePositions[i]);

        //Set View and Projection Matrices here if you want.
        //
        
        //Set Material -> Shader Program
        testShader->use();

        //Set Material->Textures 第二章结束后需要这块
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, myMaterial->diffuse);
        //glActiveTexture(GL_TEXTURE0+1);
        //glBindTexture(GL_TEXTURE_2D, myMaterial->specular);

        //Set Material ->Uniforms
        //glUniform1i(glGetUniformLocation(testShader->ID, "ourTexture"), 0);
        //glUniform1i(glGetUniformLocation(testShader->ID, "ourFace"), 3);
        
        //glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
        glUniformMatrix4fv(glGetUniformLocation(testShader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
        glUniform3f(glGetUniformLocation(testShader->ID, "objColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(testShader->ID, "ambientColor"), 0.04f, 0.04f, 0.04f);
        
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.pos"), lightD.position.x, lightD.position.y, lightD.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.dirToLight"), lightD.direction.x, lightD.direction.y, lightD.direction.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightD.color"), lightD.color.x, lightD.color.y, lightD.color.z);
        
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.pos"), lightP0.position.x, lightP0.position.y, lightP0.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.dirToLight"), lightP0.direction.x, lightP0.direction.y, lightP0.direction.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP0.color"), lightP0.color.x, lightP0.color.y, lightP0.color.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.constant"), lightP0.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.linear"), lightP0.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP0.quadratic"), lightP0.quadratic);

        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.pos"), lightP1.position.x, lightP1.position.y, lightP1.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.dirToLight"), lightP1.direction.x, lightP1.direction.y, lightP1.direction.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP1.color"), lightP1.color.x, lightP1.color.y, lightP1.color.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.constant"), lightP1.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.linear"), lightP1.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP1.quadratic"), lightP1.quadratic);

        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.pos"), lightP2.position.x, lightP2.position.y, lightP2.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.dirToLight"), lightP2.direction.x, lightP2.direction.y, lightP2.direction.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightP2.color"), lightP2.color.x, lightP2.color.y, lightP2.color.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.constant"), lightP2.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.linear"), lightP2.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightP2.quadratic"), lightP2.quadratic);


        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.pos"), lightS.position.x, lightS.position.y, lightS.position.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.dirToLight"), lightS.direction.x, lightS.direction.y, lightS.direction.z);
        glUniform3f(glGetUniformLocation(testShader->ID, "lightS.color"), lightS.color.x, lightS.color.y, lightS.color.z);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.constant"), lightS.constant);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.linear"), lightS.linear);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.quadratic"), lightS.quadratic);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.cosPhyInner"), lightS.cosPhyInner);
        glUniform1f(glGetUniformLocation(testShader->ID, "lightS.cosPhyOuter"), lightS.cosPhyOuter);

        glUniform3f(glGetUniformLocation(testShader->ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        //glUniform3f(glGetUniformLocation(testShader->ID, "material.ambient"), 1.0f,1.0f,1.0f);
        myMaterial->shader->SetUniform3f("material.ambient", myMaterial->ambient);
        //myMaterial->shader->SetUniform3f("material.diffuse", myMaterial->diffuse);
        myMaterial->shader->SetUniform1i("material.diffuse", Shader::DIFFUSE);
        //myMaterial->shader->SetUniform3f("material.specular", myMaterial->specular);
        myMaterial->shader->SetUniform1i("material.specular", Shader::SPECULAR);

        myMaterial->shader->SetUniform1f("material.shininess", myMaterial->shininess);
        //之前只是给material赋值，material类里面的值还要传到uniform去

        //glUniform1f(glGetUniformLocation(testShader->ID, "material,shininess"), 32.0f);
        



        //Set Model 第二章结束后需要这块
        //glBindVertexArray(VAO);

        //Drawcall 第二章结束后需要这块
        //glDrawArrays(GL_TRIANGLES, 0, 36);//画VAO用

        //cube.Draw(myMaterial->shader);
        model.Draw(myMaterial->shader);
        }

        // check and call events and swap the buffers
        //Clean up, prepare for next render loop
        glfwSwapBuffers(window);//双缓冲
        glfwPollEvents();//操作事件

    }

    //Exit
    glfwTerminate();
    return 0;
}