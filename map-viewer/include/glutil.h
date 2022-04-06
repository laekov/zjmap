//
// Created by zijia on 2022/4/4.
//

#ifndef ZJMAP_GLUTIL_H
#define ZJMAP_GLUTIL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <util.h>

template<class glFunc, class ...glArgs>
int GRLOpenGLFunctionCall(const char* code, const char* file, int line, glFunc&& func, glArgs && ...args)
{
    while (glGetError() != GL_NO_ERROR);
    func(args...);
    while (GLenum error = glGetError())
    {
        std::cout << "[ OpenGL Error ] (" << std::hex << error << ") " << code <<
                  " " << file << ":" << line << std::endl;
        return error;
    }
    return 0;
}

#if !defined(NDEBUG)
#define glcall(GLFUNC, ...) GRLOpenGLFunctionCall(#GLFUNC, __FILE__, __LINE__, GLFUNC, __VA_ARGS__)
#else
#define glcall(GLFUNC, ...) GRLOpenGLFunctionCall(#GLFUNC, __FILE__, __LINE__, GLFUNC, __VA_ARGS__)
#endif

int GLInit(int32_t width, int32_t height, GLFWwindow **window_ptr);

void ClearScreen(float r, float g, float b, float a);

void FlushScreen();

int ShouldClose();

GLFWwindow* GetWindowHandle();

uint32_t CompileShaderFromFile(const char * shader_file_path, uint32_t shader_type);

uint32_t LinkProgram(uint32_t vertex_shader, uint32_t fragment_shader);

#endif //ZJMAP_GLUTIL_H
