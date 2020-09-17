#pragma once

#include <procrocklib/configurables/graph.h>
#include <procrocklib/configurables/noise_graph.h>

#include "node-editor/imnodes.h"

namespace procrock {
namespace gui {
struct NoiseNodeEditor {
  imnodes::EditorContext* context = nullptr;
  NoiseGraph* current = nullptr;
  bool* visible;

  void initialize(NoiseGraph* noiseGraph);
};
void updateNoiseNodeEditor(NoiseNodeEditor& editor);

}  // namespace gui
}  // namespace procrock