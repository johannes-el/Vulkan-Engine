#include <vulkan/VulkanEngine.hpp>
#include <iostream>

auto main() -> int
{
	try {
		VulkanEngine app;
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
