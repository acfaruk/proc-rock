#include "configurables/noise_graph.h"

#include <iostream>
#include <stack>

namespace procrock {

void NoiseGraph::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup group;
  group.entry = {"Noise", "Modify the noise."};
  group.noiseGraphs.push_back(Configuration::SimpleEntry<NoiseGraph>{
      {"Noise Graph", "Modify the graph that creates the final noise."}, this});

  config.insertToConfigGroups(newGroupName + ": Noise", group);
}

int NoiseGraph::addNode(std::unique_ptr<NoiseNode> node, bool rootNode, Eigen::Vector2f position) {
  int result = graph.insert_node(node.get());
  node->id = result;
  node->position = position;
  if (rootNode && graph.get_root_node_id() == -1) {
    graph.set_root_node_id(node->id);
  }

  for (int i = 0; i < node->getModule()->GetSourceModuleCount(); i++) {
    auto inputNode = std::make_unique<ConstNoiseNode>();
    inputNode->placeholder = true;
    inputNode->id = graph.insert_node(inputNode.get());
    graph.insert_edge(node->id, inputNode->id);
    nodes.push_back(std::move(inputNode));
  }
  nodes.push_back(std::move(node));
  return result;
}

noise::module::Module* evaluateGraph(const NoiseGraph& noiseGraph) {
  auto& graph = noiseGraph.graph;
  if (graph.get_root_node_id() == -1) return nullptr;

  std::stack<int> postOrder;
  dfs_traverse(graph, graph.get_root_node_id(),
               [&postOrder](const int nodeId) -> void { postOrder.push(nodeId); });
  std::stack<noise::module::Module*> modulesStack;

  while (!postOrder.empty()) {
    const int id = postOrder.top();
    postOrder.pop();
    NoiseNode* node = graph.node(id);
    auto module = node->getModule();

    if (node->placeholder && graph.num_edges_from_node(id) != 0) continue;
    for (int i = 0; i < module->GetSourceModuleCount(); i++) {
      auto sourceModule = modulesStack.top();
      modulesStack.pop();
      module->SetSourceModule(i, *sourceModule);
    }
    modulesStack.push(module);
  }

  return modulesStack.top();
}

// Output
OutputNoiseNode::OutputNoiseNode() { module = std::make_unique<noise::module::TranslatePoint>(); }

Configuration::ConfigurationGroup OutputNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Output Node", "This will be the final value."};
  return config;
}

noise::module::Module* const OutputNoiseNode::getModule() { return module.get(); }

// Add
AddNoiseNode::AddNoiseNode() { module = std::make_unique<noise::module::Add>(); }

Configuration::ConfigurationGroup AddNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Add", "Adds the values of the two input modules."};
  return config;
}

noise::module::Module* const AddNoiseNode::getModule() { return module.get(); }

// Max
MaxNoiseNode::MaxNoiseNode() { module = std::make_unique<noise::module::Max>(); }

Configuration::ConfigurationGroup MaxNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Max", "Maximum of the two input modules."};
  return config;
}

noise::module::Module* const MaxNoiseNode::getModule() { return module.get(); }

// Min
MinNoiseNode::MinNoiseNode() { module = std::make_unique<noise::module::Min>(); }

Configuration::ConfigurationGroup MinNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Min", "Minimum of the two input modules."};
  return config;
}

noise::module::Module* const MinNoiseNode::getModule() { return module.get(); }

// Multiply
MultiplyNoiseNode::MultiplyNoiseNode() { module = std::make_unique<noise::module::Multiply>(); }

Configuration::ConfigurationGroup MultiplyNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Multiply", "Multiply the values of the two input modules."};
  return config;
}

noise::module::Module* const MultiplyNoiseNode::getModule() { return module.get(); }

// Power
PowerNoiseNode::PowerNoiseNode() { module = std::make_unique<noise::module::Power>(); }

