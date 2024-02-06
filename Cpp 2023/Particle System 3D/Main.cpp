#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void processInput(GLFWwindow* window, glm::vec3& cameraPos, float& yaw, float& pitch);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const float CAMERA_SPEED = 0.05f;
const float MOUSE_SENSITIVITY = 0.1f;

float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;

glm::vec3 cameraPos = glm::vec3(2.0f, 0.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);

GLuint shaderProgram;

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
    }
)";

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;

    Particle() : life(3.5f) {}
};

class ParticleEmitter {
public:
    glm::vec3 position;
    std::vector<Particle> particles;

    void EmitParticle() {
        Particle particle;
        particle.position = position;
        particle.velocity = glm::vec3((rand() % 100 - 50) / 100.0f, (rand() % 100 - 50) / 100.0f, (rand() % 100 - 50) / 100.0f);

        particle.color = glm::vec4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1.0f);

        particles.push_back(particle);
    }

    void Update(float deltaTime) {
        for (auto& particle : particles) {
            particle.life -= deltaTime * 0.5f;
            particle.position += particle.velocity * deltaTime;

            particle.velocity.y -= 0.5f * deltaTime;

            float sphereRadius = 0.5f;
            glm::vec3 sphereCenter(0.0f, -1.0f, 0.0f);  
            glm::vec3 particleToCenter = particle.position - sphereCenter;
            float distanceToCenter = glm::length(particleToCenter);
            if (distanceToCenter < sphereRadius) {
                
                particle.velocity = glm::reflect(particle.velocity, glm::normalize(particleToCenter));
            }
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.life <= 0.0f; }), particles.end());
    }

    void Render() {
        renderParticles();

        std::cout << "Current number of particles: " << particles.size() << std::endl;
    }

private:
    void renderParticles() {
        GLuint vbo, vao;
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
        glEnableVertexAttribArray(1);

        glPointSize(5.0f);
        glDrawArrays(GL_POINTS, 0, particles.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glBindVertexArray(0);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
};

class ParticleGenerator {
public:
    ParticleEmitter* emitter;
    float emitInterval;
    float currentTime;
    int maxParticles;

    ParticleGenerator(ParticleEmitter* _emitter, float _emitInterval, int _maxParticles)
        : emitter(_emitter), emitInterval(_emitInterval), currentTime(0.0f), maxParticles(_maxParticles) {}

    void Update(float deltaTime) {
        currentTime += deltaTime;
        while (currentTime >= emitInterval && emitter->particles.size() < maxParticles) {
            emitter->EmitParticle();
            currentTime -= emitInterval;
        }
    }
};

void renderSphere(float radius, int stacks, int sectors, const glm::vec3& position) {
    std::vector<glm::vec3> vertices;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stacks;
        float xy = radius * glm::cos(stackAngle);
        float z = radius * glm::sin(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectors;
            float x = xy * glm::cos(sectorAngle);
            float y = xy * glm::sin(sectorAngle);

            vertices.push_back(glm::vec3(x, y, z) + position);
        }
    }

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(1200, 1000, "Particle System", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldn't load OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_DEPTH_TEST); 

    ParticleEmitter emitter;
    emitter.position = glm::vec3(0.0f, 0.0f, 0.0f);

    ParticleGenerator generator(&emitter, 0.001f, 5000);

    glm::mat4 projection = glm::perspective(glm::radians(30.0f), 1200.0f / 1000.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    float yaw = -90.0f;
    float pitch = 0.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    GLuint vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, emitter.position, yaw, pitch);

        renderSphere(1.5f, 100, 100, glm::vec3(0.0f, -1.0f, 0.0f));
        generator.Update(0.005f);
        emitter.Update(0.005f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        emitter.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
void processInput(GLFWwindow* window, glm::vec3& cameraPos, float& yaw, float& pitch) {
    float cameraSpeed = 2.5f * CAMERA_SPEED;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * glm::normalize(glm::vec3(glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * glm::normalize(glm::vec3(glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(glm::vec3(glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))), glm::vec3(0.0f, 1.0f, 0.0f))) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(glm::vec3(glm::cos(glm::radians(yaw)), 0.0f, glm::sin(glm::radians(yaw))), glm::vec3(0.0f, 1.0f, 0.0f))) * cameraSpeed;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = MOUSE_SENSITIVITY;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = MOUSE_SENSITIVITY;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    float& yaw = xoffset;
    float& pitch = yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
}