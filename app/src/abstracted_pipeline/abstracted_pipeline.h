#pragma once

#include <procrocklib/configurable.h>
#include <procrocklib/pipeline.h>

namespace procrock {
class AbstractedPipeline : public Configurable {
 public:
  virtual ~AbstractedPipeline() = default;

  void connect(Pipeline* pipeline);
  void disconnect();

  bool isConnected() const;
  void update();

 protected:
  virtual void setupPipeline() = 0;
  virtual void updatePipeline() = 0;
  Pipeline* pipeline = nullptr;  // nullptr means we are not connected to a pipeline
};

const unsigned int AbstractedPipeline_Igneous = 0;
const unsigned int AbstractedPipeline_Sedimentary = 1;
const unsigned int AbstractedPipeline_Metamorphic = 2;

const char* const AbstractedPipeline_Names[] = {"Igneous", "Sedimentary", "Metamorphic"};

}  // namespace procrock