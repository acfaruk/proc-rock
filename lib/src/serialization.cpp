#include "serialization.h"

namespace Eigen {
void to_json(nlohmann::json& j, const Eigen::Vector2f& vec) {
  j = nlohmann::json{{"x", vec.x()}, {"y", vec.y()}};
}

void from_json(const nlohmann::json& j, Eigen::Vector2f& vec) {
  j.at("x").get_to(vec.x());
  j.at("y").get_to(vec.y());
}

void to_json(nlohmann::json& j, const Eigen::Vector3f& vec) {
  j = nlohmann::json{{"x", vec.x()}, {"y", vec.y()}, {"z", vec.z()}};
}
void from_json(const nlohmann::json& j, Eigen::Vector3f& vec) {
  j.at("x").get_to(vec.x());
  j.at("y").get_to(vec.y());
  j.at("z").get_to(vec.z());
}

void to_json(nlohmann::json& j, const Eigen::Vector4f& vec) {
  j = nlohmann::json{{"x", vec.x()}, {"y", vec.y()}, {"z", vec.z()}, {"w", vec.w()}};
}

void from_json(const nlohmann::json& j, Eigen::Vector4f& vec) {
  j.at("x").get_to(vec.x());
  j.at("y").get_to(vec.y());
  j.at("z").get_to(vec.z());
  j.at("w").get_to(vec.w());
}
}  // namespace Eigen

