#pragma once
#include <procrocklib/pipeline.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "render/camera.h"
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
  std::unique_ptr<RenderTexture> renderTextureAlbedo;
  std::unique_ptr<RenderTexture> renderTextureNormal;
  std::unique_ptr<RenderTexture> renderTextureRoughness;
  std::unique_ptr<RenderTexture> renderTextureMetal;
  std::unique_ptr<RenderTexture> renderTextureAmbientOcc;

  std::unique_ptr<Pipeline> pipeline;
  std::unique_ptr<DrawableMesh> drawableMesh;
  std::unique_ptr<Framebuffer> viewerFramebuffer;
  std::unique_ptr<PointLight> pointLight;

  static void onCurrentWindowResize(GLFWwindow* window, int width, int height);
  static void onOpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* userParam);
};
}  // namespace procrock
