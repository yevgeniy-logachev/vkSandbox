//
//  scene.cpp
//  vkSandbox
//
//  Created by Yevgeniy Logachev on 11/14/16.
//  Copyright © 2016 Yevgeniy Logachev. All rights reserved.
//

#include "scene.hpp"
#include <MoltenVK/vk_mvk_ios_surface.h>

Scene::Scene()
{

}

Scene::~Scene()
{

}

VkResult Scene::Initialize()
{
    const VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "vkSandbox",
        .applicationVersion = 0,
        .pEngineName = "vkSandbox",
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_0,
    };
    VkInstanceCreateInfo inst_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .pApplicationInfo = &app,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = 0,
    };
    
    VkResult result = vkCreateInstance(&inst_info, NULL, &m_Instance);
    if (result == VK_SUCCESS) {
        uint32_t physicalDeviceCount = 0;
        result = vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
        if (result)
        {
            m_PhysicalDevices.resize(physicalDeviceCount);
            vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, m_PhysicalDevices[0]);
        }
        
        printf("GPU count: %d\n", physicalDeviceCount);
    }
    
    return result;
}
