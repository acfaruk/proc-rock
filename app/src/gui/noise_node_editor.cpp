#include "noise_node_editor.h"

#include <imgui.h>

#include <iostream>

#include "../render/gl_includes.h"
#include "gui.h"

namespace procrock {
namespace gui {

void updateNoiseNodeEditor(NoiseNodeEditor& editor) {
  if (!*editor.visible) return;

  auto& graph = editor.current->graph;
  auto& nodes = editor.current->nodes;

  ImGui::Begin("Noise Node Editor");
  ImGui::TextUnformatted("Edit the noise generated with various modules.");
  ImGui::Columns(2);
  ImGui::TextUnformatted("A -- add node");
  ImGui::NextColumn();
  ImGui::TextUnformatted("X -- delete selected node or link");

  ImGui::NextColumn();

  if (ImGui::Button("Evaluate") && graph.get_root_node_id() != -1) {
    auto m = evaluateGraph(*editor.current);
    std::cout << m->GetValue(0.2, 0.3, 0.4) << std::endl;
  }
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
          int id = editor.current->addNode(std::make_unique<PerlinNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
        }

        if (ImGui::MenuItem("Const")) {
          int id = editor.current->addNode(std::make_unique<ConstNoiseNode>());
          imnodes::SetNodeScreenSpacePos(id, clickPos);
        }
        ImGui::EndMenu();
      }

      if (ImGui::MenuItem("Output") && graph.get_root_node_id() == -1) {
        int id = editor.current->addNode(std::make_unique<OutputNoiseNode>(), true);
        imnodes::SetNodeScreenSpacePos(id, clickPos);
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

    for (int i = 0; i < graph.num_edges_from_node(node->id); i++) {
      imnodes::BeginInputAttribute(*graph.neighbors(node->id).begin() + i);
      imnodes::EndInputAttribute();
    }
    if (node->id != graph.get_root_node_id()) {
      imnodes::BeginOutputAttribute(node->id);
      bool draggable = true;

      ImGui::PushItemWidth(100);
      for (auto var : config.floats) {
        ImGui::SliderFloat(var.entry.name.c_str(), var.data, var.min, var.max);
        draggable = draggable && !ImGui::IsItemActive();
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
    }
  }

  // Handle deleting links / nodes
  {
    int linkId;
    if (imnodes::IsLinkDestroyed(&linkId)) {
      graph.erase_edge(linkId);
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
      }
    }
  }

  ImGui::End();
}

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
}  // namespace gui
}  // namespace procrock