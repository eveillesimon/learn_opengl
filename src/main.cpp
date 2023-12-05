#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <valarray>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ShaderSources {
    std::string vertex;
    std::string fragment;
};

void framebufferSizeCallback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow * window);
int parseShaders(std::string_view shaderPath, ShaderSources &sources);
int compileAndLinkShaders(const ShaderSources& sources, unsigned int &shaderProgram);

int main()
{
    using namespace std;
    GLFWwindow * window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 800, "Learning OpenGL", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "GLFW failed to open a window (oh no cringe)" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initializing glew
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "GLEW encountered a problem while initializing: " << glewGetErrorString(err) << std::endl;
    }

    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    float vertices[] = {
               -0.5f,  0.5f, 0.0f, 1.0f,  0.96f, 0.87f,
               -0.5f, -0.5f, 0.0f, 1.0f,  0.41f, 0.41f,
                0.5f, -0.5f, 0.0f, 0.78f, 0.0f,  0.22f,
                0.5f,  0.5f, 0.0f, 0.07f, 0.11f, 0.27f,
    };

    unsigned int indices[] = {
            0, 1, 2,
            0, 2, 3
    };

    // Vertex Buffer
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); // move vertices to GL_ARRAY_BUFFER


    // Element Buffer
    unsigned int elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    // Shaders
    ShaderSources sources;
    if (parseShaders("res/shaders/3colors.shader", sources) != 0) {
        std::cerr << "Could not parse shaders" << std::endl;
        return -1;
    }

    unsigned int shaderProgram;
    if (compileAndLinkShaders(sources, shaderProgram) != 0) {
        std::cerr << "Could not compile or use shaders" << std::endl;
        return -1;
    }

    // Define vertices format
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    // Vertex array
    unsigned int vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0); // zero because it's the first and only (for now)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Loading texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("res/images/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Could not load texture" << std::endl;
    }

    stbi_image_free(data);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Render here
        glClearColor(0.07f / 3.2f, 0.11f / 3.2f, 0.27f / 3.2f, 1.0f / 3.2f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        auto time = (float) glfwGetTime();

        // greenColor uniform
        float shift = std::sin(time * 2.0f) / 2.0f + .5f;
        int shiftColourUniform = glGetUniformLocation(shaderProgram, "shiftColor");
        glUniform1f(shiftColourUniform, shift);

        // hOffset uniform
        float offset[2] = {
                static_cast<float>(std::cos(time * 2.0f) / 2.0f),
                static_cast<float>(std::sin(time * 2.0f) / 2.0f)
        };
        int offsetUniform = glGetUniformLocation(shaderProgram, "offset");
        glUniform2f(offsetUniform, offset[0], offset[1]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int parseShaders(const std::string_view shaderPath, ShaderSources &sources) {

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    if (!std::filesystem::exists(shaderPath)) {
        std::cerr << "The file " << shaderPath.data() << " does not exist" << std::endl;
        return -1;
    }
    std::ifstream file{};
    file.open(shaderPath.data());

    if ((file.rdstate() & file.fail()) != 0) {
        std::cerr << "Failed to open " << shaderPath.data() << std::endl;
        return -1;
    }
    std::stringstream sstream[2];
    std::string line;

    ShaderType currentMode = ShaderType::NONE;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                currentMode = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                currentMode = ShaderType::FRAGMENT;
            } else {
                std::cerr << "Shader type could not be found on line " << lineCount << std::endl;
                return -1;
            }
        } else {
            if (currentMode == ShaderType::NONE) {
                std::cerr << "Your shader should have a descriptor on line 1 (ex: \"#shader vertex\")" << std::endl;
                return -1;
            }
            sstream[(int)currentMode] << line << '\n';
        }
    }
    file.close();

    sources.vertex = sstream[(int)ShaderType::VERTEX].str();
    sources.fragment = sstream[(int)ShaderType::FRAGMENT].str();

    return 0;
}

int compileAndLinkShaders(const ShaderSources& sources, unsigned int &shaderProgram) {
    int shaderIds[2];
    int shaderTypes[2] = {
            GL_VERTEX_SHADER,
            GL_FRAGMENT_SHADER
    };
    const char * source[2] = {
            sources.vertex.c_str(),
            sources.fragment.c_str()
    };
    shaderProgram = glCreateProgram();

    for (int i = 0; i < 2; i++) {
        shaderIds[i] = glCreateShader(shaderTypes[i]);
        glShaderSource(shaderIds[i], 1, &source[i], nullptr);
        glCompileShader(shaderIds[i]);

        int success;
        char log[512];
        glGetShaderiv(shaderIds[i], GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shaderIds[i], 512, nullptr, log);
            std::cerr << "Shader compilation failed: " << log << std::endl;
            return -1;
        }

        glAttachShader(shaderProgram, shaderIds[i]);
    }

    glLinkProgram(shaderProgram);

    { // Error handling
        int success;
        char log[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
            std::cerr << "Shader program linking failed: " << log << std::endl;
            return -1;
        }
    }

    glUseProgram(shaderProgram);

    for (int shaderId : shaderIds) {
        glDeleteShader(shaderId);
    }

    return 0;
}