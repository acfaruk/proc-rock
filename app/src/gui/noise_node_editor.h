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
  void update();

  inline bool hasChanged() { return changed; };

 private:
  bool changed = false;
};

}  // namespace gui
}  // namespace procrock