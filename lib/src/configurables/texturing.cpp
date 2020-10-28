#include "configurables/texturing.h"

#include <CImg.h>

namespace procrock {

// Albedo
void GradientAlphaAlbedoGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                                std::function<bool()> activeFunc) {
  coloring.addOwnGroups(config, newGroupName, activeFunc);
}
void GradientAlphaAlbedoGenerator::modify(TextureGroup& textureGroup) {
  textureGroup.albedoData.resize(textureGroup.albedoChannels * textureGroup.width *
                                 textureGroup.height);

  const int channels = textureGroup.albedoChannels;

  for (int i = 0; i < textureGroup.displacementData.size(); i++) {
    auto color = coloring.colorFromValue(textureGroup.displacementData[i]);

    for (int c = 0; c < channels; c++) {
      textureGroup.albedoData[channels * i + c] = color(c);
    }
  }
}

AlbedoAlphaGenerator::AlbedoAlphaGenerator() {
  methods.emplace_back(std::make_unique<GradientAlphaAlbedoGenerator>());
}

void AlbedoAlphaGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                        std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup albedoGroup;
  albedoGroup.entry = {"General", "General Settings for albedo generation.", activeFunc};
  albedoGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the albedo."},
      {{"Gradient Coloring",
        "Create albedo based on a color gradient applied to the displacement."}},
      &choice});

  config.insertToConfigGroups(newGroupName, albedoGroup);

  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void AlbedoAlphaGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

void GradientAlbedoGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                           std::function<bool()> activeFunc) {
  coloring.addOwnGroups(config, newGroupName, activeFunc);
}
void GradientAlbedoGenerator::modify(TextureGroup& textureGroup) {
  textureGroup.albedoData.resize(textureGroup.albedoChannels * textureGroup.width *
                                 textureGroup.height);

  const int channels = textureGroup.albedoChannels;

  for (int i = 0; i < textureGroup.displacementData.size(); i++) {
    auto color = coloring.colorFromValue(textureGroup.displacementData[i]);

    for (int c = 0; c < channels; c++) {
      textureGroup.albedoData[channels * i + c] = color(c);
    }
  }
}

AlbedoGenerator::AlbedoGenerator() {
  methods.emplace_back(std::make_unique<GradientAlbedoGenerator>());
}

void AlbedoGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                   std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup albedoGroup;
  albedoGroup.entry = {"General", "General Settings for albedo generation.", activeFunc};
  albedoGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the albedo."},
      {{"Gradient Coloring",
        "Create albedo based on a color gradient applied to the displacement."}},
      &choice});

  config.insertToConfigGroups(newGroupName, albedoGroup);

  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void AlbedoGenerator::modify(TextureGroup& textureGroup) { methods[choice]->modify(textureGroup); }

// Normals
void GradientNormalsGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                            std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup gradientGroup;
  gradientGroup.entry = {"Gradient Based Normals",
                         "Create normals based on the gradient of the image.", activeFunc};
  gradientGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Normal Strength", "How \"deep / high\" the normals should be."},
      &normalStrength,
      1.0f,
      1.9f});

  gradientGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Source Channel", "Which texture channel should the gradient be based on?"},
      {{"Displacement", "Calculate the gradient on the displacement texture."},
       {"Albedo", "Calculate the gradient on the albedo texture."}},
      &sourceChannel});
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

  CImg<float> image;
  switch (sourceChannel) {
    case 0:
      image = CImg<float>(textureGroup.displacementData.data(), 1, textureGroup.width,
                          textureGroup.height);
      break;
    case 1:
      image = CImg<unsigned char>(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                                  textureGroup.width, textureGroup.height);
      break;
    default:
      assert("Handle all cases!" && 0);
  }
  image.permute_axes("YZCX");
  auto gradients = image.get_gradient("xy", mode - 1);

  float maxValue = std::max(gradients[0].max(), gradients[1].max());
  float minValue = std::min(gradients[0].min(), gradients[1].min());
  float max = std::max(std::abs(minValue), std::abs(maxValue));

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    Eigen::Vector3f normal;
    normal.x() = (gradients[0](i) + max) / (2 * max);
    normal.y() = (gradients[1](i) + max) / (2 * max);
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

void NormalsGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                    std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup normalGroup;
  normalGroup.entry = {"General", "General Settings for normal generation.", activeFunc};
  normalGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the normals."},
      {{"Gradient Based", "Create normals based on the gradient of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, normalGroup);
  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void NormalsGenerator::modify(TextureGroup& textureGroup) { methods[choice]->modify(textureGroup); }

// Roughness
void GreyscaleRoughnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                               std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Roughness",
                          "Create roughness based on the greyscale values of an image.",
                          activeFunc};
  greyscaleGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Source Channel", "Which texture channel should the roughness be based on?"},
      {{"Greyscale Albedo", "Use Greyscale albedo values."},
       {"Displacement", "Use displacement values."}},
      &sourceChannel});
  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 4.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});

  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}

void GreyscaleRoughnessGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.roughnessData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<float> image;
  switch (sourceChannel) {
    case 0:
      image = CImg<unsigned char>(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                                  textureGroup.width, textureGroup.height);
      break;
    case 1:
      image = CImg<float>(textureGroup.displacementData.data(), 1, textureGroup.width,
                          textureGroup.height);
      break;
    default:
      assert("Handle all cases!" && 0);
  }

  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    if (sourceChannel == 0) {
      value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
              0.1140 * (float)image(x, y, 0, 2);
    } else {
      value = image(x, y) * 255;
    }
    value *= scaling;
    value += bias;
    data[i] = std::min(255, std::max(0, value));
  }
}

RoughnessGenerator::RoughnessGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleRoughnessGenerator>());
}

void RoughnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                      std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup roughnessGroup;
  roughnessGroup.entry = {"General", "General Settings for roughness generation.", activeFunc};
  roughnessGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the roughness."},
      {{"Greyscale", "Create the roughness based on the grayscale values of an image / channel."}},
      &choice});

  config.insertToConfigGroups(newGroupName, roughnessGroup);
  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void RoughnessGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

// Metallness
void GreyscaleMetalnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                               std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Metallness",
                          "Create metalness based on the greyscale values of the image.",
                          activeFunc};

  greyscaleGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Source Channel", "Which texture channel should the metalness be based on?"},
      {{"Greyscale Albedo", "Use Greyscale albedo values."},
       {"Displacement", "Use displacement values."}},
      &sourceChannel});
  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 4.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});
  greyscaleGroup.bools.emplace_back(
      Configuration::SimpleEntry<bool>{{"True Metal",
                                        "When this is checked, each pixel of the metalness texture "
                                        "can only be black or white. No inbetween."},
                                       &trueMetal});
  greyscaleGroup.bools.emplace_back(Configuration::SimpleEntry<bool>{
      {"Use Cutoff", "Make everything after a certain value NOT metal."}, &useCutoff});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Cutoff", "After this value everything is NOT metal.", [&]() { return useCutoff; }},
      &cutoffValue,
      0,
      255});
  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}
void GreyscaleMetalnessGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.metalData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<float> image;
  switch (sourceChannel) {
    case 0:
      image = CImg<unsigned char>(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                                  textureGroup.width, textureGroup.height);
      break;
    case 1:
      image = CImg<float>(textureGroup.displacementData.data(), 1, textureGroup.width,
                          textureGroup.height);
      break;
    default:
      assert("Handle all cases!" && 0);
  }
  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    if (sourceChannel == 0) {
      value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
              0.1140 * (float)image(x, y, 0, 2);
    } else {
      value = image(x, y) * 255;
    }

    value *= scaling;
    value += bias;
    value = std::min(255, std::max(0, value));
    if (useCutoff) {
      if (trueMetal) {
        data[i] = cutoffValue > value ? 255 : 0;
      } else {
        data[i] = cutoffValue > value ? value : 0;
      }
    } else {
      if (trueMetal) {
        data[i] = value > 0 ? 255 : 0;
      } else {
        data[i] = value;
      }
    }
  }
}