namespace procrock {
void to_json(nlohmann::json& j, const ConfigurationList<float>& list) {
  j = nlohmann::json{
      {"minEntries", list.minEntries}, {"maxEntries", list.maxEntries}, {"list", list.list}};
}

void from_json(const nlohmann::json& j, ConfigurationList<float>& list) {
  j.at("minEntries").get_to(list.minEntries);
  j.at("maxEntries").get_to(list.maxEntries);
  j.at("list").get_to(list.list);
}

void to_json(nlohmann::json& j, const ConfigurationCurve& curve) {
  j = nlohmann::json{{"curvePoints", curve.curvePoints}};
}

void from_json(const nlohmann::json& j, ConfigurationCurve& curve) {
  j.at("curvePoints").get_to(curve.curvePoints);
}

void to_json(nlohmann::json& j, const NoiseNode& node) {
  j.update(nlohmann::json{{"internal_id", node.id}});
  j.update(nlohmann::json{{"position", node.position}});
}

void to_json(nlohmann::json& j, const NoiseGraph& graph) {
  nlohmann::json nodesJson;
  for (auto& node : graph.nodes) {
    if (node->placeholder) continue;
    nodesJson.push_back(nlohmann::json{
        {"_id", node->getNodeTypeId()}, {"general", *node}, {"config", node->getConfig()}});
  }

  nlohmann::json edges;
  for (auto& edge : graph.graph.edges()) {
    if (graph.graph.node(edge.to)->placeholder) continue;
    edges.push_back(nlohmann::json{{"from", edge.from}, {"to", edge.to}});
  }

  j = nlohmann::json{{"nodes", nodesJson}, {"edges", edges}};
}

void fillNoiseGraphFromJson(const nlohmann::json& j, NoiseGraph& graph) {
  graph.clear();
  for (auto& nodeJson : j.at("nodes")) {
    int nodeTypeId = nodeJson.at("_id").get<int>();
    int id = nodeJson.at("general").at("internal_id").get<int>();
    Eigen::Vector2f position = nodeJson.at("general").at("position");

    auto node = createNoiseNodeFromTypeId(nodeTypeId);
    auto config = node->getConfig();
    fillConfigGroupFromJson(nodeJson.at("config"), config);
    graph.graph.set_current_id(id);
    graph.addNode(std::move(node), nodeTypeId == NoiseNodeTypeId_Output, position);
  }

  for (auto& edgeJson : j.at("edges")) {
    int from = edgeJson.at("from").get<int>();
    int to = edgeJson.at("to").get<int>();
    graph.graph.insert_edge(from, to);
  }
}

void to_json(nlohmann::json& j, const Configuration::ConfigurationGroup& group) {
  nlohmann::json intsJson;
  for (auto& intEntry : group.ints) {
    intsJson.update(nlohmann::json{{intEntry.entry.name, *intEntry.data}});
  }
  if (group.ints.size() > 0) {
    j.update(nlohmann::json{{"ints", intsJson}});
  }

  nlohmann::json floatsJson;
  for (auto& floatEntry : group.floats) {
    floatsJson.update(nlohmann::json{{floatEntry.entry.name, *floatEntry.data}});
  }
  if (group.floats.size() > 0) {
    j.update(nlohmann::json{{"floats", floatsJson}});
  }

  nlohmann::json float3sJson;
  for (auto& float3Entry : group.float3s) {
    float3sJson.update(nlohmann::json{{float3Entry.entry.name, *float3Entry.data}});
  }
  if (group.float3s.size() > 0) {
    j.update(nlohmann::json{{"float3s", float3sJson}});
  }

  nlohmann::json boolsJson;
  for (auto& boolEntry : group.bools) {
    boolsJson.update(nlohmann::json{{boolEntry.entry.name, *boolEntry.data}});
  }
  if (group.bools.size() > 0) {
    j.update(nlohmann::json{{"bools", boolsJson}});
  }

  nlohmann::json singleChoicesJson;
  for (auto& singleChoiceEntry : group.singleChoices) {
    singleChoicesJson.update(
        nlohmann::json{{singleChoiceEntry.entry.name, *singleChoiceEntry.choice}});
  }
  if (group.singleChoices.size() > 0) {
    j.update(nlohmann::json{{"singleChoices", singleChoicesJson}});
  }

  nlohmann::json gradientColoringsJson;
  for (auto& gradientColEntry : group.gradientColorings) {
    gradientColoringsJson.update(
        nlohmann::json{{gradientColEntry.entry.name, *gradientColEntry.data}});
  }
  if (group.gradientColorings.size() > 0) {
    j.update(nlohmann::json{{"gradientColorings", gradientColoringsJson}});
  }

  nlohmann::json gradientAColoringsJson;
  for (auto& gradientAColEntry : group.gradientAlphaColorings) {
    gradientAColoringsJson.update(
        nlohmann::json{{gradientAColEntry.entry.name, *gradientAColEntry.data}});
  }
  if (group.gradientAlphaColorings.size() > 0) {
    j.update(nlohmann::json{{"gradientAlphaColorings", gradientAColoringsJson}});
  }

  nlohmann::json curvesJson;
  for (auto& curveEntry : group.curves) {
    curvesJson.update(nlohmann::json{{curveEntry.entry.name, *curveEntry.data}});
  }
  if (group.curves.size() > 0) {
    j.update(nlohmann::json{{"curves", curvesJson}});
  }

  nlohmann::json noiseGraphJson;
  for (auto& noiseGraphEntry : group.noiseGraphs) {
    noiseGraphJson.update(nlohmann::json{{noiseGraphEntry.entry.name, *noiseGraphEntry.data}});
  }
  if (group.noiseGraphs.size() > 0) {
    j.update(nlohmann::json{{"noiseGraphs", noiseGraphJson}});
  }
}
void fillConfigGroupFromJson(const nlohmann::json& j, Configuration::ConfigurationGroup& group) {
  auto test = j.find("ints");
  if (j.find("ints") != j.end()) {
    const nlohmann::json& intsJson = j.at("ints");
    for (auto& intEntry : group.ints) {
      *intEntry.data = intsJson.find(intEntry.entry.name).value().get<int>();
    }
  }

  if (j.find("floats") != j.end()) {
    const nlohmann::json& floatsJson = j.at("floats");
    for (auto& floatEntry : group.floats) {
      *floatEntry.data = floatsJson.find(floatEntry.entry.name).value().get<float>();
    }
  }

  if (j.find("float3s") != j.end()) {
    const nlohmann::json& float3sJson = j.at("float3s");
    for (auto& float3Entry : group.float3s) {
      *float3Entry.data = float3sJson.find(float3Entry.entry.name).value().get<Eigen::Vector3f>();
    }
  }

  if (j.find("bools") != j.end()) {
    const nlohmann::json& boolsJson = j.at("bools");
    for (auto& boolEntry : group.bools) {
      *boolEntry.data = boolsJson.find(boolEntry.entry.name).value().get<bool>();
    }
  }

  if (j.find("singleChoices") != j.end()) {
    const nlohmann::json& singleChoicesJson = j.at("singleChoices");
    for (auto& singleChoiceEntry : group.singleChoices) {
      *singleChoiceEntry.choice =
          singleChoicesJson.find(singleChoiceEntry.entry.name).value().get<int>();
    }
  }

  if (j.find("gradientColorings") != j.end()) {
    const nlohmann::json& gradientColoringsJson = j.at("gradientColorings");
    for (auto& gradientColEntry : group.gradientColorings) {
      *gradientColEntry.data = gradientColoringsJson.find(gradientColEntry.entry.name)
                                   .value()
                                   .get<std::map<int, Eigen::Vector3f>>();
    }
  }

  if (j.find("gradientAlphaColorings") != j.end()) {
    const nlohmann::json& gradientAColoringsJson = j.at("gradientAlphaColorings");
    for (auto& gradientAColEntry : group.gradientAlphaColorings) {
      *gradientAColEntry.data = gradientAColoringsJson.find(gradientAColEntry.entry.name)
                                    .value()
                                    .get<std::map<int, Eigen::Vector4f>>();
    }
  }

  if (j.find("curves") != j.end()) {
    const nlohmann::json& curvesJson = j.at("curves");
    for (auto& curveEntry : group.curves) {
      *curveEntry.data = curvesJson.find(curveEntry.entry.name).value().get<ConfigurationCurve>();
    }
  }

  if (j.find("noiseGraphs") != j.end()) {
    const nlohmann::json& noiseGraphJson = j.at("noiseGraphs");
    for (auto& noiseGraphEntry : group.noiseGraphs) {
      auto current = noiseGraphJson.find(noiseGraphEntry.entry.name).value();
      fillNoiseGraphFromJson(current, *noiseGraphEntry.data);
    }
  }
}

void to_json(nlohmann::json& j, const Configuration& conf) {
  for (auto& group : conf.getConfigGroupsConst()) {
    j.update(nlohmann::json{{group.first, group.second}});
  }
}

void fillConfigFromJson(const nlohmann::json& j, Configuration conf) {
  for (auto& group : conf.getConfigGroups()) {
    const auto& json = j.find(group.first).value();

    int i = 0;
    for (auto& entry : json) {
      fillConfigGroupFromJson(entry, group.second[i]);
      i++;
    }
  }
}
}  // namespace procrock