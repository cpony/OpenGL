#define WIN32_LEAN_AND_MEAN
#define STB_IMAGE_IMPLEMENTATION
#define INT_SIZE 32

#include <iostream>

#include "OpenGLImports.h"

//#include "ShaderProgram.h"
//#include "shader.h"
#include "shader_t.h"
#include "shader_c.h"
#include "Texture.h"
#include "camera.h"

#include "Debug.h"
// --------------------------------------------------------
// FUNCTION DECLARATIONS

int main(void);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


// setup functions
void setUpLibraries();
void setCallbackFunctions();
void init_VAO_and_shaders();
void init_textures();

// fft functions
void create_h0k_h0minusk_textures();
void create_butterfly_texture();
void create_fourier_components();
void fft();
void inversion();

// loop functions
void render();

// help functions 
unsigned int reverseBits(unsigned int n);

// cleanup functions
void cleanUp();


// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
// camera - give pretty starting point
//Camera camera(glm::vec3(67.0f, 627.5f, 169.9f),
//	glm::vec3(0.0f, 1.0f, 0.0f),
//	-128.1f, -42.4f);
//Camera camera(glm::vec3(0.0f, 0.0f, 0.0f),
//	glm::vec3(0.0f, 1.0f, 0.0f),
//	-128.1f, -42.4f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
// texture size

int useWireframe = 0;
int displayGrayscale = 0;
const unsigned int NUM_PATCH_PTS = 4;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --------------------------------------------------------
// --------------------------------------------------------
// USER DEFINED VARIABLES

// window settings
GLFWwindow* window = nullptr;
const int window_width = 800;
const int window_height = 800;
const char* window_title = "Poseidon";

// fft ocean parameters
float A = 10;
int L = 1000;//波长
glm::vec2 windDirection = glm::vec2(1.0f, 1.0f);
float windSpeed = 40;
bool choppy = true;

// width and height of grid
//const int N = 256;
const int N = 1024;//图片长度
const int M = 256;

// --------------------------------------------------------
// GLOBAL VARIABLES

// shaders

//Shader programRender;
 Shader programRender;
 ComputeShader programTildeHCompute;
 ComputeShader programButterflyTextureCompute;
 ComputeShader programFourierComponentCompute;
 ComputeShader programButterflyCompute;
 ComputeShader programInversionCompute;
unsigned int VAO, VBO;


//textures
Texture texture_random_noise_1;
Texture texture_random_noise_2;
Texture texture_random_noise_3;
Texture texture_random_noise_4;

Texture texture_tilde_h0k;
Texture texture_tilde_h0minusk;

Texture texture_butterfly;

Texture texture_fourier_component_dx;
Texture texture_fourier_component_dy;
Texture texture_fourier_component_dz;

Texture texture_pingpong_0;
Texture texture_pingpong_1;

Texture texture_displacement_of_points_on_grid;

// time
float lastRefreshTime = 0.0f;
float currTime = glfwGetTime();
float t = 0.0f;

// misc
int* bitReversedIndices;
int pingpong_index = 0;
const int log_2_N = (int)(log(N) / log(2));
char pressed = ' ';

// --------------------------------------------------------
// STRUCTS
struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoord;
};

