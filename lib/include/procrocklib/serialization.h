#pragma once

#include <configurable.h>
#include <generator.h>
#include <procrocklib/configurables/noise_graph.h>

#include <nlohmann/json.hpp>

namespace Eigen {
void to_json(nlohmann::json& j, const Eigen::Vector2f& vec);
void from_json(const nlohmann::json& j, Eigen::Vector2f& vec);

void to_json(nlohmann::json& j, const Eigen::Vector3f& vec);
void from_json(const nlohmann::json& j, Eigen::Vector3f& vec);

void to_json(nlohmann::json& j, const Eigen::Vector4f& vec);
void from_json(const nlohmann::json& j, Eigen::Vector4f& vec);
}  // namespace Eigen

namespace procrock {
void to_json(nlohmann::json& j, const ConfigurationList<float>& list);
void from_json(const nlohmann::json& j, ConfigurationList<float>& list);

void to_json(nlohmann::json& j, const ConfigurationCurve& curve);
void from_json(const nlohmann::json& j, ConfigurationCurve& curve);

void to_json(nlohmann::json& j, const NoiseNode& node);
void to_json(nlohmann::json& j, const NoiseGraph& graph);
void fillNoiseGraphFromJson(const nlohmann::json& j, NoiseGraph& graph);

void to_json(nlohmann::json& j, const Configuration::ConfigurationGroup& group);
void fillConfigGroupFromJson(const nlohmann::json& j, Configuration::ConfigurationGroup& group);

void to_json(nlohmann::json& j, const Configuration& conf);
void fillConfigFromJson(const nlohmann::json& j, Configuration conf);

}  // namespace procrock