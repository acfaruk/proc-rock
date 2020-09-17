#include "gui.h"

#include <procrocklib/pipeline_stage_factory.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include <iostream>

#include "icons_font_awesome5.h"
#include "node-editor/imnodes.h"

namespace procrock {
namespace gui {

MainMenu mainMenu;
SideBar sideBar;
Viewer viewer;
StatusBar statusBar;
Windows windows;
std::unique_ptr<NoiseNodeEditor> noiseNodeEditor;

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
  imnodes::Initialize();
}

void update(glm::uvec2 windowSize, Framebuffer& viewerFrame, Pipeline& pipeline,
            const Shader& shader) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  updateMainMenu();
  updateSideBar(windowSize, pipeline);
  updateViewer(windowSize, viewerFrame);
  updateStatusBar(windowSize);
  updateWindows(shader);

  if (noiseNodeEditor.get() != nullptr) {
    noiseNodeEditor->show();
  }

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

    if (ImGui::BeginMenu("Windows")) {
      ImGui::MenuItem("Textures", 0, &windows.textureWindow.show);
      ImGui::MenuItem("View Settings", 0, &windows.viewSettingsWindow.show);
      ImGui::MenuItem("Mesh Info", 0, &windows.meshInfoWindow.show);
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
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_AlwaysVerticalScrollbar);
  sideBar.width = (int)ImGui::GetWindowWidth();
  ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
  if (ImGui::CollapsingHeader("Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Select Generator...", ImVec2(-1, 0))) {
      ImGui::OpenPopup("gen_select_popup");
    }
    if (ImGui::BeginPopup("gen_select_popup")) {
      for (int i = 0; i < IM_ARRAYSIZE(PipelineStageNames_Gen); i++) {
        if (ImGui::Selectable(PipelineStageNames_Gen[i])) {
          pipeline.setGenerator(createGeneratorFromId(i));
        }
      }
      ImGui::EndPopup();
    }

    auto& gen = pipeline.getGenerator();
    updatePipelineStage(pipeline, gen);
  }

  if (ImGui::CollapsingHeader("Modifiers", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Add Modifier...", ImVec2(-1, 0))) {
      ImGui::OpenPopup("mod_select_popup");
    }
    if (ImGui::BeginPopup("mod_select_popup")) {
      for (int i = 0; i < IM_ARRAYSIZE(PipelineStageNames_Mod); i++) {
        if (ImGui::Selectable(PipelineStageNames_Mod[i])) {
          pipeline.addModifier(createModifierFromId(i));
        }
      }
      ImGui::EndPopup();
    }

    for (int i = 0; i < pipeline.getModifierCount(); i++) {
      auto& mod = pipeline.getModifier(i);
      updatePipelineStage(pipeline, mod);
    }
  }

  if (ImGui::CollapsingHeader("Parameterizer", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Select Parameterizer...", ImVec2(-1, 0))) {
      ImGui::OpenPopup("par_select_popup");
    }
    if (ImGui::BeginPopup("par_select_popup")) {
      for (int i = 0; i < IM_ARRAYSIZE(PipelineStageNames_Par); i++) {
        if (ImGui::Selectable(PipelineStageNames_Par[i])) {
          pipeline.setParameterizer(createParameterizerFromId(i));
        }
      }
      ImGui::EndPopup();
    }

    auto& par = pipeline.getParameterizer();
    updatePipelineStage(pipeline, par);
  }

  if (ImGui::CollapsingHeader("Texture Generator", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::Button("Select Texture Gen...", ImVec2(-1, 0))) {
      ImGui::OpenPopup("texgen_select_popup");
    }
    if (ImGui::BeginPopup("texgen_select_popup")) {
      for (int i = 0; i < IM_ARRAYSIZE(PipelineStageNames_TexGen); i++) {
        if (ImGui::Selectable(PipelineStageNames_TexGen[i])) {
          pipeline.setTextureGenerator(createTextureGeneratorFromId(i));
        }
      }
      ImGui::EndPopup();
    }

    auto& texGen = pipeline.getTextureGenerator();
    updatePipelineStage(pipeline, texGen);
  }
  ImGui::End();
}

