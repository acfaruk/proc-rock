#pragma once
#include "abstracted_pipeline.h"

namespace procrock {
class IgneousPipeline : public AbstractedPipeline {
 private:
  virtual void updatePipeline() override;
};

}  // namespace procrock
