#include "input_manager.h"

#include "../gui/gui.h"

namespace procRock {
glm::dvec2 InputManager::currentMousePosition;
glm::dvec2 InputManager::lastMousePosition;
glm::dvec2 InputManager::clickMousePosition;

int InputManager::currentMouseAction = 0;
int InputManager::currentMouseButton = 0;

std::vector<InputReceiver*> InputManager::inputReceivers;

void InputManager::onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (!gui::isCapturingMouse()) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }
}

void InputManager::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
  currentMouseAction = action;
  currentMouseButton = button;
}

void InputManager::onMousePos(GLFWwindow* window, double xpos, double ypos) {
  lastMousePosition = currentMousePosition;
  currentMousePosition = glm::dvec2(xpos, ypos);

  if (!gui::isCapturingMouse()) {
    // Left Click Drag
    if (currentMouseAction == GLFW_PRESS && currentMouseButton == GLFW_MOUSE_BUTTON_LEFT) {
      for (auto receiver : inputReceivers) {
        receiver->mouseLeftDrag(lastMousePosition, currentMousePosition);
      }
    }
  }
}

void InputManager::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
  if (!gui::isCapturingMouse()) {
    for (auto receiver : inputReceivers) {
      receiver->mouseScroll(glm::dvec2(xoffset, yoffset));
    }
  }
}

void InputManager::registerInputReceiver(InputReceiver* inputReceiver) {
  inputReceivers.push_back(inputReceiver);
}

}  // namespace procRock