#pragma once

#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include "OpenGLResource.h"

class CShaderGenerator
{
public:
	CShaderGenerator() = default;

    void CheckCompileErrors(unsigned int Shader, std::string Type)
    {
        int Success;
        char InfoLog[1024];
        if (Type != "PROGRAM")
        {
            glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
            if (!Success)
            {
                glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
            if (!Success)
            {
                glGetProgramInfoLog(Shader, 1024, NULL, InfoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << InfoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

	void GenerateBaseShaders(COpenGLResource* Resource)
	{        
        std::string VertexCodeString;
        std::string FragmentCodeString;
        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;
        const char* VertexPath = "Texture_GLSL.vs";
        const char* FragmentPath = "Texture_GLSL.fs";
        // ensure ifstream objects can throw exceptions:
        VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            VertexShaderFile.open(VertexPath);
            FragmentShaderFile.open(FragmentPath);
            std::stringstream VertexShaderStream, FragmentShaderStream;
            // read file's buffer contents into streams
            VertexShaderStream << VertexShaderFile.rdbuf();
            FragmentShaderStream << FragmentShaderFile.rdbuf();
            // close file handlers
            VertexShaderFile.close();
            FragmentShaderFile.close();
            // convert stream into string
            VertexCodeString = VertexShaderStream.str();
            FragmentCodeString = FragmentShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* VertexShaderCode = VertexCodeString.c_str();
        const char* FragmentShaderCode = FragmentCodeString.c_str();
        // 2. compile shaders
        unsigned int VertexShader, FragmentShader;
        // vertex shader
        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexShader, 1, &VertexShaderCode, NULL);
        glCompileShader(VertexShader);
        CheckCompileErrors(VertexShader, "VERTEX");
        // fragment Shader
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShader, 1, &FragmentShaderCode, NULL);
        glCompileShader(FragmentShader);
        CheckCompileErrors(FragmentShader, "FRAGMENT");
        // shader Program
        Resource->m_ShaderProgram = glCreateProgram();
        glAttachShader(Resource->m_ShaderProgram, VertexShader);
        glAttachShader(Resource->m_ShaderProgram, FragmentShader);
        glLinkProgram(Resource->m_ShaderProgram);
        CheckCompileErrors(Resource->m_ShaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);
	}

    void GenerateBaseSolidShaders(COpenGLResource* Resource)
    {
        std::string VertexCodeString;
        std::string FragmentCodeString;
        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;
        const char* VertexPath = "Texture_GLSL.vs";
        const char* FragmentPath = "Texture_GLSL.fs";
        // ensure ifstream objects can throw exceptions:
        VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            VertexShaderFile.open(VertexPath);
            FragmentShaderFile.open(FragmentPath);
            std::stringstream VertexShaderStream, FragmentShaderStream;
            // read file's buffer contents into streams
            VertexShaderStream << VertexShaderFile.rdbuf();
            FragmentShaderStream << FragmentShaderFile.rdbuf();
            // close file handlers
            VertexShaderFile.close();
            FragmentShaderFile.close();
            // convert stream into string
            VertexCodeString = VertexShaderStream.str();
            FragmentCodeString = FragmentShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* VertexShaderCode = VertexCodeString.c_str();
        const char* FragmentShaderCode = FragmentCodeString.c_str();
        // 2. compile shaders
        unsigned int VertexShader, FragmentShader;
        // vertex shader
        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexShader, 1, &VertexShaderCode, NULL);
        glCompileShader(VertexShader);
        CheckCompileErrors(VertexShader, "VERTEX");
        // fragment Shader
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShader, 1, &FragmentShaderCode, NULL);
        glCompileShader(FragmentShader);
        CheckCompileErrors(FragmentShader, "FRAGMENT");
        // shader Program
        Resource->m_ShaderProgram = glCreateProgram();
        glAttachShader(Resource->m_ShaderProgram, VertexShader);
        glAttachShader(Resource->m_ShaderProgram, FragmentShader);
        glLinkProgram(Resource->m_ShaderProgram);
        CheckCompileErrors(Resource->m_ShaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);
    }

    void GenerateLightShaders(COpenGLResource* Resource)
    {
        std::string VertexCodeString;
        std::string FragmentCodeString;
        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;
        const char* VertexPath = "Basic_Light.vs";
        const char* FragmentPath = "Basic_Light.fs";
        // ensure ifstream objects can throw exceptions:
        VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            VertexShaderFile.open(VertexPath);
            FragmentShaderFile.open(FragmentPath);
            std::stringstream VertexShaderStream, FragmentShaderStream;
            // read file's buffer contents into streams
            VertexShaderStream << VertexShaderFile.rdbuf();
            FragmentShaderStream << FragmentShaderFile.rdbuf();
            // close file handlers
            VertexShaderFile.close();
            FragmentShaderFile.close();
            // convert stream into string
            VertexCodeString = VertexShaderStream.str();
            FragmentCodeString = FragmentShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* VertexShaderCode = VertexCodeString.c_str();
        const char* FragmentShaderCode = FragmentCodeString.c_str();
        // 2. compile shaders
        unsigned int VertexShader, FragmentShader;
        // vertex shader
        VertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexShader, 1, &VertexShaderCode, NULL);
        glCompileShader(VertexShader);
        CheckCompileErrors(VertexShader, "VERTEX");
        // fragment Shader
        FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentShader, 1, &FragmentShaderCode, NULL);
        glCompileShader(FragmentShader);
        CheckCompileErrors(FragmentShader, "FRAGMENT");
        // shader Program
        Resource->m_ShaderProgram = glCreateProgram();
        glAttachShader(Resource->m_ShaderProgram, VertexShader);
        glAttachShader(Resource->m_ShaderProgram, FragmentShader);
        glLinkProgram(Resource->m_ShaderProgram);
        CheckCompileErrors(Resource->m_ShaderProgram, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(VertexShader);
        glDeleteShader(FragmentShader);
    }
};