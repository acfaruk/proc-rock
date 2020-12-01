#include "abstracted_pipeline.h"

namespace procrock {
void AbstractedPipeline::connect(Pipeline* pipeline) { this->pipeline = pipeline; }
void AbstractedPipeline::disconnect() { this->pipeline = nullptr; }
bool AbstractedPipeline::isConnected() const { return pipeline != nullptr; }
void AbstractedPipeline::update() {
  if (!this->isConnected()) return;

  if (!this->isChanged() && pipeline->isChanged()) {
    this->disconnect();
    return;
  }
}
}  // namespace procrock