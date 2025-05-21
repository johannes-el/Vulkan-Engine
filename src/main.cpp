#include "VulkanEngine.hpp"
#include <iostream>

int main() {
  try {
    VulkanEngine app;
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
