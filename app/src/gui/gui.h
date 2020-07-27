#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace procRock {
namespace gui {

void init(GLFWwindow* window);
void update(glm::vec2 frameBufferSize);
void render();

bool isCapturingMouse();

struct Main {
  int test = 400;
  bool test2 = true;
};

struct Settings {
  Main main;
};
extern Settings settings;
}  // namespace gui

}  // namespace procRock