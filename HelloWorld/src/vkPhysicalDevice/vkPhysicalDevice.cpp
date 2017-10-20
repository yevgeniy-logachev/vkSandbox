//
//  vkPhysicalDevice.cpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/20/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#include "vkGlobal.h"
#include "vkPhysicalDeviceImpl.hpp"

using namespace vkgl;

static uint32_t kMaxGPUCount = 1;

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance,
                                    uint32_t* pPhysicalDeviceCount,
                                    VkPhysicalDevice* pPhysicalDevices)
{
    if (!instance) {
        VKGL_LOG("Cannot enumerate physical devices, no valid instance provided");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (!pPhysicalDevices && !pPhysicalDeviceCount) {
        VKGL_LOG("Cannot enumerate physical devices, invalid parameters provided");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (!pPhysicalDevices) {
        *pPhysicalDeviceCount = kMaxGPUCount;
        return VK_SUCCESS;
    }
    
    uint32_t maxGPU = std::min(*pPhysicalDeviceCount, kMaxGPUCount);
    for (uint32_t i = 0; i < maxGPU; ++i) {
        pPhysicalDevices[i] = (VkPhysicalDevice)new vkPhysicalDeviceImpl();
    }
    
    return (maxGPU == *pPhysicalDeviceCount ? VK_SUCCESS : VK_INCOMPLETE);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                         VkPhysicalDeviceProperties* pProperties)
{
    if (!physicalDevice) {
        return;
    }
    
    ((vkPhysicalDeviceImpl *)physicalDevice)->GetPhysicalDeviceProperties(pProperties);
}

VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                                                    uint32_t* pQueueFamilyPropertyCount,
                                                                    VkQueueFamilyProperties* pQueueFamilyProperties)
{
    if (!physicalDevice ||
        !pQueueFamilyPropertyCount) {
        if (pQueueFamilyPropertyCount) {
            *pQueueFamilyPropertyCount = 0;
        }
        return;
    }
    
    ((vkPhysicalDeviceImpl *)physicalDevice)->GetPhysicalDeviceQueueFamilyProperties(*pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
