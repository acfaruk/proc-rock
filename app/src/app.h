#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <string>

namespace procRock {

class App {
 public:
  App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable = true);
  ~App();

  void run();

  static App* current;  // pointer to this for glfw callbacks

 private:
  bool init();
  bool update();
  bool render();
  bool end();

  glm::uvec2 getFrameBufferSize() const;
  glm::uvec2 getWindowSize() const;

  GLFWwindow* window;
  const std::string title;
  const std::string resourcesPath;

  struct MouseState {
    bool leftPressed = false;
    glm::dvec2 pos = glm::dvec2(0, 0);
    glm::dvec2 lastPos = glm::dvec2(0, 0);
    glm::dvec2 scrollOffset = glm::dvec2(0, 0);

  } mouseState;

  static void onCurrentWindowResize(GLFWwindow* window, int width, int height);
  static void onCurrentWindowCursorChange(GLFWwindow* window, double x, double y);
  static void onCurrentWindowMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void onCurrentWindowMouseScroll(GLFWwindow* window, double x_offset, double y_offset);
};
}  // namespace procRock
