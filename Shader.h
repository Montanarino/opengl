#ifndef SHADER_H
#define SHADER_H

#ifdef __EMSCRIPTEN__
    #include <glad/gles2.h>
#else
    #include <glad/gl.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath)
    : ID(0)
    {
        std::string vertexCode;
        std::string fragmentCode;

        {
            std::ifstream vShaderFile(vertexPath);
            std::ifstream fShaderFile(fragmentPath);

            if (!vShaderFile.is_open()) {
                std::cerr << "ERROR: Cannot open vertex shader: " << vertexPath << '\n';
                return;
            }

            if (!fShaderFile.is_open()) {
                std::cerr << "ERROR: Cannot open fragment shader: " << fragmentPath << '\n';
                return;
            }

            std::stringstream vShaderStream;
            std::stringstream fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }

        if (vertexCode.empty() || fragmentCode.empty()) {
            std::cerr << "ERROR: Shader source is empty\n";
            return;
        }

        #ifdef __EMSCRIPTEN__
        // Automatically patch Desktop GLSL (#version 330 core) to WebGL 2 (#version 300 es)
        auto patchForWebGL = [](std::string& code) {
            size_t pos = code.find("#version");
            if (pos != std::string::npos) {
                size_t endLine = code.find('\n', pos);
                code.replace(pos, endLine - pos, "#version 300 es\nprecision mediump float;");
            } else {
                code = "#version 300 es\nprecision mediump float;\n" + code;
            }
        };
        patchForWebGL(vertexCode);
        patchForWebGL(fragmentCode);
        #endif

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() 
    { 
        glUseProgram(ID); 
    }

    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }

    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
    void setMatrix4fv(const std::string &name, const glm::mat4 &matrix) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif