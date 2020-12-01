#pragma once
#include <procrocklib/pipeline.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "abstracted_pipeline/abstracted_pipeline.h"
#include "render/camera.h"
#include "render/drawables/ground.h"
#include "render/drawables/mesh.h"
#include "render/framebuffer.h"
#include "render/gl_includes.h"
#include "render/point_light.h"
#include "render/shader.h"

namespace procrock {

class App {
 public:
  App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable = true);
  ~App();

  void run();

  static App* current;  // pointer to this for glfw callbacks

  enum class LightingMode { Default = 0, Single = 1, WellLit = 2 };
  void changeLightingMode(const LightingMode mode);

  void setAbstractedPipeline(std::unique_ptr<AbstractedPipeline> abstractedPipeline);

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

  bool debug = false;  // debug mode

  std::unique_ptr<Camera> mainCam;
  std::unique_ptr<Shader> mainShader;

  std::map<std::string, std::unique_ptr<RenderTexture>> rockTexGroup;

  std::unique_ptr<Pipeline> pipeline;
  std::unique_ptr<AbstractedPipeline> abstractedPipeline;
  std::unique_ptr<DrawableMesh> drawableMesh;
  std::unique_ptr<Framebuffer> viewerFramebuffer;
  std::vector<PointLight> pointLights;

  std::unique_ptr<DrawableGround> groundPlane;
  std::map<int, std::map<std::string, std::unique_ptr<RenderTexture>>> groundTexGroups;

  LightingMode currentLightningMode = LightingMode::Default;

  static void onCurrentWindowResize(GLFWwindow* window, int width, int height);
  static void onOpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* userParam);
};
}  // namespace procrock