// --------------------------------------------------------
// MAIN
int main(void)
{
    setUpLibraries();
    setCallbackFunctions();

    init_VAO_and_shaders();
    init_textures();

    create_h0k_h0minusk_textures();
    create_butterfly_texture();
    create_fourier_components();
    fft();
    inversion();
    

    // CONNECT TO FRAGMENT SHADER
    // bind the resulting textures to fragment shader
    glBindTextureUnit(0, texture_tilde_h0k.getID());
    glBindTextureUnit(1, texture_tilde_h0minusk.getID());

    // bind resulting butterfly texture to fragment shader
    glBindTextureUnit(2, texture_butterfly.getID());

    // bind resulting dx, dy, dz fourier component textures  to fragment shader
    glBindTextureUnit(3, texture_fourier_component_dx.getID());
    glBindTextureUnit(4, texture_fourier_component_dy.getID());
    glBindTextureUnit(5, texture_fourier_component_dz.getID());

    // bind ping pong textures to fragment shader
    glBindTextureUnit(6, texture_displacement_of_points_on_grid.getID());
    glBindTextureUnit(7, texture_pingpong_1.getID());


    //______________________
	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	std::cout << "Max available tess level: " << maxTessLevel << std::endl;


	glEnable(GL_DEPTH_TEST);//不开
    
	//Shader heightMapShader("vertexSource.vert", "fragmentSource.frag");
	Shader heightMapShader("vertexSource.vert", "fragmentSource.frag", nullptr,
        "gpuheight.tcs", "gpuheight.tes");
	//glActiveTexture(GL_TEXTURE6);
    //glBindTexture(GL_TEXTURE_2D, texture_displacement_of_points_on_grid.getID());
	heightMapShader.SetUniform1i("heightMap", 6);
	//// load and create a texture
	//// -------------------------
	//// load image, create texture and generate mipmaps
	//// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	////stbi_set_flip_vertically_on_load(true);
	////int width, height, nrChannels;
	////unsigned char* data = stbi_load("E:/vs2019/OpenGl/Debug/textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);

	////if (data)
	////{
	////	std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
	////}
	////else
	////{
	////	std::cout << "Failed to load texture" << std::endl;
	////}

    int width, height, nrChannels;
    width = N;
    height = N;
    nrChannels = 4;
    //// set up vertex data (and buffer(s)) and configure vertex attributes
    //// ------------------------------------------------------------------   
    //std::vector<float> terrainvertices;
    //float yScale = 64.0f / 256.0f, yShift = 16.0f;
    //int rez = 1;
    //unsigned bytePerPixel = nrChannels;
    //for (int i = 0; i < height; i++)
    //{
    //	for (int j = 0; j < width; j++)
    //	{
    //		//unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
    //		//unsigned char y = pixelOffset[0];
    //
    //		// vertex
    //        terrainvertices.push_back(-height / 2.0f + height * i / (float)height);   // vx
    //		//vertices.push_back((int)y * yScale - yShift);   // vys
    //        //terrainvertices.push_back(128 * yScale - yShift);   // vy
    //        terrainvertices.push_back(0);// vy
    //        //vertices.push_back( 1 * yScale - yShift);   // vy
    //        terrainvertices.push_back(-width / 2.0f + width * j / (float)width);   // vz
    //
    //        terrainvertices.push_back(i / (float)height);   // u
    //        terrainvertices.push_back(j / (float)width);   // v
    //	}
    //}
    //std::cout << "Loaded " << terrainvertices.size() / 3 << " vertices" << std::endl;
    ////stbi_image_free(data);
    //
    //std::vector<unsigned> indices;
    //for (unsigned i = 0; i < height - 1; i += rez)
    //{
    //	for (unsigned j = 0; j < width; j += rez)
    //	{
    //		for (unsigned k = 0; k < 2; k++)
    //		{
    //			indices.push_back(j + width * (i + k * rez));
    //		}
    //	}
    //}
    //std::cout << "Loaded " << indices.size() << " indices" << std::endl;
    //
    //const int numStrips = (height - 1) / rez;
    //const int numTrisPerStrip = (width / rez) * 2 - 2;
    //std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    //std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;
    //
    //// first, configure the cube's VAO (and terrainVBO + terrainIBO)
    //unsigned int terrainVAO, terrainVBO, terrainIBO;
    //glGenVertexArrays(1, &terrainVAO);
    //glBindVertexArray(terrainVAO);
    //
    //glGenBuffers(1, &terrainVBO);
    //glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    //glBufferData(GL_ARRAY_BUFFER, terrainvertices.size() * sizeof(float), &terrainvertices[0], GL_STATIC_DRAW);
    //
    //// position attribute
    ////glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//只有pos，没有normal和uv
    ////glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//只有pos，没有normal和uv
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)3);//只有pos，没有normal和uv
    //glEnableVertexAttribArray(1);
    //
    //glGenBuffers(1, &terrainIBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
    //*/

		// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	std::vector<float> vertices;

	unsigned rez = 20;
	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
		}
	}
	std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

	// first, configure the cube's VAO (and terrainVBO)
	unsigned int terrainVAO, terrainVBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    //render loop
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        processInput(window);
        glEnable(GL_BLEND);

        currTime = glfwGetTime();
        if (currTime - lastRefreshTime > 1 / 30)
        {
            t += 0.25;//change
            create_fourier_components();
            fft();
            inversion();
            lastRefreshTime = currTime;
        }

		render();
		// view/projection transformations
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		view = camera.GetViewMatrix();
		programRender.SetUniform4m("projection", projection);//要用glProgramUniform不能用glUniform（估计是版本问题，不清楚）
		programRender.SetUniform4m("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(1.0f,1.0f,1.0f));
		programRender.SetUniform4m("model", model);
        //____________________________________________________________________________

		//glActiveTexture(GL_TEXTURE6);
		//glBindTexture(GL_TEXTURE_2D, texture_displacement_of_points_on_grid.getID());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glBindImageTexture(6, texture_displacement_of_points_on_grid.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);
		heightMapShader.use();

		// view/projection transformations
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
		view = camera.GetViewMatrix();
		heightMapShader.SetUniform4m("projection", projection);
		heightMapShader.SetUniform4m("view", view);

		// world transformation
		model = glm::mat4(1.0f);
		heightMapShader.SetUniform4m("model", model);

		// render the cube
		//glBindVertexArray(terrainVAO);
		////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//for (unsigned strip = 0; strip < numStrips; strip++)
		//{
		//	glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
		//		numTrisPerStrip + 2,   // number of indices to render
		//		GL_UNSIGNED_INT,     // index data type
		//		(void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
		//}

				// render the terrain
		glBindVertexArray(terrainVAO);
		glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS* rez* rez);
        
        //____
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


	// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------

	//glDeleteVertexArrays(1, &terrainVAO);
	//glDeleteBuffers(1, &terrainVBO);
	//glDeleteBuffers(1, &terrainIBO);
    cleanUp();

    glfwTerminate();
    return 0;
}