Configuration::ConfigurationGroup PowerNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Power", "Raises one module value to the value of the the second one."};
  return config;
}

noise::module::Module* const PowerNoiseNode::getModule() { return module.get(); }

// Blend
BlendNoiseNode::BlendNoiseNode() { module = std::make_unique<noise::module::Blend>(); }

Configuration::ConfigurationGroup BlendNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Blend", "Blends two modules by the value of the third."};
  return config;
}

noise::module::Module* const BlendNoiseNode::getModule() { return module.get(); }

// Select
SelectNoiseNode::SelectNoiseNode() { module = std::make_unique<noise::module::Select>(); }

Configuration::ConfigurationGroup SelectNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Select", "Select the value between two modules with the value of the third."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lower Bound", "Lower Bound of the selection range."}, &lowerBound, -1.0f, upperBound});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Upper Bound", "Upper Bound of the selection range."}, &upperBound, lowerBound, 1.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Edge Falloff", "Falloff value at the edge transition."}, &edgeFalloff, 0.0f, 1.0f});
  return config;
}

noise::module::Module* const SelectNoiseNode::getModule() {
  module->SetBounds(lowerBound, upperBound);
  module->SetEdgeFalloff(edgeFalloff);
  return module.get();
}

// Abs
AbsNoiseNode::AbsNoiseNode() { module = std::make_unique<noise::module::Abs>(); }

Configuration::ConfigurationGroup AbsNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Absolute Value", "Returns the absolute value of the input module."};
  return config;
}

noise::module::Module* const AbsNoiseNode::getModule() { return module.get(); }

// Clamp
ClampNoiseNode::ClampNoiseNode() { module = std::make_unique<noise::module::Clamp>(); }

Configuration::ConfigurationGroup ClampNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Clamp Value", "Returns a clamped value of the input module."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lower Bound", "Lower Bound of the selection range."}, &lowerBound, -1.0f, upperBound});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Upper Bound", "Upper Bound of the selection range."}, &upperBound, lowerBound, 1.0f});
  return config;
}

noise::module::Module* const ClampNoiseNode::getModule() {
  module->SetBounds(lowerBound, upperBound);
  return module.get();
}

// Exponent
ExponentNoiseNode::ExponentNoiseNode() { module = std::make_unique<noise::module::Exponent>(); }

Configuration::ConfigurationGroup ExponentNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Exponent", "Maps value to an exponential curve."};
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"Exponent", "Exponent to use."}, &exponent, 0.0f, 10.0f});
  return config;
}

noise::module::Module* const ExponentNoiseNode::getModule() {
  module->SetExponent(exponent);
  return module.get();
}

// Invert
InvertNoiseNode::InvertNoiseNode() { module = std::make_unique<noise::module::Invert>(); }

Configuration::ConfigurationGroup InvertNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Invert", "Inverts the value."};
  return config;
}

noise::module::Module* const InvertNoiseNode::getModule() { return module.get(); }

// Scale Bias
ScaleBiasNoiseNode::ScaleBiasNoiseNode() { module = std::make_unique<noise::module::ScaleBias>(); }

Configuration::ConfigurationGroup ScaleBiasNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Scale & Bias", "Scales the output value and applies a bias."};
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"Scale", "Scale to use."}, &scale, 0.0f, 10.0f});
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"Bias", "Bias to use."}, &bias, -1.0f, 1.0f});
  return config;
}

noise::module::Module* const ScaleBiasNoiseNode::getModule() {
  module->SetBias(bias);
  module->SetScale(scale);
  return module.get();
}

// Displace
DisplaceNoiseNode::DisplaceNoiseNode() { module = std::make_unique<noise::module::Displace>(); }

Configuration::ConfigurationGroup DisplaceNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Displace", "Displaces input1 in each dimension by input 2-4"};
  return config;
}

noise::module::Module* const DisplaceNoiseNode::getModule() { return module.get(); }

