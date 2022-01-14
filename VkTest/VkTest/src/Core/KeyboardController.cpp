#include "KeyboardController.h"

void KeyboardController::Move(GLFWwindow* window, float dt, GameObject& gameObject) {
    glm::vec3 rotate{ 0.0f };
    if (glfwGetKey(window, m_Key.lookRight) == GLFW_PRESS)	rotate.y += 1.0f;
    if (glfwGetKey(window, m_Key.lookLeft) == GLFW_PRESS)	rotate.y -= 1.0f;
    if (glfwGetKey(window, m_Key.lookUp) == GLFW_PRESS)		rotate.x += 1.0f;
    if (glfwGetKey(window, m_Key.lookDown) == GLFW_PRESS)	rotate.x -= 1.0f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.rotation += m_LookSpeed * dt * glm::normalize(rotate);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
    const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
    const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

    glm::vec3 moveDir{ 0.0f };
    if (glfwGetKey(window, m_Key.moveForward) == GLFW_PRESS)	moveDir += forwardDir;
    if (glfwGetKey(window, m_Key.moveBackward) == GLFW_PRESS)	moveDir -= forwardDir;
    if (glfwGetKey(window, m_Key.moveRight) == GLFW_PRESS)		moveDir += rightDir;
    if (glfwGetKey(window, m_Key.moveLeft) == GLFW_PRESS)		moveDir -= rightDir;
    if (glfwGetKey(window, m_Key.moveUp) == GLFW_PRESS)			moveDir += upDir;
    if (glfwGetKey(window, m_Key.moveDown) == GLFW_PRESS)		moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += m_MoveSpeed * dt * glm::normalize(moveDir);
    }
}
