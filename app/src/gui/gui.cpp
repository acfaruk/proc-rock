#include "gui.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

#include <iostream>

#include "icons_font_awesome5.h"

namespace procrock {
namespace gui {

MainMenu mainMenu;
SideBar sideBar;
Viewer viewer;
StatusBar statusBar;

void init(GLFWwindow* window, const std::string& path) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  ImGuiIO io = ImGui::GetIO();
  std::string font = path + "/fonts/RobotoMono-Regular.ttf";
  io.Fonts->AddFontFromFileTTF(font.c_str(), 17.0f);

  ImFontConfig config;
  config.MergeMode = true;
  config.GlyphMinAdvanceX = 17.0f;

  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  std::string icon_font = path + "/fonts/fa-solid-900.ttf";
  io.Fonts->AddFontFromFileTTF(icon_font.c_str(), 17.0f, &config, icon_ranges);
}

void update(glm::uvec2 windowSize, Framebuffer& viewerFrame, Pipeline& pipeline) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  updateMainMenu();
  updateSideBar(windowSize, pipeline);
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

void updateSideBar(glm::uvec2 windowSize, Pipeline& pipeline) {
  ImGui::SetNextWindowPos(ImVec2(0, mainMenu.height));
  ImGui::SetNextWindowSize(
      ImVec2((float)sideBar.width, (float)windowSize.y - mainMenu.height - statusBar.height));
  ImGui::Begin("Settings", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  sideBar.width = (int)ImGui::GetWindowWidth();
  ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
  if (ImGui::BeginTabBar("Tab Bar", tabBarFlags)) {
    if (ImGui::BeginTabItem("Pipeline")) {
      if (ImGui::CollapsingHeader("Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& gen = pipeline.getGenerator();
        updatePipelineStage(pipeline, gen, false, false);
      }

      if (ImGui::CollapsingHeader("Modifiers", ImGuiTreeNodeFlags_DefaultOpen)) {
        int selected_mod = -1;
        if (ImGui::Button("Add Modifier...", ImVec2(-1, 0))) {
          ImGui::OpenPopup("mod_select_popup");
        }
        if (ImGui::BeginPopup("mod_select_popup")) {
          for (int i = 0; i < IM_ARRAYSIZE(PipelineStage_Mod_All); i++) {
            if (ImGui::Selectable(PipelineStage_Mod_All[i])) {
              selected_mod = i;
              pipeline.addModifierFromId(i);
            }
          }
          ImGui::EndPopup();
        }

        for (int i = 0; i < pipeline.getModifierCount(); i++) {
          auto& mod = pipeline.getModifier(i);
          updatePipelineStage(pipeline, mod);
        }
      }

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
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
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
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  ImGui::Text("Proc-Rock InDev");
  ImGui::SameLine((float)windowSize.x - 370);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  ImGui::End();
}

void updateViewSettings() {
  ImGui::Checkbox("Wireframe Mode", &sideBar.viewSettings.wireframe);
  if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderAngle("Yaw", &sideBar.viewSettings.light.yaw, 0.0f, 360.0f);
    ImGui::SliderAngle("Pitch", &sideBar.viewSettings.light.pitch, 0.0f, 360.0f);
    ImGui::ColorEdit3("Ambient", &sideBar.viewSettings.light.ambientColor[0]);
  }
}

void updatePipelineStage(Pipeline& pipeline, PipelineStage& pipelineStage, bool moveable,
                         bool deleteable) {
  auto info = pipelineStage.getInfo();
  std::string id = pipelineStage.getId();
  sideBar.stageSettings.insert(std::pair<std::string, StageSettings>(id, {false}));
  ImGui::BeginChild(id.c_str(), ImVec2(0, 70), true);

  // Info and description
  ImGui::Text(info.name.c_str());
  ImGui::SameLine();
  helpMarker(info.description);

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(0, 25));

  if (deleteable) {
    ImGui::SameLine((float)sideBar.width - 50);
    if (ImGui::Button(ICON_FA_TIMES_CIRCLE)) {
      pipeline.removePipelineStage(&pipelineStage);
      sideBar.stageSettings[id].visible = false;
    }
  }

  if (ImGui::Button(ICON_FA_COG " Settings")) {
    sideBar.stageSettings[id].visible = !sideBar.stageSettings[id].visible;
  }

  if (moveable) {
    ImGui::SameLine((float)sideBar.width - 50);
    ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_DOWN);
    ImGui::SameLine((float)sideBar.width - 80);
    ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_UP);
  }

  pipelineStage.setChanged(false);

  // Configurable stuff of the stage
  if (sideBar.stageSettings[id].visible) {
    ImGui::SetNextWindowSize(ImVec2(sideBar.width, -1));
    ImGui::Begin(id.c_str(), &sideBar.stageSettings[id].visible);
    updateConfigurable(pipelineStage);
    ImGui::End();
  }
  ImGui::EndChild();
}

void updateConfigurable(Configurable& configurable) {
  auto config = configurable.getConfiguration();
  bool changed = false;

  for (auto var : config.singleChoices) {
    const char* preview_value = var.choices[*var.choice].name.c_str();
    if (ImGui::BeginCombo(var.entry.name.c_str(), preview_value)) {
      for (int i = 0; i < var.choices.size(); i++) {
        bool item_selected = i == *var.choice;

        ImGui::PushID((void*)(intptr_t)i);
        if (ImGui::Selectable(var.choices[i].name.c_str(), item_selected)) {
          *var.choice = i;
          changed = true;
        }
        if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
          ImGui::SetTooltip(var.choices[i].description.c_str());
        }
        if (item_selected) ImGui::SetItemDefaultFocus();
        ImGui::PopID();
      }
      ImGui::EndCombo();
    }
    ImGui::SameLine();
    helpMarker(var.entry.description);
  }
  for (auto var : config.floats) {
    ImGui::SliderFloat(var.entry.name.c_str(), var.data, var.min, var.max);
    changed |= ImGui::IsItemEdited();
    ImGui::SameLine();
    helpMarker(var.entry.description);
  }

  for (auto var : config.ints) {
    ImGui::SliderInt(var.entry.name.c_str(), var.data, var.min, var.max);
    changed |= ImGui::IsItemEdited();
    ImGui::SameLine();
    helpMarker(var.entry.description);
  }

  configurable.setChanged(changed);
}

void helpMarker(std::string& description) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(description.c_str());
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

bool isCapturingMouse() { return ImGui::GetIO().WantCaptureMouse; }

void render() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
}  // namespace gui

}  // namespace procrock