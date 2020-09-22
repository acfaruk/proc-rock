#include "configurables/texturing.h"

#include <CImg.h>

namespace procrock {

// Normals

void GradientNormalsGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup gradientGroup;
  gradientGroup.entry = {"Gradient Based Normals",
                         "Create normals based on the gradient of the image."};
  gradientGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normal Strength", "How \"deep / high\" the normals should be."},
      &normalStrength,
      1.0f,
      1.9f});

  gradientGroup.singleChoices.emplace_back(
      Configuration::SingleChoiceEntry{{"Mode", "Which gradient method to use."},
                                       {{"BFD", "Backward Finite Differences"},
                                        {"CFD", "Cenetered Finite Differences"},
                                        {"FFD", "Forward Finite Differences"},
                                        {"Sobel", "Using Sobel Kernels"},
                                        {"RotInv.", "Using Rotation Invariant Kernels"},
                                        {"DRF", "Deriche Recursive Filter"},
                                        {"VVRF", "Van Vliet Recursive Filter"}},
                                       &mode});
  config.insertToConfigGroups(newGroupName, gradientGroup);
}
void GradientNormalsGenerator::modify(TextureGroup& textureGroup) {
  assert(textureGroup.albedoChannels == 3 || textureGroup.albedoChannels == 4);

  using namespace cimg_library;
  auto& data = textureGroup.normalData;
  data.clear();
  data.resize(textureGroup.width * textureGroup.height * 3);

  CImg<unsigned char> image(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                            textureGroup.width, textureGroup.height);
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", mode - 1);
  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    Eigen::Vector3f normal;
    normal.x() = (gradients[0](i) + 1024) / 2048.0;
    normal.y() = (gradients[1](i) + 1024) / 2048.0;
    normal.z() = 1 / normalStrength;
    normal = 255 * normal;

    data[(3 * i)] = (int)normal.x();
    data[(3 * i) + 1] = (int)normal.y();
    data[(3 * i) + 2] = (int)normal.z();
  }
}

NormalsGenerator::NormalsGenerator() {
  methods.emplace_back(std::make_unique<GradientNormalsGenerator>());
}

void NormalsGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup normalGroup;
  normalGroup.entry = {"General", "General Settings for normal generation."};
  normalGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the normals."},
      {{"Gradient Based", "Create normals based on the gradient of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, normalGroup);
  methods[choice]->addOwnGroups(config, newGroupName);
}
void NormalsGenerator::modify(TextureGroup& textureGroup) { methods[choice]->modify(textureGroup); }

// Roughness
void GreyscaleRoughnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Roughness",
                          "Create roughness based on the greyscale values of the image."};

  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 10.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});

  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}

void GreyscaleRoughnessGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.roughnessData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<unsigned char> image(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                            textureGroup.width, textureGroup.height);
  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
            0.1140 * (float)image(x, y, 0, 2);

    value *= scaling;
    value += bias;

    data[i] = std::min(255, std::max(0, value));
  }
}

RoughnessGenerator::RoughnessGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleRoughnessGenerator>());
}

void RoughnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup roughnessGroup;
  roughnessGroup.entry = {"General", "General Settings for roughness generation."};
  roughnessGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the roughness."},
      {{"Greyscale", "Create the roughness based on the grayscale values of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, roughnessGroup);
  methods[choice]->addOwnGroups(config, newGroupName);
}
void RoughnessGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

// Metallness
void GreyscaleMetalnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Metallness",
                          "Create metalness based on the greyscale values of the image."};

  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 10.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Cutoff", "After this value everything is NOT metal."}, &cutoffValue, 0, 255});
  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}
void GreyscaleMetalnessGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.metalData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<unsigned char> image(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                            textureGroup.width, textureGroup.height);
  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
            0.1140 * (float)image(x, y, 0, 2);

    value *= scaling;
    value += bias;
    value = std::min(255, std::max(0, value));
    data[i] = cutoffValue > value ? 255 : 0;
  }
}

MetalnessGenerator::MetalnessGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleMetalnessGenerator>());
}

void MetalnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup metalnessGroup;
  metalnessGroup.entry = {"General", "General Settings for metallness generation."};
  metalnessGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the metalness."},
      {{"Greyscale", "Create the metalness based on the grayscale values of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, metalnessGroup);
  methods[choice]->addOwnGroups(config, newGroupName);
}
void MetalnessGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

// Ambient Occ.
void GreyscaleAmbientOcclusionGenerator::addOwnGroups(Configuration& config,
                                                      std::string newGroupName) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Ambient Occlusion",
                          "Create ambient occlusion based on the greyscale values of the image."};

  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 10.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});
  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}
void GreyscaleAmbientOcclusionGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.ambientOccData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<unsigned char> image(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                            textureGroup.width, textureGroup.height);
  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
            0.1140 * (float)image(x, y, 0, 2);

    value *= scaling;
    value += bias;
    data[i] = std::min(255, std::max(0, value));
  }
}

AmbientOcclusionGenerator::AmbientOcclusionGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleAmbientOcclusionGenerator>());
}

void AmbientOcclusionGenerator::addOwnGroups(Configuration& config, std::string newGroupName) {
  Configuration::ConfigurationGroup ambOccGroup;
  ambOccGroup.entry = {"General", "General Settings for ambient occlusion generation."};
  ambOccGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the ambient occlusion."},
      {{"Greyscale", "Create the metalness based on the grayscale values of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, ambOccGroup);
  methods[choice]->addOwnGroups(config, newGroupName);
}
void AmbientOcclusionGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}
}  // namespace procrock