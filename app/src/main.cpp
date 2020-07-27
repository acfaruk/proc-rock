#define PROGRAM_NAME "Proc-Rock"

#include <iostream>
#include <memory>

#include "app.h"

int main(int argc, char *argv[]) {
  if (argv[1] != nullptr) {
    auto rootPath = std::string(argv[1]) + "/app/res";
    auto app = std::make_unique<procRock::App>(glm::uvec2(1280, 720), PROGRAM_NAME, rootPath);

    app->run();
  }
  return 0;
}