void updateViewer(glm::uvec2 windowSize, Framebuffer& viewerFrame) {
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::SetNextWindowPos(ImVec2((float)sideBar.width, (float)mainMenu.height));
  ImGui::SetNextWindowSize(ImVec2((float)windowSize.x - sideBar.width,
                                  (float)windowSize.y - mainMenu.height - statusBar.height));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Begin("Viewer", 0,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  viewer.position = glm::uvec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
  viewer.size = glm::uvec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
  viewer.focused = ImGui::IsWindowFocused();
  viewer.howered = ImGui::IsWindowHovered();
  viewerFrame.resize(viewer.size);

  ImGui::Image((ImTextureID)(uint64_t)viewerFrame.getRenderedTextures()[0],
               ImVec2((float)viewer.size.x, (float)viewer.size.y), ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
  ImGui::PopStyleVar();
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

void updateWindows(const Shader& shader) {
  if (windows.textureWindow.show) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(512, 580), ImVec2(512, 580));
    ImGui::Begin("Textures", &windows.textureWindow.show);
    ImGui::BeginTabBar("textures-tab-bar");

    for (auto& texture : shader.textures) {
      if (ImGui::BeginTabItem(texture.first.c_str())) {
        ImGui::Image((ImTextureID)(uint64_t)texture.second->getID(), ImVec2((float)512, (float)512),
                     ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndTabItem();
      }
    }

    ImGui::EndTabBar();
    ImGui::End();
  }

  if (windows.viewSettingsWindow.show) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(sideBar.width, 0), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin("View Settings", &windows.viewSettingsWindow.show);
    ImGui::Checkbox("Wireframe Mode", &windows.viewSettingsWindow.wireframe);
    ImGui::ColorEdit3("Clear Color", &windows.viewSettingsWindow.clearColor[0]);
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SliderAngle("Yaw", &windows.viewSettingsWindow.light.yaw, 0.0f, 360.0f);
      ImGui::SliderAngle("Pitch", &windows.viewSettingsWindow.light.pitch, 0.0f, 360.0f);
      ImGui::ColorEdit3("Ambient", &windows.viewSettingsWindow.light.ambientColor[0]);
    }
    ImGui::End();
  }

  if (windows.meshInfoWindow.show) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(sideBar.width, 0), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin("Mesh Info", &windows.meshInfoWindow.show);
    ImGui::Columns(2);
    ImGui::Text("Vertices");
    ImGui::NextColumn();
    ImGui::Text(std::to_string(windows.meshInfoWindow.vertices).c_str());
    ImGui::NextColumn();
    ImGui::Text("Faces");
    ImGui::NextColumn();
    ImGui::Text(std::to_string(windows.meshInfoWindow.faces).c_str());
    ImGui::NextColumn();
    ImGui::Text("Texture Width");
    ImGui::NextColumn();
    ImGui::Text(std::to_string(windows.meshInfoWindow.textureWidth).c_str());
    ImGui::NextColumn();
    ImGui::Text("Texture Height");
    ImGui::NextColumn();
    ImGui::Text(std::to_string(windows.meshInfoWindow.textureHeight).c_str());
    ImGui::End();
  }
}

void updatePipelineStage(Pipeline& pipeline, PipelineStage& stage) {
  stage.setChanged(false);

  auto info = stage.getInfo();
  std::string id = stage.getId();
  sideBar.stageData.insert(std::pair<std::string, StageSettings>(id, {false}));
  ImGui::BeginChild(id.c_str(), ImVec2(0, 70), true);

  // Info and description
  ImGui::Text(info.name.c_str());
  ImGui::SameLine();
  helpMarker(info.description);

  ImGui::SameLine();
  ImGui::Dummy(ImVec2(0, 25));

  if (stage.isRemovable()) {
    ImGui::SameLine((float)sideBar.width - 60);
    if (ImGui::Button(ICON_FA_TIMES_CIRCLE)) {
      ImGui::EndChild();
      pipeline.removePipelineStage(&stage);
      sideBar.stageData[id].visible = false;
      return;
    }
  }

  if (ImGui::Button(ICON_FA_COG " Settings")) {
    sideBar.stageData[id].visible = !sideBar.stageData[id].visible;
  }

  if (stage.isMoveable()) {
    ImGui::SameLine((float)sideBar.width - 60);
    if (ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_DOWN)) {
      pipeline.movePipelineStageDown(&stage);
    }
    ImGui::SameLine((float)sideBar.width - 90);
    if (ImGui::Button(ICON_FA_ARROW_ALT_CIRCLE_UP)) {
      pipeline.movePipelineStageUp(&stage);
    }
  }

  // Configurable stuff of the stage
  if (sideBar.stageData[id].visible) {
    ImGui::SetNextWindowSizeConstraints(ImVec2(sideBar.width, 100), ImVec2(FLT_MAX, viewer.size.y));
    ImGui::Begin(id.c_str(), &sideBar.stageData[id].visible);
    updateConfigurable(stage);
    ImGui::End();
  }
  ImGui::EndChild();
}

