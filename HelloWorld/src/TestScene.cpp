//
//  TestScene.cpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/10/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#include "TestScene.hpp"


Scene::Scene()
{
    // Instance
    {
        std::tuple<VkResult, VkInstance> instanceData = CreateInstance("Hello World", "VkEngine");
        VkResult result = std::get<0>(instanceData);
        if (result != VK_SUCCESS) {
            printf("Can't create Vulkan instance\n");
            return;
        }
        m_Instance = std::get<1>(instanceData);
    }
    
    // GPU
    {
        std::tuple<VkResult, std::vector<VkPhysicalDevice> > physicalDeviceData = GetPhysicalDevices(m_Instance);
        VkResult result = std::get<0>(physicalDeviceData);
        if (result != VK_SUCCESS) {
            printf("Can't retrieve Vulkan compatible GPU list\n");
            return;
        }
        m_PhysicalDevices = std::get<1>(physicalDeviceData);
    }
    
    // GPU Props
    VkPhysicalDeviceProperties pProperties;
    {
        
        vkGetPhysicalDeviceProperties(m_PhysicalDevices[0], &pProperties);
    }
    
    // GPU Queue Props
    {
        m_QueueProperties = GetPhysicalDeviceQueueProps(m_Instance, m_PhysicalDevices[0]);
    }
}

std::tuple<VkResult, VkInstance> Scene::CreateInstance(const std::string& appName, const std::string& engineName)
{
    const VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = appName.c_str(),
        .applicationVersion = 0,
        .pEngineName = engineName.c_str(),
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
    
    VkInstance instance;
    VkResult result = vkCreateInstance(&inst_info, nullptr, &instance);
    std::tuple<VkResult, VkInstance> instanceData(result, instance);
    return instanceData;
}

std::tuple<VkResult, std::vector<VkPhysicalDevice> > Scene::GetPhysicalDevices(const VkInstance& instance)
{
    uint32_t physicalDeviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);
    
    std::tuple<VkResult, std::vector<VkPhysicalDevice> > physycalDeviceData(result, physicalDevices);
    return physycalDeviceData;
}

std::vector<VkQueueFamilyProperties> Scene::GetPhysicalDeviceQueueProps(const VkInstance& instance,
                                                                        const VkPhysicalDevice physicalDevice)
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, &queueProperties[0]);
    
    return queueProperties;
}

Scene::~Scene()
{
    
}
