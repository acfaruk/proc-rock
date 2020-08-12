#include "gui.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <iostream>

namespace procrock {
namespace gui {

MainMenu mainMenu;
SideBar sideBar;
Viewer viewer;
StatusBar statusBar;

void init(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void update(glm::uvec2 windowSize, Framebuffer& viewerFrame) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  updateMainMenu();
  updateSideBar(windowSize);
  updateViewer(windowSize, viewerFrame);
  updateStatusBar(windowSize);

  ImGui::EndFrame();
  ImGui::Render();
}

void updateMainMenu() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char* test = tinyfd_openFileDialog("Open A Pipeline", "", 0, NULL, NULL, 0);
        if (test != NULL) {
          std::cout << test << std::endl;
        }
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void updateSideBar(glm::uvec2 windowSize) {
  ImGui::SetNextWindowPos(ImVec2(0, mainMenu.height));
  ImGui::SetNextWindowSize(
      ImVec2((float)sideBar.width, (float)windowSize.y - mainMenu.height - statusBar.height));
  ImGui::Begin("Settings", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  sideBar.width = (int)ImGui::GetWindowWidth();
  ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;

  if (ImGui::BeginTabBar("Tab Bar", tabBarFlags)) {
    if (ImGui::BeginTabItem("Pipeline")) {
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("View")) {
      updateViewSettings();
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
  ImGui::End();
}

void updateViewer(glm::uvec2 windowSize, Framebuffer& viewerFrame) {
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::SetNextWindowPos(ImVec2((float)sideBar.width, (float)mainMenu.height));
  ImGui::SetNextWindowSize(ImVec2((float)windowSize.x - sideBar.width,
                                  (float)windowSize.y - mainMenu.height - statusBar.height));

  ImGui::Begin("Viewer", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  viewer.position = glm::uvec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
  viewer.size = glm::uvec2(glm::uvec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
  viewer.focused = ImGui::IsWindowFocused();
  viewer.howered = ImGui::IsWindowHovered();
  viewerFrame.resize(viewer.size);

  ImGui::Image((ImTextureID)(uint64_t)viewerFrame.getRenderedTextures()[0],
               ImVec2((float)viewer.size.x, (float)viewer.size.y - statusBar.height), ImVec2(0, 1),
               ImVec2(1, 0));
  ImGui::End();
}

void updateStatusBar(glm::uvec2 windowSize) {
  ImGui::SetNextWindowPos(ImVec2(0.0f, (float)windowSize.y - statusBar.height));
  ImGui::SetNextWindowSize(ImVec2((float)windowSize.x, (float)statusBar.height));

  ImGui::Begin("Status Bar", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

  ImGui::Text("Ready");
  ImGui::SameLine((float)windowSize.x - 330);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  ImGui::End();
}

void updateViewSettings() {
  if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderAngle("Yaw", &sideBar.viewSettings.light.yaw, 0.0f, 360.0f);
    ImGui::SliderAngle("Pitch", &sideBar.viewSettings.light.pitch, 0.0f, 360.0f);
    ImGui::ColorEdit3("Ambient", &sideBar.viewSettings.light.ambientColor[0]);
  }
}

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
}  // namespace gui

}  // namespace procrock