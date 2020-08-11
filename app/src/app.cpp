#include "app.h"

#include <procrocklib/mesh.h>

#include <iostream>

#include "gui/gui.h"
#include "io/input_manager.h"

namespace procRock {
App* App::current = nullptr;
App::App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable)
    : resourcesPath(resPath) {
#ifndef NDEBUG
  this->debug = true;
#endif
  // Initialize GLFW
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
  if (debug) {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  }
  window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, title.c_str(), NULL, NULL);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  glfwSetWindowSizeCallback(window, onCurrentWindowResize);
  glfwSetMouseButtonCallback(window, InputManager::onMouseButton);
  glfwSetCursorPosCallback(window, InputManager::onMousePos);
  glfwSetScrollCallback(window, InputManager::onMouseScroll);
  glfwSetWindowSizeLimits(window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW Initialization failed..." << std::endl;
  }

  glfwSwapInterval(1);

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Resource Path is " << resourcesPath << std::endl;

  if (debug) {
    if (GLEW_KHR_debug) {
      std::cout << "KHR debugging enabled." << std::endl;
      glDebugMessageCallback(onOpenGLDebugMessage, this);
    } else if (GLEW_ARB_debug_output) {
      std::cout << "ARB debugging enabled." << std::endl;
      glDebugMessageCallbackARB(onOpenGLDebugMessage, this);
    }
  }

  using namespace procrocklib;
  std::string teststr = "hello world from library part";
  test(teststr);
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
  mainCam->lookAt(glm::vec3(3, 3, 3), glm::vec3(0), glm::vec3(0, 1, 0));

  testCube = std::make_unique<Cube>();

  pointLight = std::make_unique<PointLight>(glm::vec3(7, 0, 0));

  mainShader = std::make_unique<Shader>(resourcesPath + "/shaders/main.vert",
                                        resourcesPath + "/shaders/main.frag");

  viewerFramebuffer = std::make_unique<Framebuffer>(glm::uvec2(200, 200));

  InputManager::registerInputReceiver(mainCam.get());

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  return true;
}

bool App::update() {
  gui::update(this->getWindowSize(), *viewerFramebuffer.get());
  mainCam->setViewport(gui::viewer.size);
  mainShader->uniforms3f["camPos"] = mainCam->getPosition();

  pointLight->setEulerAngles(gui::sideBar.viewSettings.light.yaw,
                             gui::sideBar.viewSettings.light.pitch);
  mainShader->uniforms3f["lightPos"] = pointLight->getPosition();
  mainShader->uniforms3f["lightColor"] = pointLight->getColor();
  mainShader->uniforms3f["ambientColor"] = gui::sideBar.viewSettings.light.ambientColor;

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
void App::onOpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar* message, const void* userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source: Other";
      break;
  }
  std::cout << std::endl;

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type: Other";
      break;
  }
  std::cout << std::endl;

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Severity: notification";
      break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
}  // namespace procRock
