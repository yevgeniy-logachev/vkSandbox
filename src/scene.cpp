//
//  scene.cpp
//  vkSandbox
//
//  Created by Yevgeniy Logachev on 11/14/16.
//  Copyright © 2016 Yevgeniy Logachev. All rights reserved.
//

#include "scene.hpp"
#include <assert.h>
#include <MoltenVK/vk_mvk_ios_surface.h>

Scene::Scene()
{

}

Scene::~Scene()
{

}

VkResult Scene::Initialize(void* view)
{
    const VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "vkSandbox",
        .applicationVersion = 0,
        .pEngineName = "vkSandbox",
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_0,
    };
    VkInstanceCreateInfo inst_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &app,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = 0,
    };
    
    // Instance and GPUs
    VkResult result = vkCreateInstance(&inst_info, nullptr, &m_Instance);
    assert(result == VK_SUCCESS);
    
    uint32_t physicalDeviceCount = 0;
    result = vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
    assert(result == VK_SUCCESS);
    m_PhysicalDevices.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, m_PhysicalDevices.data());
    
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevices[0], &m_MemoryProperties);
    vkGetPhysicalDeviceProperties(m_PhysicalDevices[0], &m_PhysicalDeviceProperties);
    
    // Queue properties
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevices[0], &queueCount, nullptr);
    assert(queueCount > 0);
    
    m_QueueProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevices[0], &queueCount, m_QueueProperties.data());
    
    // Logical device
    float queuePriorities[1] = {0.0};
    VkDeviceQueueCreateInfo queueInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = 0,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities,
    };
    
    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr
    };
    
    result = vkCreateDevice(m_PhysicalDevices[0], &deviceInfo, nullptr, &m_LogicalDevice);
    assert(result == VK_SUCCESS);
    
    std::tuple<VkQueueFamilyProperties, uint32_t> graphicQueueProps = FindQueueProperties(VK_QUEUE_GRAPHICS_BIT);
    VkCommandPoolCreateInfo cmdPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = std::get<1>(graphicQueueProps)
    };

    result = vkCreateCommandPool(m_LogicalDevice, &cmdPoolInfo, NULL, &m_CommandPool);
    assert(result == VK_SUCCESS);
    
    /* Create the command buffer from the command pool */
    VkCommandBufferAllocateInfo cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_CommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    
    result = vkAllocateCommandBuffers(m_LogicalDevice, &cmd, &m_CommandBuffer);
    assert(result == VK_SUCCESS);
    
#if defined(VK_USE_PLATFORM_IOS_MVK)
    VkIOSSurfaceCreateInfoMVK createInfo = {
        .sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK,
        .pNext = nullptr,
        .flags = 0,
        .pView = view
    };
    result = vkCreateIOSSurfaceMVK(m_Instance, &createInfo, nullptr, &m_Surface);
    assert(result == VK_SUCCESS);
#endif
    
    
    
    return result;
}

std::tuple<VkQueueFamilyProperties, uint32_t> Scene::FindQueueProperties(VkQueueFlags flag)
{
    std::tuple<VkQueueFamilyProperties, uint32_t> queueProps;
    int index = 0;
    std::all_of(m_QueueProperties.begin(), m_QueueProperties.end(), [&](VkQueueFamilyProperties props)
    {
        if (props.queueFlags & flag) {
            std::get<0>(queueProps) = props;
            std::get<1>(queueProps) = index;
            return true;
        }
        return false;
    });
    return std::move(queueProps);
}
