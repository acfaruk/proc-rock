#pragma once

#include <imgui.h>
#include <procrocklib/configurables/graph.h>
#include <procrocklib/configurables/noise_graph.h>

#include "node-editor/imnodes.h"

namespace procrock {
namespace gui {
struct NoiseNodeEditor {
  imnodes::EditorContext* context = nullptr;
  NoiseGraph* current = nullptr;

  ImVec2 position{0.0f, 0.0f};
  float width = 0.0f;
  float maxHeight = 0.0f;

  void initialize(NoiseGraph* noiseGraph);
  void update();

  inline bool hasChanged() { return changed; };
  inline float getHeight() { return current == nullptr ? 0 : height; };

 private:
  float height = 300.0f;
  bool changed = false;
};

}  // namespace gui
}  // namespace procrock