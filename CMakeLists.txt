cmake_minimum_required(VERSION 4.0)
project(VulkanProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(include)

add_executable(VulkanApp
    src/main.cpp
    src/VulkanEngine.cpp
    src/Logger.cpp
    src/FileIO.cpp
    src/VulkanInstance.cpp
    src/VulkanSwapchain.cpp
)

add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found!")
endif()

find_package(glfw3 REQUIRED)

target_link_libraries(VulkanApp PRIVATE Vulkan::Vulkan glfw imgui)

target_include_directories(VulkanApp
    PRIVATE include
    PRIVATE external/VulkanMemoryAllocator/include
    PRIVATE external/imgui
    PRIVATE external/imgui/backends
)

add_subdirectory(external/fastgltf)
target_link_libraries(VulkanApp PRIVATE fastgltf)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