void updateConfigurable(Configurable& configurable) {
  auto config = configurable.getConfiguration();
  bool changed = false;
  int idCounter = 0;
  for (auto mainGroup : config.getConfigGroups()) {
    if (ImGui::CollapsingHeader(mainGroup.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
      for (auto group : mainGroup.second) {
        ImGui::PushID(idCounter++);
        ImGui::Text(group.entry.name.c_str());
        ImGui::Separator();
        ImGui::TextWrapped(group.entry.description.c_str());

        for (auto var : group.bools) {
          ImGui::Checkbox(var.entry.name.c_str(), var.data);
          changed |= ImGui::IsItemDeactivatedAfterEdit();
          ImGui::SameLine();
          helpMarker(var.entry.description);
        }

        for (auto var : group.singleChoices) {
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
        for (auto var : group.floats) {
          ImGui::SliderFloat(var.entry.name.c_str(), var.data, var.min, var.max);
          changed |= ImGui::IsItemDeactivatedAfterEdit();
          ImGui::SameLine();
          helpMarker(var.entry.description);
        }
        for (auto var : group.ints) {
          ImGui::SliderInt(var.entry.name.c_str(), var.data, var.min, var.max);
          changed |= ImGui::IsItemDeactivatedAfterEdit();
          ImGui::SameLine();
          helpMarker(var.entry.description);
        }

        for (auto var : group.float3s) {
          ImGui::SliderFloat3(var.entry.name.c_str(), var.data->data(), var.min.x(), var.max.x());
          changed |= ImGui::IsItemDeactivatedAfterEdit();
          ImGui::SameLine();
          helpMarker(var.entry.description);
        }

        for (auto gradient : group.gradientColorings) {
          ImGui::Text(gradient.entry.name.c_str());
          ImGui::SameLine();
          helpMarker(gradient.entry.description);
          int to_be_removed = -1;
          int id = 0;
          for (auto& x : *gradient.colors) {
            ImGui::PushID(id++);
            ImGui::Text(std::to_string(x.first).c_str());
            ImGui::SameLine(50);
            ImGui::ColorEdit3("", x.second.data(),
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            changed |= ImGui::IsItemDeactivatedAfterEdit();

            ImGui::SameLine(200);
            if (ImGui::Button(ICON_FA_TIMES_CIRCLE)) {
              to_be_removed = x.first;
              changed = true;
            }
            ImGui::PopID();
          }

          if (to_be_removed != -1) {
            gradient.colors->erase(to_be_removed);
          }

          static int to_be_added = 0;
          ImGui::SliderInt("", &to_be_added, 1, 100);
          ImGui::SameLine();
          if (ImGui::Button(ICON_FA_PLUS_CIRCLE)) {
            gradient.colors->emplace(to_be_added, Eigen::Vector3f{0, 0, 0});
            changed = true;
          }
        }

        for (auto var : group.noiseGraphs) {
          ImGui::Text(var.entry.name.c_str());
          ImGui::SameLine();
          helpMarker(var.entry.description);

          if (ImGui::Button("Edit Graph", ImVec2(-1, 30))) {
            noiseNodeEditor = std::make_unique<NoiseNodeEditor>(*var.data);
            noiseNodeEditor->visible = true;
          }
          changed = true;
        }

        ImGui::Dummy(ImVec2(0, 20));
        ImGui::PopID();
      }
    }
  }

  configurable.setChanged(changed);
}

void helpMarker(std::string& description) {
  ImGui::Text("(?)");
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