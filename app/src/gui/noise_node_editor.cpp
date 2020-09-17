#include "noise_node_editor.h"

#include <imgui.h>

#include <iostream>

#include "../render/gl_includes.h"
#include "gui.h"

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
  if (!*this->visible) return;

  auto& graph = this->current->graph;
  auto& nodes = this->current->nodes;

  ImGui::Begin("Noise Node Editor");
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

        if (ImGui::MenuItem("Const")) {
          int id = this->current->addNode(std::make_unique<ConstNoiseNode>());
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
        ImGui::SliderFloat(var.entry.name.c_str(), var.data, var.min, var.max);
        draggable = draggable && !ImGui::IsItemActive();
        changed |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::SameLine();
        helpMarker(var.entry.description);
      }

      for (auto var : config.ints) {
        ImGui::SliderInt(var.entry.name.c_str(), var.data, var.min, var.max);
        draggable = draggable && !ImGui::IsItemActive();
        changed |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::SameLine();
        helpMarker(var.entry.description);
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

        auto iter = std::find_if(nodes.begin(), nodes.end(),
                                 [nodeId](const std::unique_ptr<NoiseNode>& node) -> bool {
                                   return node->id == nodeId;
                                 });
        auto& node = *iter;
        for (auto x : graph.neighbors(node->id)) {
          if (graph.node(x)->placeholder) {
            auto xIter = std::find_if(
                nodes.begin(), nodes.end(),
                [x](const std::unique_ptr<NoiseNode>& node) -> bool { return node->id == x; });
            nodes.erase(xIter);
            graph.erase_node(x);
            iter = std::find_if(nodes.begin(), nodes.end(),
                                [nodeId](const std::unique_ptr<NoiseNode>& node) -> bool {
                                  return node->id == nodeId;
                                });
          }
        }
        graph.erase_node(nodeId);
        nodes.erase(iter);
        changed = true;
      }
    }
  }

  ImGui::End();
}

}  // namespace gui
}  // namespace procrock