// initializes vertex buffers and all shaderprograms
void init_VAO_and_shaders()
{
    // create shaders
    programRender = Shader("VertexShader.shader", "FragmentShader.shader");
    programTildeHCompute = ComputeShader("TildeHCompute.shader");
    programButterflyTextureCompute = ComputeShader("ButterflyTextureCompute.shader");
    programFourierComponentCompute = ComputeShader("FourierComponentCompute.shader");
    programButterflyCompute = ComputeShader("ButterflyCompute.shader");
    programInversionCompute = ComputeShader("InversionCompute.shader");
	//Shader programRender("VertexShader.vert", "FragmentShader.frag");
 //   ComputeShader programTildeHCompute("TildeHCompute.shader");
 //   ComputeShader programButterflyTextureCompute("ButterflyTextureCompute.shader");
 //   ComputeShader programFourierComponentCompute("FourierComponentCompute.shader");
 //   ComputeShader programButterflyCompute("ButterflyCompute.shader");
 //   ComputeShader programInversionCompute("InversionCompute.shader");


    // create vertex objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // setup vertex array
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // setup vertex buffer ( mapping from coordinates (x,y) to texture coord (u,v)
    Vertex vertices[] = {
        //    x           y       z      u  v
        { { -1.0f,    -1.0f,    0 },{ 0, 0 } }, // bottom-left
        { { +1.0f,    -1.0f,    0 },{ 1, 0 } }, // bottom-right
        { { +1.0f,    +1.0f,    0 },{ 1, 1 } }, // top-right
        { { -1.0f,    +1.0f,    0 },{ 0, 1 } }, // top-left
    };
	//Vertex vertices[] = {
	//	//    x           y       z      u  v
	//	{ { -float(N) / 2.0f,    -float(N) / 2.0f,    0 },{ 0, 0 } }, // bottom-left
	//	{ { +float(N) / 2.0f,    -float(N) / 2.0f,    0 },{ 1, 0 } }, // bottom-right
	//	{ { +float(N) / 2.0f,    +float(N) / 2.0f,    0 },{ 1, 1 } }, // top-right
	//	{ { -float(N) / 2.0f,    +float(N) / 2.0f,    0 },{ 0, 1 } }, // top-left
	//};

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //一直到render里的VAO
}

// initializes all used textures
void init_textures()
{
    texture_tilde_h0k = Texture(false, N, N);
    texture_tilde_h0minusk = Texture(false, N, N);
    texture_random_noise_1 = Texture(true, N, N);
    texture_random_noise_2 = Texture(true, N, N);
    texture_random_noise_3 = Texture(true, N, N);
    texture_random_noise_4 = Texture(true, N, N);

    texture_butterfly = Texture(false, log_2_N, N);

    texture_fourier_component_dx = Texture(false, N, N);
    texture_fourier_component_dy = Texture(false, N, N);
    texture_fourier_component_dz = Texture(false, N, N);

    texture_pingpong_0 = Texture(false, N, N);
    texture_pingpong_1 = Texture(false, N, N);

    texture_displacement_of_points_on_grid = Texture(false, N, N);

}