// Rotate Point
RotatePointNoiseNode::RotatePointNoiseNode() {
  module = std::make_unique<noise::module::RotatePoint>();
}

Configuration::ConfigurationGroup RotatePointNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Rotate Point", "Rotates the coordinates of the input value around the origin."};
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"x Angle", "Angle to rotate."}, &xAngle, 0.0f, 360.0f});
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"y Angle", "Angle to rotate."}, &yAngle, 0.0f, 360.0f});
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"z Angle", "Angle to rotate"}, &zAngle, 0.0f, 360.0f});
  return config;
}

noise::module::Module* const RotatePointNoiseNode::getModule() {
  module->SetAngles(xAngle, yAngle, zAngle);
  return module.get();
}

// Scale Point
ScalePointNoiseNode::ScalePointNoiseNode() {
  module = std::make_unique<noise::module::ScalePoint>();
}

Configuration::ConfigurationGroup ScalePointNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Scale Point", "Scales the coordinates of the input values."};
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"x Scale", "Scaling amount."}, &xScale, -10.0f, 10.0f});
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"y Scale", "Scaling amount."}, &yScale, -10.0f, 10.0f});
  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"z Scale", "Scaling amount."}, &zScale, -10.0f, 10.0f});
  return config;
}

noise::module::Module* const ScalePointNoiseNode::getModule() {
  module->SetScale(xScale, yScale, zScale);
  return module.get();
}

// Translate Point
TranslatePointNoiseNode::TranslatePointNoiseNode() {
  module = std::make_unique<noise::module::TranslatePoint>();
}

Configuration::ConfigurationGroup TranslatePointNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Translate Point", "Translates the coordinates of the input value."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"x Translation", "Translation amount."}, &xTranslation, -10.0f, 10.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"y Translation", "Translation amount."}, &yTranslation, -10.0f, 10.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"z Translation", "Translation amount."}, &zTranslation, -10.0f, 10.0f});
  return config;
}

noise::module::Module* const TranslatePointNoiseNode::getModule() {
  module->SetTranslation(xTranslation, yTranslation, zTranslation);
  return module.get();
}

// Turbulence
TurbulenceNoiseNode::TurbulenceNoiseNode() {
  module = std::make_unique<noise::module::Turbulence>();
}

Configuration::ConfigurationGroup TurbulenceNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Turbulence", "Applies a turbulence to the input coordinates."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "How frequently the displacement changes."}, &frequency, 0.0f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Power", "Scaling factor of displacement."}, &power, 0.0f, 10.0f});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Roughness", "Roughness of changes to displacement."}, &roughness, 0, 20});
  config.ints.push_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for random numbers."}, &seed, 0, 100000});
  return config;
}

noise::module::Module* const TurbulenceNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetPower(power);
  module->SetRoughness(roughness);
  module->SetSeed(seed);
  return module.get();
}

// Const
ConstNoiseNode::ConstNoiseNode() { module = std::make_unique<noise::module::Const>(); }

Configuration::ConfigurationGroup ConstNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Const Value", "Returns the same value everywhere."};

  config.floats.push_back(
      Configuration::BoundedEntry<float>{{"Value", "Value to return."}, &value, -1.0f, 1.0f});
  return config;
}

noise::module::Module* const ConstNoiseNode::getModule() {
  module->SetConstValue(value);
  return module.get();
}

// Perlin
PerlinNoiseNode::PerlinNoiseNode() { module = std::make_unique<noise::module::Perlin>(); }

Configuration::ConfigurationGroup PerlinNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;
  config.entry = {"Perlin Noise", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Perlin Noise"}, &persistence, 0.0f, 1.0f});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the perlin noise function."}, &seed, 1, 100000});

  config.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  return config;
}

// Billow
noise::module::Module* const PerlinNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetLacunarity(lacunarity);
  module->SetPersistence(persistence);
  module->SetOctaveCount(octaveCount);
  module->SetSeed(seed);
  return module.get();
}

