#include "app.h"

#include <procrocklib/gen/icosahedron_generator.h>
#include <procrocklib/mod/subdivision_modifier.h>
#include <procrocklib/par/xatlas_parameterizer.h>
#include <procrocklib/texgen/noise_texture_generator.h>

#include <iostream>

#include "gui/gui.h"
#include "io/input_manager.h"

namespace procrock {
App* App::current = nullptr;
App::App(glm::uvec2 windowSize, std::string title, std::string resPath, bool resizable)
    : resourcesPath(resPath) {
#ifndef NDEBUG
  this->debug = true;
#endif
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "GLFW Initialization failed..." << std::endl;
  }
  current = this;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
  if (debug) {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  }
  window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, title.c_str(), NULL, NULL);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  glfwSetWindowSizeCallback(window, onCurrentWindowResize);
  glfwSetMouseButtonCallback(window, InputManager::onMouseButton);
  glfwSetCursorPosCallback(window, InputManager::onMousePos);
  glfwSetScrollCallback(window, InputManager::onMouseScroll);
  glfwSetWindowSizeLimits(window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

  glfwMakeContextCurrent(window);

  // Initialize GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "GLEW Initialization failed..." << std::endl;
  }

  glfwSwapInterval(1);

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Resource Path is " << resourcesPath << std::endl;

  if (debug) {
    if (GLEW_KHR_debug) {
      std::cout << "KHR debugging enabled." << std::endl;
      glDebugMessageCallback(onOpenGLDebugMessage, this);
    } else if (GLEW_ARB_debug_output) {
      std::cout << "ARB debugging enabled." << std::endl;
      glDebugMessageCallbackARB(onOpenGLDebugMessage, this);
    }
  }
}

App::~App() {
  if (window) {
    glfwDestroyWindow(window);
    window = 0;
  }
  glfwTerminate();
}

void App::run() {
  double t;
  double dt;
  double previousTime = 0;

  init();
  while (!glfwWindowShouldClose(window)) {
    t = glfwGetTime();
    dt = t - previousTime;
    if (dt > 0.005) {
      glfwPollEvents();
      previousTime = t;

      if (!this->update()) return;
      if (!this->render()) return;
      glfwSwapBuffers(window);
    }
  }
  end();
}

void App::changeLightingMode(const LightingMode mode) {
  currentLightningMode = mode;
  auto& guiLights = gui::windows.viewSettingsWindow.lights;

  pointLights.clear();
  guiLights.clear();

  switch (mode) {
    case LightingMode::WellLit:
      pointLights.push_back(PointLight(glm::vec3(7, 2, 0), 300));
      pointLights.push_back(PointLight(glm::vec3(-3, 4, 0), 50));
      pointLights.push_back(PointLight(glm::vec3(0, 4, -3), 50));
      pointLights.push_back(PointLight(glm::vec3(5, 0, 0), 50));
      pointLights.push_back(PointLight(glm::vec3(-5, 0, 0), 50));
      pointLights.push_back(PointLight(glm::vec3(0, 5, 0), 50));
      pointLights.push_back(PointLight(glm::vec3(0, -5, 0), 50));
      pointLights.push_back(PointLight(glm::vec3(0, 0, 5), 50));
      pointLights.push_back(PointLight(glm::vec3(0, 0, -5), 50));
      break;
    case LightingMode::Default:
      pointLights.push_back(PointLight(glm::vec3(7, 4, 0), 200));
      pointLights.push_back(PointLight(glm::vec3(-7, 4, 3), 200));
      pointLights.push_back(PointLight(glm::vec3(-7, 4, -3), 200));
      pointLights.push_back(PointLight(glm::vec3(-3, 4, 0), 10));
      pointLights.push_back(PointLight(glm::vec3(0, 4, -3), 10));
      pointLights.push_back(PointLight(glm::vec3(5, 0, 0), 10));
      pointLights.push_back(PointLight(glm::vec3(-5, 0, 0), 10));
      pointLights.push_back(PointLight(glm::vec3(0, 5, 0), 10));
      pointLights.push_back(PointLight(glm::vec3(0, -5, 0), 10));
      pointLights.push_back(PointLight(glm::vec3(0, 0, 5), 10));
      pointLights.push_back(PointLight(glm::vec3(0, 0, -5), 10));
      break;
    case LightingMode::Single:
      pointLights.push_back(PointLight(glm::vec3(7, 7, 0), 800));
      break;
    default:
      assert("Handle all cases!" && 0);
      break;
  }
  guiLights.resize(pointLights.size());
  for (int i = 0; i < pointLights.size(); i++) {
    guiLights[i].intensity = pointLights[i].intensity;
  }
}

