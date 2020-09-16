#include "noise_node_editor.h"

#include <imgui.h>

#include <iostream>

#include "../render/gl_includes.h"
#include "gui.h"
#include "node-editor/imnodes.h"

namespace procrock {
namespace gui {
void NoiseNodeEditor::show() {
  ImGui::Begin("Noise Node Editor");
  ImGui::TextUnformatted("Edit the noise generated with various modules.");
  ImGui::Columns(2);
  ImGui::TextUnformatted("A -- add node");
  ImGui::NextColumn();
  ImGui::TextUnformatted("X -- delete selected node or link");
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
          auto newNode = std::make_unique<PerlinNoiseNode>();
          newNode->id = graph.insert_node(newNode.get());
          imnodes::SetNodeScreenSpacePos(newNode->id, clickPos);
          nodes.push_back(std::move(newNode));
        }

        if (ImGui::MenuItem("Const")) {
          auto newNode = std::make_unique<ConstNoiseNode>();
          newNode->id = graph.insert_node(newNode.get());
          imnodes::SetNodeScreenSpacePos(newNode->id, clickPos);
          nodes.push_back(std::move(newNode));
        }
        ImGui::EndMenu();
      }

      if (ImGui::MenuItem("Output") && rootNodeId == -1) {
        auto newNode = std::make_unique<OutputNoiseNode>();
        newNode->id = graph.insert_node(newNode.get());
        imnodes::SetNodeScreenSpacePos(newNode->id, clickPos);
        rootNodeId = newNode->id;
        for (int i = 0; i < newNode->getModule()->GetSourceModuleCount(); i++) {
          auto inputNode = std::make_unique<ConstNoiseNode>();
          inputNode->placeholder = true;
          inputNode->id = graph.insert_node(inputNode.get());
          graph.insert_edge(newNode->id, inputNode->id);
          nodes.push_back(std::move(inputNode));
        }
        nodes.push_back(std::move(newNode));
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
  }

  // draw nodes & links

  for (auto& node : nodes) {
    if (node->placeholder) continue;

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
    if (node->id != rootNodeId) {
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
        if (nodeId == rootNodeId) rootNodeId = -1;

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
}  // namespace gui
}  // namespace procrock