MetalnessGenerator::MetalnessGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleMetalnessGenerator>());
}

void MetalnessGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                      std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup metalnessGroup;
  metalnessGroup.entry = {"General", "General Settings for metallness generation.", activeFunc};
  metalnessGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the metalness."},
      {{"Greyscale", "Create the metalness based on the grayscale values of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, metalnessGroup);
  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void MetalnessGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

// Ambient Occ.
void GreyscaleAmbientOcclusionGenerator::addOwnGroups(Configuration& config,
                                                      std::string newGroupName,
                                                      std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup greyscaleGroup;
  greyscaleGroup.entry = {"Greyscale Based Ambient Occlusion",
                          "Create ambient occlusion based on the greyscale values of the image.",
                          activeFunc};
  greyscaleGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Source Channel", "Which texture channel should the occlusion be based on?"},
      {{"Displacement", "Use displacement values."},
       {"Greyscale Albedo", "Use Greyscale albedo values."}},
      &sourceChannel});
  greyscaleGroup.floats.emplace_back(Configuration::BoundedEntry<float>{
      {"Scaling", "Scale the greyscale value."}, &scaling, 0.001f, 4.0f});
  greyscaleGroup.ints.emplace_back(Configuration::BoundedEntry<int>{
      {"Bias", "Apply a bias to the greyscale value."}, &bias, -255, 255});
  config.insertToConfigGroups(newGroupName, greyscaleGroup);
}
void GreyscaleAmbientOcclusionGenerator::modify(TextureGroup& textureGroup) {
  using namespace cimg_library;

  auto& data = textureGroup.ambientOccData;

  data.clear();
  data.resize(textureGroup.width * textureGroup.height);

  CImg<float> image;
  switch (sourceChannel) {
    case 0:
      image = CImg<float>(textureGroup.displacementData.data(), 1, textureGroup.width,
                          textureGroup.height);
      break;
    case 1:
      image = CImg<unsigned char>(textureGroup.albedoData.data(), textureGroup.albedoChannels,
                                  textureGroup.width, textureGroup.height);
      break;
    default:
      assert("Handle all cases!" && 0);
  }
  image.permute_axes("YZCX");

  for (int i = 0; i < textureGroup.width * textureGroup.height; i++) {
    int value = 0;
    int y = i / textureGroup.width;
    int x = i % textureGroup.width;
    if (sourceChannel == 1) {
      value = 0.2989 * (float)image(x, y, 0, 0) + 0.5970 * (float)image(x, y, 0, 1) +
              0.1140 * (float)image(x, y, 0, 2);
    } else {
      value = image(x, y) * 255;
    }

    value *= scaling;
    value += bias;
    data[i] = std::min(255, std::max(0, value));
  }
}

AmbientOcclusionGenerator::AmbientOcclusionGenerator() {
  methods.emplace_back(std::make_unique<GreyscaleAmbientOcclusionGenerator>());
}

void AmbientOcclusionGenerator::addOwnGroups(Configuration& config, std::string newGroupName,
                                             std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup ambOccGroup;
  ambOccGroup.entry = {"General", "General Settings for ambient occlusion generation.", activeFunc};
  ambOccGroup.singleChoices.emplace_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose how to generate the ambient occlusion."},
      {{"Greyscale", "Create the metalness based on the grayscale values of the albedo."}},
      &choice});

  config.insertToConfigGroups(newGroupName, ambOccGroup);
  for (int i = 0; i < methods.size(); i++) {
    auto func = [=]() { return activeFunc() && choice == i; };
    methods[i]->addOwnGroups(config, newGroupName, func);
  }
}
void AmbientOcclusionGenerator::modify(TextureGroup& textureGroup) {
  methods[choice]->modify(textureGroup);
}

}  // namespace procrock