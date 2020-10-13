#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <procrocklib/pipeline.h>

#include <glm/glm.hpp>
#include <iostream>
#include <map>

#include "../render/framebuffer.h"
#include "../render/shader.h"
#include "node-editor/imnodes.h"
#include "noise_node_editor.h"

namespace procrock {
namespace gui {

void init(GLFWwindow* window, const std::string& path);
void update(glm::uvec2 windowSize, Framebuffer& viewerFrame, Pipeline& pipeline,
            const Shader& shader);

void updateMainMenu(Pipeline& pipeline);
void updateSideBar(glm::uvec2 windowSize, Pipeline& pipeline);
void udpateCurrentStageEditor(glm::uvec2 windowSize);
void updateViewer(glm::uvec2 windowSize, Framebuffer& viewerFrame);
void updateStatusBar(glm::uvec2 windowSize);
void updateWindows(const Shader& shader);

void updatePipelineStage(Pipeline& pipeline, PipelineStage& stage);
void updateConfigurable(PipelineStage& stage);

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
};

struct CurrentStageEditor {
  PipelineStage* current = nullptr;
  int width = 350;
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
  glm::vec3 clearColor = glm::vec3(0.6);
  struct Light {
    float yaw = 0;
    float pitch = 0;
    float intensity = 100;
    glm::vec3 color = glm::vec3(1);
  };

  std::vector<Light> lights;
  glm::vec3 ambientColor = glm::vec3(0.01);

  struct GroundPlane {
    bool show = false;
    float height = -0.4f;
    int textureChoice = 0;
  } groundPlane;
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
extern CurrentStageEditor currentStageEditor;

extern NoiseNodeEditor noiseNodeEditor;

}  // namespace gui

}  // namespace procrock