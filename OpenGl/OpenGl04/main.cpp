#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"

// GLM Mathemtics
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>

//光线追踪小球

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

using namespace std;
using namespace glm;

const int Width = 1600;
const int Height = 800;

const int TexWidth = 1024 * 2;
const int TexHeight = 1024 * 2;

float deltaTime = 0.0f;//上一帧与当前帧的时间间隔
float lastFrame = 0.0f;//上一帧的时间
double lastX = 800.0;//上一帧鼠标的x值
double lastY = 400.0;//下一帧鼠标的y值
bool firstMouse = true;


#pragma region Camera Declare
Camera camera(glm::vec3(13.0f, 10.0f, 10.0f), glm::vec3(0, -1.0f, 0), glm::vec3(0, 1.0f, 0));//原来的方法
//Camera camera(glm::vec3(0, 0, 3.0f), glm::radians(-15.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));

#pragma endregion

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//按esc键退出窗口
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.speedZ = 5.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.speedZ = -5.0f;
    }
    else {
        camera.speedZ = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.speedX = 5.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.speedX = -5.0f;
    }
    else {
        camera.speedX = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.speedY = -5.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.speedY = 5.0f;
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


//绘制图片
unsigned int imageVAO = 0;
unsigned int imageVBO = 0;
unsigned int imageEBO = 0;
void RenderImage() {
    float vertices[] = {
             1.0f,  1.0f, 0.0f,  // top right
             1.0f, -1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 0.0f,  // bottom left
            -1.0f,  1.0f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    glGenVertexArrays(1, &imageVAO);
    glBindVertexArray(imageVAO);

    glGenBuffers(1, &imageVBO);
    glBindBuffer(GL_ARRAY_BUFFER, imageVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &imageEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imageEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(Width, Height, "PathTracing", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    // build and compile our shader program
    // ------------------------------------
    Shader* pathtracingShader = new Shader("vertexSource.vert", "fragmentSource.frag"); // you can name your shader files however you like



    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        //ourShader.use();
        //glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        pathtracingShader->use();

        glUniform2f(glGetUniformLocation(pathtracingShader->ID, "screenSize"), Width, Height);

        glUniform3f(glGetUniformLocation(pathtracingShader->ID, "camera.lower_left_corner"), -2.0, -1.0, -1.0);
        glUniform3f(glGetUniformLocation(pathtracingShader->ID, "camera.horizontal"), 4.0, 0.0, 0.0);
        glUniform3f(glGetUniformLocation(pathtracingShader->ID, "camera.vertical"), 0.0, 2.0, 0.0);
        glUniform3f(glGetUniformLocation(pathtracingShader->ID, "lookfrom"), camera.Position.x, camera.Position.y, camera.Position.z);
        //glUniform3f(glGetUniformLocation(pathtracingShader->ID, "lookat"), 0, -1.0f, 0);
        //glUniform3f(glGetUniformLocation(pathtracingShader->ID, "vup"), 0, 1.0f, 0);

        

        RenderImage();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}