bool App::init() {
  gui::init(window, this->resourcesPath);

  mainCam = std::make_unique<Camera>(getFrameBufferSize());
  mainCam->lookAt(glm::vec3(3, 3, 3), glm::vec3(0), glm::vec3(0, 1, 0));

  pipeline = std::make_unique<Pipeline>();
  pipeline->setGenerator(std::make_unique<IcosahedronGenerator>());
  pipeline->addModifier(std::make_unique<SubdivisionModifier>());
  pipeline->setParameterizer(std::make_unique<XAtlasParameterizer>());
  pipeline->setTextureGenerator(std::make_unique<NoiseTextureGenerator>());

  mainShader = std::make_unique<Shader>(resourcesPath + "/shaders/main.vert",
                                        resourcesPath + "/shaders/main.frag");

  groundPlane = std::make_unique<DrawableGround>();

  changeLightingMode(currentLightningMode);

  viewerFramebuffer = std::make_unique<Framebuffer>(glm::uvec2(200, 200));

  rockTexGroup["albedo"] = std::make_unique<RenderTexture>();
  rockTexGroup["normalMap"] = std::make_unique<RenderTexture>();
  rockTexGroup["roughnessMap"] = std::make_unique<RenderTexture>();
  rockTexGroup["metalMap"] = std::make_unique<RenderTexture>();
  rockTexGroup["ambientOccMap"] = std::make_unique<RenderTexture>();
  rockTexGroup["displacementMap"] = std::make_unique<RenderTexture>();

  groundTexGroups[0]["albedo"] = std::make_unique<RenderTexture>();
  groundTexGroups[0]["albedo"]->loadFromFile(resourcesPath + "/textures/gravel/albedo.jpg");
  groundTexGroups[0]["normalMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[0]["normalMap"]->loadFromFile(resourcesPath + "/textures/gravel/normals.jpg");
  groundTexGroups[0]["roughnessMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[0]["roughnessMap"]->loadFromFile(resourcesPath +
                                                   "/textures/gravel/roughness.jpg");
  groundTexGroups[0]["ambientOccMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[0]["ambientOccMap"]->loadFromFile(resourcesPath +
                                                    "/textures/gravel/ambientOcc.jpg");
  groundTexGroups[0]["displacementMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[0]["displacementMap"]->loadFromFile(resourcesPath +
                                                      "/textures/gravel/displacement.jpg");
  groundTexGroups[0]["metalMap"] = std::make_unique<RenderTexture>();

  groundTexGroups[1]["albedo"] = std::make_unique<RenderTexture>();
  groundTexGroups[1]["albedo"]->loadFromFile(resourcesPath + "/textures/mossy/albedo.jpg");
  groundTexGroups[1]["normalMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[1]["normalMap"]->loadFromFile(resourcesPath + "/textures/mossy/normals.jpg");
  groundTexGroups[1]["roughnessMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[1]["roughnessMap"]->loadFromFile(resourcesPath + "/textures/mossy/roughness.jpg");
  groundTexGroups[1]["ambientOccMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[1]["ambientOccMap"]->loadFromFile(resourcesPath +
                                                    "/textures/mossy/ambientOcc.jpg");
  groundTexGroups[1]["displacementMap"] = std::make_unique<RenderTexture>();
  groundTexGroups[1]["displacementMap"]->loadFromFile(resourcesPath +
                                                      "/textures/mossy/displacement.jpg");

  groundTexGroups[1]["metalMap"] = std::make_unique<RenderTexture>();

  InputManager::registerInputReceiver(mainCam.get());

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  return true;
}

bool App::update() {
  if (pipeline->isChanged()) {
    auto mesh = pipeline->getCurrentMesh();
    drawableMesh = std::make_unique<DrawableMesh>(*mesh);

    rockTexGroup["albedo"]->loadFromData(mesh->textures.albedoData.data(), mesh->textures.width,
                                         mesh->textures.height);

    rockTexGroup["normalMap"]->loadFromData(mesh->textures.normalData.data(), mesh->textures.width,
                                            mesh->textures.height);

    rockTexGroup["roughnessMap"]->loadFromData(mesh->textures.roughnessData.data(),
                                               mesh->textures.width, mesh->textures.height, 1);

    rockTexGroup["metalMap"]->loadFromData(mesh->textures.metalData.data(), mesh->textures.width,
                                           mesh->textures.height, 1);

    rockTexGroup["ambientOccMap"]->loadFromData(mesh->textures.ambientOccData.data(),
                                                mesh->textures.width, mesh->textures.height, 1);

    rockTexGroup["displacementMap"]->loadFromData(mesh->textures.displacementData.data(),
                                                  mesh->textures.width, mesh->textures.height, 1);

    gui::windows.meshInfoWindow.vertices = mesh->vertices.rows();
    gui::windows.meshInfoWindow.faces = mesh->faces.rows();
    gui::windows.meshInfoWindow.textureWidth = mesh->textures.width;
    gui::windows.meshInfoWindow.textureHeight = mesh->textures.height;
  }

  gui::update(this->getWindowSize(), *viewerFramebuffer, *pipeline, *mainShader);

  mainCam->setViewport(gui::viewer.size);
  mainShader->uniforms3f["camPos"] = mainCam->getPosition();

  for (int i = 0; i < pointLights.size(); i++) {
    const auto& light = gui::windows.viewSettingsWindow.lights[i];
    pointLights[i].setEulerAngles(light.yaw, light.pitch);
    pointLights[i].color = light.color;
    pointLights[i].intensity = light.intensity;

    mainShader->uniforms3f["lightPos[" + std::to_string(i) + "]"] = pointLights[i].position;
    mainShader->uniforms3f["lightColors[" + std::to_string(i) + "]"] = pointLights[i].color;
    mainShader->uniformsf["lightIntensities[" + std::to_string(i) + "]"] = pointLights[i].intensity;
  }

  if (static_cast<int>(currentLightningMode) != gui::windows.viewSettingsWindow.lightChoice) {
    changeLightingMode(static_cast<LightingMode>(gui::windows.viewSettingsWindow.lightChoice));
  }

  mainShader->uniforms3f["ambientColor"] = gui::windows.viewSettingsWindow.ambientColor;
  mainShader->uniformsi["lightCount"] = pointLights.size();

  groundPlane->setPosition(glm::vec3{0, gui::windows.viewSettingsWindow.groundPlane.height, 0});

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  if (gui::windows.viewSettingsWindow.wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  return true;
}

bool App::render() {
  auto cColor = gui::windows.viewSettingsWindow.clearColor;
  glClearColor(cColor.r, cColor.g, cColor.b, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  viewerFramebuffer->bind();
  drawableMesh->draw(*mainCam, *mainShader);
  if (gui::windows.viewSettingsWindow.groundPlane.show) {
    for (auto& pair : groundTexGroups[gui::windows.viewSettingsWindow.groundPlane.textureChoice]) {
      mainShader->textures[pair.first] = pair.second.get();
    }
    groundPlane->draw(*mainCam, *mainShader);
  }
  for (auto& pair : rockTexGroup) {
    mainShader->textures[pair.first] = pair.second.get();
  }
  bindDefaultFrameBuffer(getFrameBufferSize());
  gui::render();
  return true;
}

bool App::end() { return true; }

glm::uvec2 App::getFrameBufferSize() const {
  glm::ivec2 sizes(0);
  glfwGetFramebufferSize(window, &sizes.x, &sizes.y);
  return glm::uvec2(sizes);
}

glm::uvec2 App::getWindowSize() const {
  glm::ivec2 sizes(0);
  glfwGetWindowSize(window, &sizes.x, &sizes.y);
  return glm::uvec2(sizes);
}

void App::onCurrentWindowResize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  current->update();
  current->render();
  glfwSwapBuffers(window);
}
void App::onOpenGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity,
                               GLsizei length, const GLchar* message, const void* userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "Source: API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "Source: Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "Source: Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "Source: Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "Source: Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "Source: Other";
      break;
  }
  std::cout << std::endl;

  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "Type: Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "Type: Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "Type: Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "Type: Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "Type: Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "Type: Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "Type: Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "Type: Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "Type: Other";
      break;
  }
  std::cout << std::endl;

  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "Severity: high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "Severity: medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "Severity: low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "Severity: notification";
      break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
}  // namespace procrock
