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

void updateViewSettings();

void updatePipelineStage(Pipeline& pipeline, PipelineStage& stage);
void updateConfigurable(Configurable& configurable);

void helpMarker(std::string& description);

bool isCapturingMouse();

void render();

std::string inline addressToString(void const* address) {
  std::ostringstream result;
  result << address;
  return result.str();
}

struct StageSettings {
  bool visible = false;
};

struct ViewSettings {
  bool wireframe;
  struct Light {
    float yaw = 3.67f;
    float pitch = 4.01f;
    glm::vec3 ambientColor = glm::vec3(0.2);
  } light;
};

struct MainMenu {
  const int height = 20;
};

struct SideBar {
  int width = 350;
  ViewSettings viewSettings;
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

struct TextureWindow {
  bool show = false;
};

struct Windows {
  TextureWindow textureWindow;
};

extern MainMenu mainMenu;
extern SideBar sideBar;
extern Viewer viewer;
extern StatusBar statusBar;
extern Windows windows;

}  // namespace gui

}  // namespace procrock