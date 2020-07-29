#include "app.h"

#include <iostream>

#include "gui/gui.h"
#include "io/input_manager.h"

namespace procRock {
App* App::current = nullptr;
App::App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable)
    : resourcesPath(resPath) {
  if (!glfwInit()) {
    std::cerr << "GLFW Initialization failed..." << std::endl;
  }
  current = this;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

  window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, title.c_str(), NULL, NULL);

  glfwSetWindowSizeCallback(window, onCurrentWindowResize);
  glfwSetMouseButtonCallback(window, InputManager::onMouseButton);
  glfwSetCursorPosCallback(window, InputManager::onMousePos);
  glfwSetScrollCallback(window, InputManager::onMouseScroll);

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

  mainCam = std::make_unique<Camera>(getFrameBufferSize());
  mainCam->lookAt(glm::vec3(2, 5, 2), glm::vec3(0), glm::vec3(1, 0, 0));

  testCube = std::make_unique<Cube>();

  mainShader = std::make_unique<Shader>(resourcesPath + "/shaders/main.vert",
                                        resourcesPath + "/shaders/main.frag");

  viewerFramebuffer = std::make_unique<Framebuffer>(glm::uvec2(200, 200));

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  return true;
}

bool App::update() {
  gui::update(this->getFrameBufferSize(), *viewerFramebuffer.get());
  mainCam->setViewport(gui::viewer.size);

  mainShader->setFVec3("camPos", mainCam->getPosition());
  return true;
}

bool App::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  viewerFramebuffer->bind();
  testCube->draw(*mainCam.get(), *mainShader.get());

  bindDefaultFrameBuffer(getFrameBufferSize());
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
}  // namespace procRock
