#include <iostream>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#include "Model.h"
#include "ShaderCompiler.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string const g_vertexShaderFile = "C:/Users/sonu/Graphics/openGL/shader101/shader101/vertex.glsl";
std::string const g_fragmentShaderFile = "C:/Users/sonu/Graphics/openGL/shader101/shader101/fragment.glsl";

// camera
std::shared_ptr<Camera> g_camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
float g_lastX = SCR_WIDTH / 2.0f;
float g_lastY = SCR_HEIGHT / 2.0f;
bool g_firstMouse = true;

// timing
float g_deltaTime = 0.0f;	// time between current frame and last frame
float g_lastFrame = 0.0f;

void makeLookAt() {
    glm::vec3 pos = g_camera->Position;
    glm::vec3 target = pos + g_camera->Front;
    glm::vec3 worldUp(0, 1, 0);
    glm::vec3 up, forward, right;

    forward = glm::normalize(pos - target);
    right = glm::cross(worldUp, forward);
    up = glm::cross(forward, right);
    glm::mat4 rot;
    rot[0] = glm::vec4(right, 0);
    rot[1] = glm::vec4(up, 0);
    rot[2] = glm::vec4(forward, 0);
    rot[3] = glm::vec4(0, 0, 0, 1);

    std::cout << "Rot:" << rot << std::endl;
    glm::mat4 translation(1.0f);
    translation[3] = glm::vec4(pos, 1.0);
    std::cout << "translation:"<<translation << std::endl;

    std::cout << "camtoWorld:" << translation * rot << std::endl;
    std::cout << "worldToCam" << glm::transpose(rot) * glm::inverse(translation) << std::endl;

}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    std::shared_ptr<Shader> program = std::make_shared<Shader>(g_vertexShaderFile.c_str(), g_fragmentShaderFile.c_str());
    std::vector<std::unique_ptr<Model>> cubes;
 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // world space positions of our cubes
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

    int numCubes = sizeof(cubePositions) / sizeof(glm::vec3);
    for (int i = 0; i < numCubes; ++i) {
        cubes.emplace_back(std::make_unique<Model>());
        cubes.back()->Initialize();
        cubes.back()->SetProjectionMatrix(projection);
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, cubePositions[i]);

        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        cubes.back()->SetModelMatrix(model);
    }
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
// --------------------
        float currentFrame = glfwGetTime();
        g_deltaTime = currentFrame - g_lastFrame;
        g_lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // camera/view transformation
        glm::mat4 const &view = g_camera->GetViewMatrix();
      //  makeLookAt();
       // std::cout << "Camera view: " << view << std::endl;

        for (int i = 0; i < numCubes; ++i) {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
           // glm::mat4 temp(1.0f);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            //temp = glm::transpose(temp);
            cubes[i]->SetModelMatrix(model);
            program->use();
            program->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
            program->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            //program->setVec3("lightPos", 7.0f, 2.0f, 4.0f);
            program->setVec3("lightPos", g_camera->Position);
            program->setVec3("viewPos", g_camera->Position);
     
            cubes[i]->Draw(program, view);
        }
       // std::cout << glm::vec3(view[3]) << std::endl;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        glfwSwapInterval(1);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    for (int i = 0; i < numCubes; ++i) {
        cubes[i]->Destroy();
    }
    
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera->ProcessKeyboard(FORWARD, g_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera->ProcessKeyboard(BACKWARD, g_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera->ProcessKeyboard(LEFT, g_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera->ProcessKeyboard(RIGHT, g_deltaTime);
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_firstMouse)
    {
        g_lastX = xpos;
        g_lastY = ypos;
        g_firstMouse = false;
    }

    float xoffset = xpos - g_lastX;
    float yoffset = g_lastY - ypos; // reversed since y-coordinates go from bottom to top

    g_lastX = xpos;
    g_lastY = ypos;

    g_camera->ProcessMouseMovement(xoffset, yoffset);
}