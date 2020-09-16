#pragma once

#include <procrocklib/configurables/graph.h>
#include <procrocklib/configurables/noise_graph.h>

namespace procrock {
namespace gui {

class NoiseNodeEditor {
 public:
  void show();

 private:
  NoiseGraph graph;
  int rootNodeId = -1;
  std::vector<std::unique_ptr<NoiseNode>> nodes;
};
}  // namespace gui
}  // namespace procrock