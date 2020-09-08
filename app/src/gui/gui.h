#pragma once

#include <imgui.h>
#include <procrocklib/pipeline.h>

#include <glm/glm.hpp>
#include <map>

#include "../render/framebuffer.h"
#include "../render/shader.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace procrock {
namespace gui {

void init(GLFWwindow* window, const std::string& path);
void update(glm::uvec2 windowSize, Framebuffer& viewerFrame, Pipeline& pipeline,
            const Shader& shader);

void updateMainMenu();
void updateSideBar(glm::uvec2 windowSize, Pipeline& pipeline);
void updateViewer(glm::uvec2 windowSize, Framebuffer& viewerFrame);
void updateStatusBar(glm::uvec2 windowSize);
void updateWindows(const Shader& shader);

void updatePipelineStage(Pipeline& pipeline, PipelineStage& stage);
void updateConfigurable(Configurable& configurable);

void helpMarker(std::string& description);

bool isCapturingMouse();

void render();

struct StageSettings {
  bool visible = false;
};

struct MainMenu {
  const int height = 20;
};

struct SideBar {
  int width = 350;
  std::map<std::string, StageSettings> stageData;
};

struct Viewer {
  glm::uvec2 size;
  glm::uvec2 position;
  bool focused;
  bool howered;
};

struct StatusBar {
  const int height = 35;
};

struct Window {
  bool show = false;
};

struct TextureWindow : public Window {};

struct ViewSettingsWindow : public Window {
  bool wireframe = false;
  glm::vec3 clearColor = glm::vec3(0.235);
  struct Light {
    float yaw = 3.67f;
    float pitch = 4.01f;
    glm::vec3 ambientColor = glm::vec3(0.01);
  } light;
};

struct MeshInfoWindow : public Window {
  int vertices = 0;
  int faces = 0;
  int textureWidth = 0;
  int textureHeight = 0;
};

struct Windows {
  TextureWindow textureWindow;
  ViewSettingsWindow viewSettingsWindow;
  MeshInfoWindow meshInfoWindow;
};

extern MainMenu mainMenu;
extern SideBar sideBar;
extern Viewer viewer;
extern StatusBar statusBar;
extern Windows windows;

}  // namespace gui

}  // namespace procrock