#pragma once

#include <procrocklib/configurable.h>
#include <procrocklib/pipeline.h>

namespace procrock {
class AbstractedPipeline : public Configurable {
 public:
  void connect(Pipeline* pipeline);
  void disconnect();

  bool isConnected() const;
  void update();

 protected:
  virtual void updatePipeline() = 0;
  Pipeline* pipeline;  // Pipeline to be modified, if nullptr we are not connected to a pipeline
};

const unsigned int AbstractedPipeline_Igneous = 0;

const char* const AbstractedPipeline_Names[] = {"Igneous"};

}  // namespace procrock