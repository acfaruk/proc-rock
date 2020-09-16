#pragma once
#include <procrocklib/configurable.h>

namespace procrock {
class ConfigurableExtender {
 public:
  virtual void addOwnGroups(Configuration& config, std::string newGroupName) = 0;
};

class GroupConfigurable {
 public:
  virtual Configuration::ConfigurationGroup getConfig() = 0;
};
}  // namespace procrock