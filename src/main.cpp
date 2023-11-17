#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

void framebufferSizeCallback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow * window);
int fetchShader(std::string_view shaderPath, std::string & result);

// Constant color fragment shader
const char * fragmentShaderSource = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main() {\n"
                                    "   FragColor = vec4(0.15f, 0.3f, 0.6f, 1.0f);\n"
                                    "}\n";

int main()
{
    GLFWwindow * window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(800, 600, "Learning OpenGL", nullptr, nullptr);
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

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    float vertices[] = {
            -0.5f,  0.5f , 0.0f,
             0.5f,  0.5f , 0.0f,
             0.0f, -0.5f , 0.0f,
    };

    // Vertex Buffer
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); // move vertices to GL_ARRAY_BUFFER

    // Vertex Shader
    std::string * vertexShaderSource{};
    if (fetchShader("res/shaders/basic/vertex.shader", *vertexShaderSource)) {
     std::cerr << "Error happened fetching shader" << std::endl;
    }

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (char const * const *) vertexShaderSource->c_str(), nullptr);
    glCompileShader(vertexShader);

    { // Error handling
        int success;
        char log[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, log);
            std::cerr << "Vertex shader compilation failed: " << log << std::endl;
        }
    }

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    { // Error handling
        int success;
        char log[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, log);
            std::cerr << "Fragment shader compilation failed: " << log << std::endl;
        }
    }

    // Shader Program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    { // Error handling
        int success;
        char log[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
            std::cerr << "Shader program linking failed: " << log << std::endl;
        }
    }

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define vertices format
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    // Vertex array
    unsigned int vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // zero because it's the first and only (for now)

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Render here
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

int fetchShader(std::string_view shaderPath, std::string & result) {

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
    std::ostringstream source{};
    std::string line{};

    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    file.close();

    std::cout << "extracted file" << std::endl;
    std::cout << source.str() << std::endl;

    result = source.str();
    return 0;
}