// create tilde h0k and tilde h0minusk textures
void create_h0k_h0minusk_textures() {

    // bind image units of tilde_h0k and h0minusk
    glBindImageTexture(0, texture_tilde_h0k.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, texture_tilde_h0minusk.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    // bind image units of noise textures 
    glBindImageTexture(2, texture_random_noise_1.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(3, texture_random_noise_2.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(4, texture_random_noise_3.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(5, texture_random_noise_4.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA8);

	//// set uniform variables of the shader
	//programTildeHCompute.setInt("N", N);
	////programTildeHCompute.setInt("L", L);
 //   programTildeHCompute.SetUniform1i("L", L);
	//programTildeHCompute.setFloat("A", A);
	//programTildeHCompute.setFloat("windSpeed", windSpeed);
	////programTildeHCompute.setVec2("windDirection", windDirection);
 //   programTildeHCompute.SetUniform1fv("windDirection", windDirection);
	//programTildeHCompute.setInt("choppy", choppy);

	// set uniform variables of the shader
	programTildeHCompute.SetUniform1i("N", N);
	programTildeHCompute.SetUniform1i("L", L);
	programTildeHCompute.SetUniform1f("A", A);
	programTildeHCompute.SetUniform1f("windSpeed", windSpeed);
	programTildeHCompute.SetUniform1fv("windDirection", windDirection);
	programTildeHCompute.SetUniform1i("choppy", choppy);


    // run the tildeHCompute shader to write to textures
    //programTildeHCompute.compute(N, N);
	programTildeHCompute.use();
	glDispatchCompute(N, N, 1);
    glUseProgram(0);

}

// create butterfly texture and reversed bit indices
void create_butterfly_texture() {

    //generate reversed bit indices
    bitReversedIndices = new int[N];
    int bits = (log(N) / log(2));
    for (int i = 0; i < N; i++)
    {
        unsigned int x = reverseBits(i);
        x = (x << bits) | (x >> (INT_SIZE - bits));
        bitReversedIndices[i] = x;
    }

    // create the shader storage buffer that passes bitReversedIndices
    unsigned int reverseIndicesSSBO;
    glGenBuffers(1, &reverseIndicesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, reverseIndicesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * N, bitReversedIndices, GL_STATIC_DRAW);
    delete[] bitReversedIndices;
    // buffer assigned to binding index 0
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, reverseIndicesSSBO);
    // unbind buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // bind output texture we are writing to 
    glBindImageTexture(1, texture_butterfly.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

	//programButterflyTextureCompute.setInt("N", N);
    programButterflyTextureCompute.SetUniform1i("N", N);

	// run the butterfly compute shader to write to butterfly texture
	//programButterflyTextureCompute.compute(N, N);
	programButterflyTextureCompute.use();
	glDispatchCompute(N, N, 1);
    glUseProgram(0);
}

// create fourier components dx, dy and dz textures
void create_fourier_components()
{
    // bind the h0k and h0minusk to image unit 0 and 1
    glBindImageTexture(0, texture_tilde_h0k.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, texture_tilde_h0minusk.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);

    // bind image units used in fourier component compute shader to dx dy dz write textures
    glBindImageTexture(2, texture_fourier_component_dx.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(3, texture_fourier_component_dy.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(4, texture_fourier_component_dz.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

	//// set uniform variables of the shader
	//programFourierComponentCompute.setFloat("time", t);
	//programFourierComponentCompute.setInt("N", N);
	//programFourierComponentCompute.setInt("L", L);
	// set uniform variables of the shader
	programFourierComponentCompute.SetUniform1f("time", t);
	programFourierComponentCompute.SetUniform1i("N", N);
	programFourierComponentCompute.SetUniform1i("L", L);

	// run the shader to write to dx, dy, dz textures
	//programFourierComponentCompute.compute(N, N);
	programFourierComponentCompute.use();
	glDispatchCompute(N, N, 1);
    glUseProgram(0);
}


// butterfly fourier compututation
void fft()
{
    // bind image units used in butterfly texture compute shader
    glBindImageTexture(0, texture_butterfly.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, texture_fourier_component_dy.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(2, texture_pingpong_1.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // one dimensional FFT in horizontal direction 
    for (int stage = 0; stage < log_2_N; stage++)
    {
		//programButterflyCompute.updateButterflyComputeUniforms(pingpong_index, 0, stage);
		//programButterflyCompute.setInt("pingpong_index", pingpong_index);
		//programButterflyCompute.setInt("direction", 0);
		//programButterflyCompute.setInt("stage", stage);
		programButterflyCompute.SetUniform1i("pingpong_index", pingpong_index);
		programButterflyCompute.SetUniform1i("direction", 0);
		programButterflyCompute.SetUniform1i("stage", stage);

		//programButterflyCompute.compute(N, N);
		programButterflyCompute.use();
		glDispatchCompute(N, N, 1);
        glUseProgram(0);

        glFinish();
        pingpong_index = !pingpong_index;
    }

    // one dimensional FFT in vertical direction   
    for (int stage = 0; stage < log_2_N; stage++)
    {
		//programButterflyCompute.updateButterflyComputeUniforms(pingpong_index, 1, stage);
		//programButterflyCompute.setInt("pingpong_index", pingpong_index);
		//programButterflyCompute.setInt("direction", 1);
		//programButterflyCompute.setInt("stage", stage);
		programButterflyCompute.SetUniform1i("pingpong_index", pingpong_index);
		programButterflyCompute.SetUniform1i("direction", 1);
		programButterflyCompute.SetUniform1i("stage", stage);
		//programButterflyCompute.compute(N, N);
		programButterflyCompute.use();
		glDispatchCompute(N, N, 1);
        glUseProgram(0);

        glFinish();
        pingpong_index = !pingpong_index;
    }
}

// inversion and permutation compute shader
void inversion()
{
    // bind image units used in inversion compute shader, texture pinpong1 is an empty texture, the dy component serves as texture pingpong0
    glBindImageTexture(0, texture_displacement_of_points_on_grid.getID(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, texture_fourier_component_dy.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(2, texture_pingpong_1.getID(), 0, false, 0, GL_READ_ONLY, GL_RGBA32F);

	// set uniform variables of the shader
	//programInversionCompute.setInt("pingpong", pingpong_index);
	//programInversionCompute.setInt("N", N);
	programInversionCompute.SetUniform1i("pingpong", pingpong_index);
	programInversionCompute.SetUniform1i("N", N);
	// run the shader to write to the pingpong textures
	//programInversionCompute.compute(N, N);
	programInversionCompute.use();
	glDispatchCompute(N, N, 1);
    glUseProgram(0);
}

// set up OpenGL, GLFW, GLEW
void setUpLibraries()
{
    // initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    // create window
    window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
    }

    // set OpenGL version 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window); 


    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
    }
}

// setting calback functions
void setCallbackFunctions(void) {
    //glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

// runs renderProgram clears buffer
void render()
{
    glClearColor(0.0f, 0.1f, 0.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // render quad
    //programRender.bind();
    programRender.use();

    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
    //programRender.unbind()
    glUseProgram(0);
}

// calls all destructors
void cleanUp()
{
    // delete all objects
    glDeleteProgram(programTildeHCompute.ID);
    glDeleteProgram(programRender.ID);
    glDeleteProgram(programButterflyTextureCompute.ID);
    glDeleteProgram(programFourierComponentCompute.ID);
    glDeleteProgram(programButterflyCompute.ID);
    glDeleteProgram(programInversionCompute.ID);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    texture_random_noise_1.deleteTexture();
    texture_random_noise_2.deleteTexture();
    texture_random_noise_3.deleteTexture();
    texture_random_noise_4.deleteTexture();

    texture_tilde_h0k.deleteTexture();
    texture_tilde_h0minusk.deleteTexture();
    texture_butterfly.deleteTexture();

    texture_pingpong_0.deleteTexture();
    texture_pingpong_1.deleteTexture();

    texture_displacement_of_points_on_grid.deleteTexture();
}

// is called when keys are pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //if (action != GLFW_RELEASE) 
    //{ 
    //    return; 
    //}

    //switch (key) {
    //case GLFW_KEY_ESCAPE:
    //    glfwSetWindowShouldClose(window, true);
    //    break;
    //case GLFW_KEY_C:
    //    choppy = !choppy;
    //    break;
    //case GLFW_KEY_A:                // amplitude
    //    pressed = 'A';
    //    break;
    //case GLFW_KEY_L:                // L
    //    pressed = 'L';
    //    break;
    //case GLFW_KEY_S:                // windspeed
    //    pressed = 'S';
    //    break;


    //case GLFW_KEY_RIGHT_BRACKET:    // german keyboard: +
    //    switch (pressed)
    //    {
    //    case 'A':
    //        A++;
    //        break;
    //    case 'L':
    //        L+=10;
    //        break;
    //    case 'S':
    //        windSpeed++;
    //        break;
    //    }
    //    break;

    //case GLFW_KEY_SLASH:            // german keyboard: -
    //    switch (pressed)
    //    {
    //    case 'A':
    //        A--;
    //        break;
    //    case 'L':
    //        L-=10;
    //        break;
    //    case 'S':
    //        windSpeed--;
    //        break;
    //    }
    //    break;
    //}

}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
        //std::cout << "W" << std::endl;
    }

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}



// reverses the bits of a given unsigned int 
unsigned int reverseBits(unsigned int num)
{
    unsigned int  NO_OF_BITS = sizeof(num) * 8;
    unsigned int reverse_num = 0;
    int i;
    for (i = 0; i < NO_OF_BITS; i++)
    {
        if ((num & (1 << i)))
            reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
    }
    return reverse_num;
}
