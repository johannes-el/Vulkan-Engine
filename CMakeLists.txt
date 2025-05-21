cmake_minimum_required(VERSION 3.5)
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
)

find_package(Vulkan REQUIRED)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found!")
endif()

find_package(glfw3 REQUIRED)

target_link_libraries(VulkanApp PRIVATE Vulkan::Vulkan glfw)

target_include_directories(VulkanApp
    PRIVATE include
    PRIVATE external/VulkanMemoryAllocator/include
)

add_subdirectory(external/fastgltf)
target_link_libraries(VulkanApp PRIVATE fastgltf)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
