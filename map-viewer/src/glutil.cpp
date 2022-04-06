//
// Created by zijia on 2022/4/4.
//

#include "glutil.h"

static GLFWwindow * g_glfwwindow;

int GLInit(int32_t width, int32_t height, GLFWwindow **window_ptr) {
    GLFWwindow *&window = g_glfwwindow;
    /* Initialize the library */
    if (!glfwInit())
    {
        puts("[ERROR] GLFW init failed!");
        fflush(stdout);
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(width, height, u8"紫荆地图", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        puts("[ERROR] GLAD init failed!");
        fflush(stdout);
        glfwTerminate();
        return -1;
    }
    if (window_ptr) * window_ptr = g_glfwwindow;
    glfwSetWindowSizeCallback(g_glfwwindow, [](GLFWwindow * window, int width, int height){glViewport(0, 0, width, height);});
    std::cout << glGetString(GL_VERSION) << std::endl;
    std::cout.flush();
    return 0;
}



uint32_t CompileShaderFromFile(const char * shader_file_path, uint32_t shader_type)
{
    uint32_t shader = glCreateShader(shader_type);
    FILE * file;
    fopen_s(&file, shader_file_path, "rb");
    if (!file) return shader;
    fseek(file, 0, SEEK_END);
    int32_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *source_code = new char [file_size + 1];
    fread(source_code, 1, file_size, file);
    fclose(file);
    source_code[file_size] = 0;
    glcall(glShaderSource, shader, 1, &source_code, &file_size);
    glcall(glCompileShader, shader);
    delete [] source_code;
    int success;
    glcall(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int length;
        glcall(glGetShaderiv, shader, GL_INFO_LOG_LENGTH, &length);
        char * info_log = new char [length + 1];
        glcall(glGetShaderInfoLog, shader, length, &length, info_log);
        info_log[length] = 0;
        printf("[FAILED] Shader Compilation Error:\n%s\n", info_log);
        delete [] info_log;
        return shader;
    }
    return shader;
}

uint32_t LinkProgram(uint32_t vertex_shader, uint32_t fragment_shader)
{
    uint32_t program = glCreateProgram();
    glcall(glAttachShader, program, vertex_shader);
    glcall(glAttachShader, program, fragment_shader);
    glcall(glLinkProgram, program);
    int success;
    glcall(glGetProgramiv, program, GL_LINK_STATUS, &success);
    if (!success)
    {
        int length;
        glcall(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &length);
        char * info_log = new char [length + 1];
        glcall(glGetProgramInfoLog, program, length, &length, info_log);
        info_log[length] = 0;
        printf("[FAILED] Shader Compilation Error:\n%s\n", info_log);
        delete [] info_log;
        return program;
    }
    return program;
}

void ClearScreen(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void FlushScreen() {
    glfwSwapBuffers(g_glfwwindow);
    glfwPollEvents();
}

int ShouldClose() {
    return glfwWindowShouldClose(g_glfwwindow);
}

GLFWwindow *GetWindowHandle() {
    return g_glfwwindow;
}
