#include "noise_node_editor.h"

#include <imgui.h>

#include <iostream>

#include "../render/gl_includes.h"
#include "gui.h"
#include "icons_font_awesome5.h"

namespace procrock {
namespace gui {

void NoiseNodeEditor::initialize(NoiseGraph* noiseGraph) {
  imnodes::EditorContextFree(this->context);
  this->context = imnodes::EditorContextCreate();
  this->current = noiseGraph;
  imnodes::EditorContextSet(this->context);

  for (auto& node : this->current->nodes) {
    if (node->placeholder) continue;
    imnodes::SetNodeGridSpacePos(node->id, ImVec2(node->position.x(), node->position.y()));
  }
}

void NoiseNodeEditor::update() {
  changed = false;
  if (this->current == nullptr) return;

  auto& graph = this->current->graph;
  auto& nodes = this->current->nodes;

  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(ImVec2(width, height));
  ImGui::SetNextWindowSizeConstraints(ImVec2(width, 100), ImVec2(width, maxHeight));
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, 0);
  ImGui::Begin("Noise Node Editor", 0,
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
  height = ImGui::GetWindowHeight();
  ImGui::TextUnformatted("Edit the noise generated with various modules.");
  ImGui::Columns(2);
  ImGui::TextUnformatted("A -- add node");
  ImGui::NextColumn();
  ImGui::TextUnformatted("X -- delete selected node or link");

  ImGui::NextColumn();
  ImGui::Columns(1);

  imnodes::BeginNodeEditor();
  // new nodes
  {
    const bool openPopup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                           imnodes::IsEditorHovered() && ImGui::IsKeyReleased(GLFW_KEY_A);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
    if (!ImGui::IsAnyItemHovered() && openPopup) {
      ImGui::OpenPopup("Add Node");
    }

    if (ImGui::BeginPopup("Add Node")) {
      const ImVec2 clickPos = ImGui::GetMousePosOnOpeningCurrentPopup();
      if (ImGui::BeginMenu("Generators")) {
        if (ImGui::MenuItem("Perlin")) {
          int id = this->current->addNode(std::make_unique<PerlinNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Billow")) {
          int id = this->current->addNode(std::make_unique<BillowNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("RidgedMulti")) {
          int id = this->current->addNode(std::make_unique<RidgedMultiNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Voronoi Cells")) {
          int id = this->current->addNode(std::make_unique<VoronoiNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Cylinders")) {
          int id = this->current->addNode(std::make_unique<CylindersNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Spheres")) {
          int id = this->current->addNode(std::make_unique<SpheresNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Const")) {
          int id = this->current->addNode(std::make_unique<ConstNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Modifiers")) {
        if (ImGui::MenuItem("Abs")) {
          int id = this->current->addNode(std::make_unique<AbsNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Clamp")) {
          int id = this->current->addNode(std::make_unique<ClampNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Exponent")) {
          int id = this->current->addNode(std::make_unique<ExponentNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Invert")) {
          int id = this->current->addNode(std::make_unique<InvertNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Scale & Bias")) {
          int id = this->current->addNode(std::make_unique<ScaleBiasNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Terrace")) {
          int id = this->current->addNode(std::make_unique<TerraceNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Transformers")) {
        if (ImGui::MenuItem("Displace")) {
          int id = this->current->addNode(std::make_unique<DisplaceNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Rotate Point")) {
          int id = this->current->addNode(std::make_unique<RotatePointNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Scale Point")) {
          int id = this->current->addNode(std::make_unique<ScalePointNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Translate Point")) {
          int id = this->current->addNode(std::make_unique<TranslatePointNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Turbulence")) {
          int id = this->current->addNode(std::make_unique<TurbulenceNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Combiners")) {
        if (ImGui::MenuItem("Add")) {
          int id = this->current->addNode(std::make_unique<AddNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Max")) {
          int id = this->current->addNode(std::make_unique<MaxNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Min")) {
          int id = this->current->addNode(std::make_unique<MinNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Multiply")) {
          int id = this->current->addNode(std::make_unique<MultiplyNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        if (ImGui::MenuItem("Power")) {
          int id = this->current->addNode(std::make_unique<PowerNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Selectors")) {
        if (ImGui::MenuItem("Blend")) {
          int id = this->current->addNode(std::make_unique<BlendNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }

        if (ImGui::MenuItem("Select")) {
          int id = this->current->addNode(std::make_unique<SelectNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
          changed = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::MenuItem("Output") && graph.get_root_node_id() == -1) {
        int id = this->current->addNode(std::make_unique<OutputNoiseNode>(), true);
        imnodes::SetNodeScreenSpacePos(id, clickPos);
        changed = true;
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
  }

  // draw nodes & links

  for (auto& node : nodes) {
    if (node->placeholder) continue;

    auto pos = imnodes::GetNodeGridSpacePos(node->id);
    node->position = Eigen::Vector2f(pos.x, pos.y);

    auto config = node->getConfig();
    imnodes::BeginNode(node->id);
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(config.entry.name.c_str());
    ImGui::SameLine();
    gui::helpMarker(config.entry.description);
    imnodes::EndNodeTitleBar();

    int counter = 1;
    for (auto neighbourId : graph.neighbors(node->id)) {
      imnodes::BeginInputAttribute(neighbourId);
      ImGui::TextUnformatted(std::string("Input " + std::to_string(counter++)).c_str());
      imnodes::EndInputAttribute();
    }

    if (node->id != graph.get_root_node_id()) {
      imnodes::BeginOutputAttribute(node->id);
      bool draggable = true;
      ImGui::PushItemWidth(100);
      for (auto var : config.bools) {
        ImGui::Checkbox(var.entry.name.c_str(), var.data);
        changed |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::SameLine();
        helpMarker(var.entry.description);
      }

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
        ImGui::SliderFloat(var.entry.name.c_str(), var.data, var.minValue, var.maxValue);
        draggable = draggable && !ImGui::IsItemActive();
        changed |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::SameLine();
        helpMarker(var.entry.description);
      }

      for (auto var : config.ints) {
        ImGui::SliderInt(var.entry.name.c_str(), var.data, var.minValue, var.maxValue);
        draggable = draggable && !ImGui::IsItemActive();
        changed |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::SameLine();
        helpMarker(var.entry.description);
      }

      for (auto var : config.floatLists) {
        ImGui::Text(var.entry.name.c_str());
        ImGui::SameLine();
        helpMarker(var.entry.description);
        float to_be_removed = NAN;
        int id = 0;
        for (auto x : var.data->values()) {
          ImGui::PushID(id++);
          ImGui::Text(std::to_string(x).c_str());
          ImGui::SameLine(50);

          ImGui::SameLine(200);
          if (ImGui::Button(ICON_FA_TIMES_CIRCLE)) {
            to_be_removed = x;
            changed = true;
          }
          ImGui::PopID();
        }

        if (!isnan(to_be_removed)) {
          var.data->erase(to_be_removed);
        }

        static float to_be_added = 0;
        ImGui::SliderFloat("", &to_be_added, var.minValue, var.maxValue);
        draggable = draggable && !ImGui::IsItemActive();
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLUS_CIRCLE)) {
          var.data->insert(to_be_added);
          changed = true;
        }
      }

      imnodes::SetNodeDraggable(node->id, draggable);

      ImGui::PopItemWidth();
      imnodes::EndOutputAttribute();
    }
    imnodes::EndNode();
  }

  // draw links

  for (const auto& edge : graph.edges()) {
    auto node1 = graph.node(edge.from);
    auto node2 = graph.node(edge.to);

    if (node2->placeholder) continue;
    imnodes::Link(edge.id, edge.from, edge.to);
  }
  imnodes::EndNodeEditor();

  // Handle new links
  {
    int from, to;
    if (imnodes::IsLinkCreated(&from, &to)) {
      auto fromNode = graph.node(from);
      auto toNode = graph.node(to);

      if (toNode->placeholder) {
        std::swap(from, to);
      }

      if (graph.num_edges_from_node(from) > 0) {
        for (auto& otherTo : graph.neighbors(from)) {
          graph.erase_edge(graph.edge_id(from, otherTo));
        }
      }
      graph.insert_edge(from, to);
      changed = true;
    }
  }

  // Handle deleting links / nodes
  {
    int linkId;
    if (imnodes::IsLinkDestroyed(&linkId)) {
      graph.erase_edge(linkId);
      changed = true;
    }
  }

  {
    const int numSelected = imnodes::NumSelectedLinks();
    if (numSelected > 0 && ImGui::IsKeyReleased(GLFW_KEY_X)) {
      static std::vector<int> selectedLinks;
      selectedLinks.resize(static_cast<size_t>(numSelected));
      imnodes::GetSelectedLinks(selectedLinks.data());
      for (const int edgeId : selectedLinks) {
        graph.erase_edge(edgeId);
        changed = true;
      }
    }
  }

  {
    const int numSelected = imnodes::NumSelectedNodes();
    if (numSelected > 0 && ImGui::IsKeyReleased(GLFW_KEY_X)) {
      static std::vector<int> selectedNodes;
      selectedNodes.resize(static_cast<size_t>(numSelected));
      imnodes::GetSelectedNodes(selectedNodes.data());
      for (const int nodeId : selectedNodes) {
        if (nodeId == graph.get_root_node_id()) graph.set_root_node_id(-1);
        std::vector<int> toErase;
        for (int x : graph.neighbors(nodeId)) {
          if (graph.node(x)->placeholder) {
            auto xIter = std::find_if(
                nodes.begin(), nodes.end(),
                [=](const std::unique_ptr<NoiseNode>& node) -> bool { return node->id == x; });
            toErase.push_back(x);
            nodes.erase(xIter);
          }
        }
        for (int x : toErase) {
          graph.erase_node(x);
        }
        auto iter = std::find_if(
            nodes.begin(), nodes.end(),
            [=](const std::unique_ptr<NoiseNode>& node) -> bool { return node->id == nodeId; });
        graph.erase_node(nodeId);
        nodes.erase(iter);
        changed = true;
      }
    }
  }

  ImGui::End();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
}

}  // namespace gui
}  // namespace procrock