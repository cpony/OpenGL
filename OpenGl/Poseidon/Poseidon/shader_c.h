#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include "OpenGLImports.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

enum class ShaderType {
	VERTEX = 0, COMPUTE = 1, FRAGMENT = 2
};

class ComputeShader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    ComputeShader() 
    {
    }
    
    ComputeShader(const char* computePath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string computeCode;
        std::ifstream cShaderFile;
        // ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            cShaderFile.open(computePath);

            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << cShaderFile.rdbuf();
            // close file handlers
            cShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* cShaderCode = computeCode.c_str();
        // 2. compile shaders
        unsigned int compute;
        // compute shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        checkCompileErrors(compute, "COMPUTE");
        
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(compute);
    }
    


    
 //   ComputeShader(const char* computeFilePath)
	//{
	//	this->ID = glCreateProgram();

	//	GLuint computeShader = createShader(computeFilePath, ShaderType::COMPUTE);

	//	glAttachShader(ID, computeShader);
	//	glLinkProgram(ID);

	//	int success;
	//	char infoLog[512];
	//	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	//	if (!success)
	//	{

	//		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
	//		std::cout << "ERROR: Shader Linking failed \n" << infoLog << std::endl;
	//	}

	//	glDeleteShader(computeShader);
	//}


 //   GLuint createShader(const char* filePath, ShaderType type) {

 //       GLenum shaderType;
 //       switch (type)
 //       {
 //       case ShaderType::VERTEX:
 //           shaderType = GL_VERTEX_SHADER;
 //           break;
 //       case ShaderType::COMPUTE:
 //           shaderType = GL_COMPUTE_SHADER;
 //           break;
 //       case ShaderType::FRAGMENT:
 //           shaderType = GL_FRAGMENT_SHADER;
 //           break;
 //       default:
 //           break;
 //       }

 //       std::string absoluteFilePath = std::filesystem::absolute(filePath).u8string();
 //       std::ifstream filestream(absoluteFilePath, std::ios::in);

 //       std::string content;

 //       std::string line = "";
 //       while (!filestream.eof())
 //       {
 //           std::getline(filestream, line);
 //           content.append(line + "\n");
 //       }
 //       filestream.close();
 //       const char* shaderSource = content.c_str();

 //       GLuint shader = glCreateShader(shaderType);
 //       glShaderSource(shader, 1, &shaderSource, NULL);

 //       glCompileShader(shader);
 //       GLint succeded;
 //       glGetShaderiv(shader, GL_COMPILE_STATUS, &succeded);

 //       if (succeded == GL_FALSE)
 //       {
 //           //logging
 //           GLint logSize;
 //           glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
 //           GLchar* message = new char[logSize];
 //           glGetShaderInfoLog(shader, logSize, NULL, message);
 //           std::cout << "compile error in Shader program with id " << ID << "...";
 //           std::cout << message << std::endl;
 //           delete[] message;
 //       }

 //       return shader;
 //   }

    

	void compute(int width, int height, int depth=1) {
		use();
		dispatchCompute(width, height, depth);
        glUseProgram(0);
	}

	void dispatchCompute(int width, int height, int depth)
	{
		glDispatchCompute(width, height, depth);
	}

    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }

    //_______
	void SetUniform1fv(const std::string& name, glm::vec2 vector)
	{
        //glUniform2f(glGetUniformLocation(ID, name.c_str()), vector.x, vector.y);
        glProgramUniform2f(ID, glGetUniformLocation(ID, name.c_str()), vector.x, vector.y);
	}
	void SetUniform1i(const std::string& name, int value)
	{
		glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), value);
	}
	void SetUniform1f(const std::string& name, float value)
	{
		glProgramUniform1f(ID, glGetUniformLocation(ID, name.c_str()), value);
	}
    //_______
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif