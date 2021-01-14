#include "form_generator.h"

#include "../../math.h"

namespace procrock {
void FormGeneratorExtender::addOwnGroups(Configuration& config, std::string newGroupName,
                                         std::function<bool()> activeFunc) {
  Configuration::ConfigurationGroup group;
  group.entry = {"General", "General Settings to change the form of the rock."};
  group.singleChoices.push_back(Configuration::SingleChoiceEntry{
      {"Method", "Choose the method to create a base form."},
      {{"Pebble", "Create a pebble like rock."},
       {"Liquid", "Create a rock heavely looking like a soft liquid."},
       {"Cube", "Simple cube to look at the textures."},
       {"Boulder", "Create a boulder with interesting edges."}},
      &formChoice});

  group.ints.push_back(
      Configuration::BoundedEntry<int>{{"Seed", "Change the form"}, &seed, 0, 1000000});
  group.bools.push_back(Configuration::SimpleEntry<bool>{
      {"Cut Ground", "Should the ground be cut from the rock?", [&]() { return formChoice <= 1; }},
      &cutGround});
  group.floats.push_back(Configuration::BoundedEntry<float>{
      {"Geometry Amount", "Complexity of geometry"}, &geometryDetail, 0.1, 1});
  group.floats.push_back(Configuration::BoundedEntry<float>{
      {"Roughness", "Roughness of rock surface"}, &roughness, 0, 1});
  group.floats.push_back(Configuration::BoundedEntry<float>{
      {"Distortion", "Distortion of rock surface (linked to roughness)"}, &distortion, 0, 1});

  config.insertToConfigGroups(newGroupName, group);
}

void FormGeneratorExtender::setupPipeline(Pipeline* pipeline) {
  auto mod = std::make_unique<DecimateModifier>();
  this->modSkinDecimate = mod.get();
  pipeline->addModifier(std::move(mod));

  auto mod0 = std::make_unique<CutPlaneModifier>();
  this->modPyramidCut = mod0.get();
  pipeline->addModifier(std::move(mod0));

  auto mod1 = std::make_unique<SubdivisionModifier>();
  this->modSubdiv = mod1.get();
  pipeline->addModifier(std::move(mod1));

  auto mod2 = std::make_unique<CutPlaneModifier>();
  this->modGroundCut = mod2.get();
  pipeline->addModifier(std::move(mod2));

  auto mod3 = std::make_unique<DisplaceAlongNormalsModifier>();
  this->modRoughness = mod3.get();
  pipeline->addModifier(std::move(mod3));

  auto mod4 = std::make_unique<TransformationModifier>();
  this->modTransform = mod4.get();
  pipeline->addModifier(std::move(mod4));

  auto mod5 = std::make_unique<DecimateModifier>();
  this->modDecimate = mod5.get();
  pipeline->addModifier(std::move(mod5));
}
void FormGeneratorExtender::updatePipeline(Pipeline* pipeline) {
  switch (formChoice) {
    case 0: {
      auto gen = std::make_unique<IcosahedronGenerator>();
      genPebble = gen.get();
      pipeline->setGenerator(std::move(gen));
      modSubdiv->subdivisions = 3;
      modSubdiv->mode = 0;
    } break;
    case 1: {
      auto gen = std::make_unique<SkinSurfaceGenerator>();
      genLiquid = gen.get();
      pipeline->setGenerator(std::move(gen));
      modSubdiv->subdivisions = 0;
      modSubdiv->mode = 0;
      genLiquid->seed = seed + 10;
      genLiquid->pointAmount = 100;
      genLiquid->pointSize = 0.08;
      genLiquid->shrinkFactor = 0.4;
    } break;
    case 2: {
      auto gen = std::make_unique<CuboidGenerator>();
      genCube = gen.get();
      pipeline->setGenerator(std::move(gen));
      modSubdiv->subdivisions = 3;
      modSubdiv->mode = 1;

      cutGround = false;
    } break;
    case 3: {
      auto gen = std::make_unique<PyramidGenerator>();
      genBoulder = gen.get();
      pipeline->setGenerator(std::move(gen));
      modSubdiv->subdivisions = 2;
      modSubdiv->mode = 0;

      cutGround = false;
    } break;
    deafult:
      assert(0 && "Handle all choices!");
      break;
  }
  modPyramidCut->setDisabled(formChoice != 3);
  std::mt19937 rng;
  rng.seed(seed + 10);

  float rotY = (rng() / (float)rng.max()) * (M_PI / 2);
  float rotZ = (rng() / (float)rng.max()) * (M_PI / 2);

  modPyramidCut->rotation.y() = rotY;
  modPyramidCut->rotation.z() = rotZ;

  // Skin Decimate
  modSkinDecimate->setDisabled(formChoice != 1);
  modSkinDecimate->relativeValue = 0.3;

  // Cut Ground
  modGroundCut->rotation = Eigen::Vector3f(0, 0, (6.0 / 4.0) * M_PI);
  modGroundCut->setDisabled(!cutGround);
  if (cutGround) {
    modTransform->translation = Eigen::Vector3f(0, -0.5, 0);
  } else {
    modTransform->translation = Eigen::Vector3f(0, 0, 0);
  }

  // Geometry Detail
  modDecimate->relativeValue = geometryDetail;

  updateGeometryRoughness();
}
void FormGeneratorExtender::updateGeometryRoughness() {
  // Texture: Noise
  auto& noise = modRoughness->noiseGraph;
  noise.clear();
  auto billowNoiseNode = std::make_unique<BillowNoiseNode>();
  auto billowNoiseNodePtr = billowNoiseNode.get();
  auto billowNoiseNodeId = noise.addNode(std::move(billowNoiseNode), false, {0, 0});

  billowNoiseNodePtr->frequency = math::lerp(0.3, 10, roughness);
  billowNoiseNodePtr->seed = seed + 10;

  int output = noise.addNode(createNoiseNodeFromTypeId(NoiseNodeTypeId_Output), true, {400, 0});
  noise.addEdge(billowNoiseNodeId, output);

  modRoughness->selection = 1;
  modRoughness->factor = math::lerp(0, 0.5, distortion);
}
}  // namespace procrock