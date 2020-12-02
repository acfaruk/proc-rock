#include "abstracted_pipeline.h"

namespace procrock {

void AbstractedPipeline::connect(Pipeline* pipeline) {
  this->pipeline = pipeline;
  this->pipeline->clear();
  this->setupPipeline();
  this->setChanged(true);
}

void AbstractedPipeline::disconnect() { this->pipeline = nullptr; }
bool AbstractedPipeline::isConnected() const { return pipeline != nullptr; }

void AbstractedPipeline::update() {
  if (!this->isConnected()) return;
  if (!this->isChanged() && pipeline->isChanged()) {
    this->disconnect();
    return;
  }
  if (this->isChanged()) {
    this->updatePipeline();
  }
  this->setChanged(false);
}
}  // namespace procrock