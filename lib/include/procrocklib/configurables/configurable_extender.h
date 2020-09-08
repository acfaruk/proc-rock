#pragma once
#include <procrocklib/configurable.h>

namespace procrock {
class ConfigurableExtender {
  virtual void addOwnGroups(Configuration& config, std::string& groupBaseName) = 0;
};
}  // namespace procrock