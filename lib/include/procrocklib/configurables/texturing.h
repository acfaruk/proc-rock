#pragma once

#include <procrocklib/configurables/coloring.h>
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

// Albedo
class GradientAlphaAlbedoGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

  GradientAlphaColoring coloring;
};

class AlbedoAlphaGenerator : public TextureGroupModifier {
 public:
  AlbedoAlphaGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;
};

class GradientAlbedoGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

  GradientColoring coloring;
};

class AlbedoGenerator : public TextureGroupModifier {
 public:
  AlbedoGenerator();
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

 private:
  std::vector<std::unique_ptr<TextureGroupModifier>> methods;
  int choice = 0;
};

// Normals
class GradientNormalsGenerator : public TextureGroupModifier {
 public:
  virtual void addOwnGroups(
      Configuration& config, std::string newGroupName,
      std::function<bool()> activeFunc = []() { return true; }) override;
  virtual void modify(TextureGroup& textureGroup) override;

  float normalStrength = 1.0f;
  // 0 = Backward finite differences, 1 = Centered finite differences, 2 = Forward finite
  // differences, 3 = Using Sobel kernels, 4 = Using rotation invariant kernels, 5 = Using
  // Deriche recursive filter, 6 = Using Van Vliet recursive filter.
  int mode = 3;
  int sourceChannel = 0;  // 0 = displacement, 1 = albedo
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

  float scaling = 2.0f;
  int bias = 0;

  int sourceChannel = 0;  // 0 = greyscale albedo, 1 = displacement
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

  float scaling = 0.2f;
  int bias = 0;

  bool trueMetal = false;
  bool useCutoff = false;
  int cutoffValue = 10;

  int sourceChannel = 0;  // 0 = greyscale albedo, 1 = displacement
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

  float scaling = 0.5f;
  int bias = 0;

  int sourceChannel = 0;  // 0 = displacement, 1 = greyscale
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