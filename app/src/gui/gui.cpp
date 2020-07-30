#include "gui.h"

#include <iostream>

namespace procRock {
namespace gui {

Settings settings;
Viewer viewer;

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

void update(glm::vec2 frameBufferSize, Framebuffer& viewerFrame) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Menu Bar
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Test")) {
        std::cout << "test" << std::endl;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGui::SetNextWindowPos(ImVec2(0, 20));
  ImGui::SetNextWindowSize(ImVec2(350, frameBufferSize.y - 30 - 20));

  ImGui::Begin("Settings", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  if (ImGui::CollapsingHeader("Main", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderInt("Test Value.", &settings.main.test, 1, 3000);
    ImGui::SameLine();
    HelpMarker("Test help.");
    ImGui::Checkbox("Test Bool", &settings.main.test2);
    ImGui::SameLine();
  }
  ImGui::End();

  ImGui::SetNextWindowBgAlpha(0);
  ImGui::SetNextWindowPos(ImVec2(350, 20));
  ImGui::SetNextWindowSize(ImVec2(frameBufferSize.x - 350, frameBufferSize.y - 30 - 20));

  ImGui::Begin("Viewer", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
  viewer.size = glm::uvec2(glm::uvec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
  viewerFrame.resize(viewer.size);

  ImGui::Image((ImTextureID)viewerFrame.getRenderedTextures()[0],
               ImVec2(viewer.size.x, viewer.size.y - 35), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(0, frameBufferSize.y - 30));
  ImGui::SetNextWindowSize(ImVec2(frameBufferSize.x, 30));

  ImGui::Begin("Status Bar", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

  ImGui::Text("Ready");
  ImGui::SameLine(frameBufferSize.x - 330);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  ImGui::End();

  ImGui::EndFrame();
  ImGui::Render();
}

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }
}  // namespace gui

}  // namespace procRock