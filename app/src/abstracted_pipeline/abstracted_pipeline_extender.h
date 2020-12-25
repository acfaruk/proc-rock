#pragma once
#include <procrocklib/configurables/configurable_extender.h>
#include <procrocklib/pipeline.h>

namespace procrock {
class AbstractedPipelineExtender : public ConfigurableExtender {
 public:
  AbstractedPipelineExtender(){};

  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) = 0;

  virtual void setupPipeline(Pipeline* pipeline) = 0;
  virtual void updatePipeline(Pipeline* pipeline) = 0;
};
}  // namespace procrock