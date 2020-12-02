#include "igneous_pipeline.h"

namespace procrock {
IgneousPipeline::IgneousPipeline() {
  Configuration::ConfigurationGroup group;
  group.entry = {"Form", "Change the base form of the igneous rock."};
  group.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Change the form drastically."}, &seed, 0, 1000000});

  config.insertToConfigGroups("General", group);
}

void IgneousPipeline::setupPipeline() {
  auto gen = std::make_unique<SkinSurfaceGenerator>();
  this->generator = gen.get();
  this->pipeline->setGenerator(std::move(gen));
}

void IgneousPipeline::updatePipeline() {
  generator->seed = this->seed + 10;
  generator->setChanged(true);
}
}  // namespace procrock
