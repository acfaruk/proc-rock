#include "gui.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <iostream>

namespace procRock {
namespace gui {

MainMenu mainMenu;
SideBar sideBar;
Viewer viewer;
StatusBar statusBar;
const int MAIN_MENU_HEIGHT = 20;

static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void init(GLFWwindow* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void update(glm::uvec2 framebufferSize, Framebuffer& viewerFrame) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  updateMainMenu();
  updateSideBar(framebufferSize);
  updateViewer(framebufferSize, viewerFrame);
  updateStatusBar(framebufferSize);

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

void updateSideBar(glm::uvec2 framebufferSize) {
  ImGui::SetNextWindowPos(ImVec2(0, MAIN_MENU_HEIGHT));
  ImGui::SetNextWindowSize(
      ImVec2(sideBar.width, framebufferSize.y - mainMenu.height - statusBar.height));
  ImGui::Begin("Settings", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  sideBar.width = ImGui::GetWindowWidth();
  ImGui::End();
}

void updateViewer(glm::uvec2 framebufferSize, Framebuffer& viewerFrame) {
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::SetNextWindowPos(ImVec2(sideBar.width, MAIN_MENU_HEIGHT));
  ImGui::SetNextWindowSize(ImVec2(framebufferSize.x - sideBar.width,
                                  framebufferSize.y - MAIN_MENU_HEIGHT - statusBar.height));

  ImGui::Begin("Viewer", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  viewer.position = glm::uvec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
  viewer.size = glm::uvec2(glm::uvec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
  viewer.focused = ImGui::IsWindowFocused();
  viewer.howered = ImGui::IsWindowHovered();
  viewerFrame.resize(viewer.size);

  ImGui::Image((ImTextureID)viewerFrame.getRenderedTextures()[0],
               ImVec2(viewer.size.x, viewer.size.y - statusBar.height), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
}

void updateStatusBar(glm::uvec2 framebufferSize) {
  ImGui::SetNextWindowPos(ImVec2(0, framebufferSize.y - statusBar.height));
  ImGui::SetNextWindowSize(ImVec2(framebufferSize.x, statusBar.height));

  ImGui::Begin("Status Bar", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

  ImGui::Text("Ready");
  ImGui::SameLine(framebufferSize.x - 330);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  ImGui::End();
}

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
}  // namespace gui

}  // namespace procRock