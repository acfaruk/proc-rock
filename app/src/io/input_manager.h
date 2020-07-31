#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../render/gl_includes.h"

namespace procRock {

class InputReceiver {
 public:
  InputReceiver(bool viewerOnly) : viewerOnly(viewerOnly) {}
  virtual void mouseLeftDrag(glm::dvec2 pos1, glm::dvec2 pos2) = 0;
  virtual void mouseScroll(glm::dvec2 offset) = 0;

  bool isViewerOnly() { return viewerOnly; }

 protected:
  bool viewerOnly;  // Only interested in events that happen in the viewer specifically
};

class InputManager {
 public:
  static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void onMousePos(GLFWwindow* window, double xpos, double ypos);
  static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
  static void registerInputReceiver(InputReceiver* inputReceiver);

  static glm::ivec2 lastMousePosition;
  static glm::ivec2 clickMousePosition;

  static glm::ivec2 currentMousePosition;
  static int currentMouseAction;
  static int currentMouseButton;

  static std::vector<InputReceiver*> inputReceivers;
};
}  // namespace procRock