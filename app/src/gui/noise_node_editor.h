#pragma once

#include <procrocklib/configurables/graph.h>
#include <procrocklib/configurables/noise_graph.h>

namespace procrock {
namespace gui {

class NoiseNodeEditor {
 public:
  NoiseNodeEditor(NoiseGraph& graph);
  bool visible = true;
  void show();
 private:
  NoiseGraph& noiseGraph;
};
}  // namespace gui
}  // namespace procrock