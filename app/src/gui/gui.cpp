#include "gui.h"

#include <imgui.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace procRock {
namespace gui {

Settings settings;

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
  ImGui_ImplOpenGL3_Init();
}

void update(glm::vec2 frameBufferSize) {
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

  ImGui::EndFrame();
  ImGui::Render();
}

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }
}  // namespace gui

}  // namespace procRock