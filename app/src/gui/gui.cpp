#include "gui.h"

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

  // ImGui::ShowDemoWindow();

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(320, frameBufferSize.y));

  ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize);
  if (ImGui::CollapsingHeader("Main", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderInt("Test Value.", &settings.main.test, 1, 3000);
    ImGui::SameLine();
    HelpMarker("Test help.");
    ImGui::Checkbox("Test Bool", &settings.main.test2);
    ImGui::SameLine();
  }
  ImGui::End();

  ImGui::Begin("Viewer");

  viewer.size = glm::uvec2(glm::uvec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()));
  viewerFrame.resize(viewer.size);

  ImGui::Image((ImTextureID)viewerFrame.getRenderedTextures()[0],
               ImVec2(viewer.size.x, viewer.size.y - 35), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();

  ImGui::EndFrame();
  ImGui::Render();
}

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }
}  // namespace gui

}  // namespace procRock