#include "vulkan/VulkanEngine.hpp"
#include "vulkan/VulkanImGui.hpp"

void initImgui(VulkanEngine* engine)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(engine->_window, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = engine->_vk.instance;
	init_info.PhysicalDevice = engine->_vk.physicalDevice;
	init_info.Device = engine->_vk.device;
	init_info.Queue = engine->_vk.graphicsQueue;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;

	ImGui_ImplVulkan_Init(&init_info);
}
