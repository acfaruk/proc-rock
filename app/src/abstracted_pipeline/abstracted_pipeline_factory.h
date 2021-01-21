#pragma once
#include <memory>

#include "abstracted_pipeline.h"
#include "all.h"

namespace procrock {
std::unique_ptr<AbstractedPipeline> inline createAbstractPipelineFromId(unsigned int id) {
  switch (id) {
    case AbstractedPipeline_Igneous:
      return std::make_unique<IgneousPipeline>();
    case AbstractedPipeline_Sedimentary:
      return std::make_unique<SedimentaryPipeline>();
    case AbstractedPipeline_Metamorphic:
      return std::make_unique<MetamorphicPipeline>();
    default:
      assert(0 && "make sure all abstract pipeline types are handled!");
  }

  return std::make_unique<IgneousPipeline>();
}
}  // namespace procrock