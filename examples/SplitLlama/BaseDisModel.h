#ifndef BASEDISMODEL_H // 作用：防止BaseDisModel.h被重复引用
#define BASEDISMODEL_H
#include <buddy/Core/Container.h>
#include <buddy/LLM/TextContainer.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace buddy;
class BaseDisModel {
public:
  /// Print [Log] label in bold blue format.
  static void printLogLabel() { std::cout << "\033[34;1m[Log] \033[0m"; }

  /// Load parameters into data container.
  static void loadParameters(const std::string &paramFilePath,
                      MemRef<float, 1> &params) {
    const auto loadStart = std::chrono::high_resolution_clock::now();
    std::ifstream paramFile(paramFilePath, std::ios::in | std::ios::binary);
    if (!paramFile.is_open()) {
      std::cout << paramFilePath << std::endl;
      throw std::runtime_error("[Error] Failed to open params file!");
    }
    printLogLabel();
    std::cout << "Loading params..." << std::endl;
    printLogLabel();
    std::cout << "Params file: " << std::filesystem::canonical(paramFilePath)
              << std::endl;
    paramFile.read(reinterpret_cast<char *>(params.getData()),
                   sizeof(float) * (params.getSize()));
    if (paramFile.fail()) {
      throw std::runtime_error("Error occurred while reading params file!");
    }
    paramFile.close();
    const auto loadEnd = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> loadTime =
        loadEnd - loadStart;
    printLogLabel();
    std::cout << "Params load time: " << (double)(loadTime.count()) / 1000
              << "s\n"
              << std::endl;
  }

  static void getParameters(const size_t *paramSize_group, size_t group_len, int size,
                     const std::string &splitNum,
                     std::vector<MemRef<float, 1>> &paramsContainers) {

    std::string llamaBuildDir = LLAMA_EXAMPLE_BUILD_PATH;

    for (size_t i = 0; i < group_len; i++) {
      if (paramSize_group[i] == size) {
        std::string paramsDir = llamaBuildDir + "/subgraph" +
                                std::to_string(i) + "_arg" + splitNum + ".data";
        MemRef<float, 1> paramsContainer({paramSize_group[i]});

        BaseDisModel::loadParameters(paramsDir, paramsContainer);
        paramsContainers.push_back(std::move(paramsContainer));
      }
    }
  }
};

#endif // BASEDISMODEL_H
