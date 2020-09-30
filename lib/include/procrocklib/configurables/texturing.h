#pragma once

#include <procrocklib/configurables/configurable_extender.h>
#include <procrocklib/mesh.h>

namespace procrock {

class TextureGroupModifier : public ConfigurableExtender {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) = 0;
  virtual void modify(TextureGroup& textureGroup) = 0;
};

// Normals
class GradientNormalsGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  float normalStrength = 1.45f;

  // 0 = Backward finite differences, 1 = Centered finite differences, 2 = Forward finite
  // differences, 3 = Using Sobel kernels, 4 = Using rotation invariant kernels, 5 = Using
  // Deriche recursive filter, 6 = Using Van Vliet recursive filter.
  int mode = 3;
};

class NormalsGenerator : public TextureGroupModifier {
 public:
  NormalsGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;  // 0 == gradients
};

// Roughness
class GreyscaleRoughnessGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  float scaling = 2.0f;
  int bias = 0;
};

class RoughnessGenerator : public TextureGroupModifier {
 public:
  RoughnessGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;  // 0 = grayscale
};

// Metallness
class GreyscaleMetalnessGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  float scaling = 1.0f;
  int bias = 0;
  int cutoffValue = 10;
};

class MetalnessGenerator : public TextureGroupModifier {
 public:
  MetalnessGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;  // 0 = greyscale
};

// Ambient Occ
class GreyscaleAmbientOcclusionGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  float scaling = 0.5f;
  int bias = 0;
};

class AmbientOcclusionGenerator : public TextureGroupModifier {
 public:
  AmbientOcclusionGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;  // 0 = greyscale
};

}  // namespace procrock