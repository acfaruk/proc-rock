#include "input_manager.h"

#include <iostream>

#include "../gui/gui.h"

namespace procrock {
glm::ivec2 InputManager::currentMousePosition;
glm::ivec2 InputManager::lastMousePosition;
glm::ivec2 InputManager::clickMousePosition;

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

  // Left Click Drag
  if (currentMouseAction == GLFW_PRESS && currentMouseButton == GLFW_MOUSE_BUTTON_LEFT) {
    for (auto receiver : inputReceivers) {
      if (receiver->isViewerOnly()) {
        if (gui::viewer.focused) {
          receiver->mouseLeftDrag(lastMousePosition - glm::ivec2(gui::viewer.position),
                                  currentMousePosition - glm::ivec2(gui::viewer.position));
        }
      } else {
        receiver->mouseLeftDrag(lastMousePosition, currentMousePosition);
      }
    }
  }
}

void InputManager::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
  for (auto receiver : inputReceivers) {
    if (receiver->isViewerOnly()) {
      if (gui::viewer.howered) {
        receiver->mouseScroll(glm::dvec2(xoffset, yoffset));
      }
    } else {
      receiver->mouseScroll(glm::dvec2(xoffset, yoffset));
    }
  }
}

void InputManager::registerInputReceiver(InputReceiver* inputReceiver) {
  inputReceivers.push_back(inputReceiver);
}

}  // namespace procrock