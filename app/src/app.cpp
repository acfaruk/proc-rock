#include "app.h"

#include <iostream>

#include "gui/gui.h"

namespace procRock {
App* App::current = nullptr;
App::App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable)
    : resourcesPath(resPath) {
  if (!glfwInit()) {
    std::cerr << "GLFW Initialization failed..." << std::endl;
  }
  current = this;

  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

  window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, title.c_str(), NULL, NULL);

  glfwSetWindowSizeCallback(window, onCurrentWindowResize);
  glfwSetMouseButtonCallback(window, onCurrentWindowMouseButton);
  glfwSetCursorPosCallback(window, onCurrentWindowCursorChange);
  glfwSetScrollCallback(window, onCurrentWindowMouseScroll);

  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW Initialization failed..." << std::endl;
  }

  glfwSwapInterval(1);

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Resource Path is " << resourcesPath << std::endl;
}

App::~App() {
  if (window) {
    glfwDestroyWindow(window);
    window = 0;
  }
  glfwTerminate();
}

void App::run() {
  double t;
  double dt;
  double previousTime = 0;

  init();
  while (!glfwWindowShouldClose(window)) {
    t = glfwGetTime();
    dt = t - previousTime;
    if (dt > 0.005) {
      glfwPollEvents();
      previousTime = t;

      if (!this->update()) return;
      if (!this->render()) return;
      glfwSwapBuffers(window);
    }
  }
  end();
}

bool App::init() {
  gui::init(window);
  return true;
}

bool App::update() {
  gui::update(this->getFrameBufferSize());

  mouseState.lastPos = mouseState.pos;
  mouseState.scrollOffset = glm::dvec2(0, 0);
  return true;
}

bool App::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glDrawArrays(GL_TRIANGLES, 0, 3);

  gui::render();
  return true;
}

bool App::end() { return true; }

glm::uvec2 App::getFrameBufferSize() const {
  glm::ivec2 sizes(0);
  glfwGetFramebufferSize(window, &sizes.x, &sizes.y);
  return glm::uvec2(sizes);
}

glm::uvec2 App::getWindowSize() const {
  glm::ivec2 sizes(0);
  glfwGetWindowSize(window, &sizes.x, &sizes.y);
  return glm::uvec2(sizes);
}

void App::onCurrentWindowResize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  current->update();
  current->render();
  glfwSwapBuffers(window);
}

void App::onCurrentWindowCursorChange(GLFWwindow* window, double x, double y) {
  if (!gui::isCapturingMouse()) {
    current->mouseState.pos = glm::dvec2(x, y);
  }
}

void App::onCurrentWindowMouseButton(GLFWwindow* window, int button, int action, int mods) {
  if (!gui::isCapturingMouse()) {
    current->mouseState.leftPressed = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
  }
}

void App::onCurrentWindowMouseScroll(GLFWwindow* window, double x_offset, double y_offset) {
  if (!gui::isCapturingMouse()) {
    current->mouseState.scrollOffset = glm::dvec2(x_offset, y_offset);
  }
}
}  // namespace procRock