BillowNoiseNode::BillowNoiseNode() { module = std::make_unique<noise::module::Billow>(); }

Configuration::ConfigurationGroup BillowNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;

  config.entry = {"Billow Noise", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Persistence", "Roughness of the Billow Noise"}, &persistence, 0.0f, 1.0f});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the billow noise function."}, &seed, 1, 100000});

  config.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  return config;
}

noise::module::Module* const BillowNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetLacunarity(lacunarity);
  module->SetPersistence(persistence);
  module->SetOctaveCount(octaveCount);
  module->SetSeed(seed);
  return module.get();
}

// Ridged
RidgedMultiNoiseNode::RidgedMultiNoiseNode() {
  module = std::make_unique<noise::module::RidgedMulti>();
}

Configuration::ConfigurationGroup RidgedMultiNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;

  config.entry = {"Ridged Multi Noise", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the first octave"}, &frequency, 0.1f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Lacunarity", "Frequency Multiplier between successive octaves."}, &lacunarity, 1.5f, 3.5f});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Octaves", "Number of octaves that generate the noise."}, &octaveCount, 1, 10});
  config.ints.push_back(Configuration::BoundedEntry<int>{
      {"Seed", "Seed for the ridged multi noise function."}, &seed, 1, 100000});

  config.singleChoices.push_back(
      Configuration::SingleChoiceEntry{{"Quality", "Quality of Noise generation"},
                                       {{"Low Quality", "Low Quality Noise"},
                                        {"Standard Quality", "Standard Quality Noise"},
                                        {"High Quality", "High Quality Noise"}},
                                       &qualityChoice});
  return config;
}

noise::module::Module* const RidgedMultiNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetLacunarity(lacunarity);
  module->SetOctaveCount(octaveCount);
  module->SetSeed(seed);
  return module.get();
}

// Voronoi
VoronoiNoiseNode::VoronoiNoiseNode() { module = std::make_unique<noise::module::Voronoi>(); }

Configuration::ConfigurationGroup VoronoiNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;

  config.entry = {"Voronoi Cells", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the seed points."}, &frequency, 0.1f, 100.0f});
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Displacement", "Set frequency of the seed points."}, &displacement, 0.1f, 1.0f});
  config.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Distance", "Change value based on distance to center of cell."}, &enableDistance});
  config.ints.push_back(
      Configuration::BoundedEntry<int>{{"Seed", "Seed for the Vornoi Cells."}, &seed, 1, 100000});
  return config;
}

noise::module::Module* const VoronoiNoiseNode::getModule() {
  module->SetFrequency(frequency);
  module->SetSeed(seed);
  module->EnableDistance(enableDistance);
  module->SetDisplacement(displacement);
  return module.get();
}

// Spheres
SpheresNoiseNode::SpheresNoiseNode() { module = std::make_unique<noise::module::Spheres>(); }

Configuration::ConfigurationGroup SpheresNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;

  config.entry = {"Spheres", "Creates spheres/waves."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the seed points."}, &frequency, 0.1f, 100.0f});
  return config;
}

noise::module::Module* const SpheresNoiseNode::getModule() {
  module->SetFrequency(frequency);
  return module.get();
}

// Cylinders
CylindersNoiseNode::CylindersNoiseNode() { module = std::make_unique<noise::module::Cylinders>(); }

Configuration::ConfigurationGroup CylindersNoiseNode::getConfig() {
  Configuration::ConfigurationGroup config;

  config.entry = {"Cylinders", "Set the various parameters of the noise function(s)."};
  config.floats.push_back(Configuration::BoundedEntry<float>{
      {"Frequency", "Set frequency of the cylinders"}, &frequency, 0.1f, 100.0f});
  return config;
}

noise::module::Module* const CylindersNoiseNode::getModule() {
  module->SetFrequency(frequency);
  return module.get();
}

}  // namespace procrock