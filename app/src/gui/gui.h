#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

#include "../render/framebuffer.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace procRock {
namespace gui {

void init(GLFWwindow* window);
void update(glm::vec2 frameBufferSize, Framebuffer& viewerFrame);
void render();

bool isCapturingMouse();

struct Main {
  int test = 400;
  bool test2 = true;
};

struct Settings {
  Main main;
};

struct Viewer {
  glm::uvec2 size;
};

extern Settings settings;
extern Viewer viewer;
}  // namespace gui

}  // namespace procRock