#pragma once

#include <Core/Window.h>
#include <Core/GameObject.h>

class KeyboardController {
public:
    void Move(GLFWwindow* window, float dt, GameObject& gameObject);
public:
    struct Keys {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_E;
        int moveDown = GLFW_KEY_Q;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };
private:
    Keys m_Key;
    float m_MoveSpeed{ 3.0f };
    float m_LookSpeed{ 1.5f };
};