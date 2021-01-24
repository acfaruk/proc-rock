#include <procrocklib/pipeline.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>

char* getCmdOption(char** begin, char** end, const std::string& option) {
  char** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
  // -r = run a parameter file and export in place
  char* parameterFile = getCmdOption(argv, argv + argc, "-r");
  if (parameterFile) {
    procrock::Pipeline pipeline;
    pipeline.enableOutput(true);
    pipeline.loadFromFile(parameterFile);
    pipeline.getCurrentMesh();

    procrock::Pipeline::ExportSettings settings;
    pipeline.exportCurrent("output.obj", settings);
  }

  // -b = benchmark a folder, run all files in folder three times and get avg times
  // also varies texture sizes for more timings
  char* benchmarkFolder = getCmdOption(argv, argv + argc, "-b");
  if (benchmarkFolder) {
    procrock::Pipeline pipeline;
    pipeline.enableOutput(false);

    for (const auto& entry : std::filesystem::directory_iterator(benchmarkFolder)) {
      std::string parameterFile = entry.path().generic_string();

      std::cout << "Benchmarking file: " << parameterFile << std::endl;

      pipeline.loadFromFile(parameterFile);

      // iterate through texture sizes
      std::array<long long, 5> currentTimes;
      for (int i = 0; i < 5; i++) {
        pipeline.getParameterizer().textureSizeChoice = i;

        long long durationSum = 0;
        for (int j = 0; j < 3; j++) {
          pipeline.getGenerator().setChanged(true);
          auto start = std::chrono::high_resolution_clock::now();
          pipeline.getCurrentMesh();
          auto end = std::chrono::high_resolution_clock::now();

          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

          durationSum += duration.count();
        }

        int res = std::pow(2, i + 7);
        std::cout << res << "x" << res << ": " << durationSum / 3 << "ms";
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
  }

  return 0;
}