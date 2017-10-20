//
//  vkPhysicalDeviceImpl.cpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/20/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#include "vkPhysicalDeviceImpl.hpp"

using namespace vkgl;

vkPhysicalDeviceImpl::vkPhysicalDeviceImpl()
{
    VKGL_LOG("vkPhysicalDeviceImpl::vkPhysicalDeviceImpl\n");
}

vkPhysicalDeviceImpl::~vkPhysicalDeviceImpl()
{
    VKGL_LOG("vkPhysicalDeviceImpl::~vkPhysicalDeviceImpl\n");
}

void vkPhysicalDeviceImpl::GetPhysicalDeviceProperties(VkPhysicalDeviceProperties* pProperties) const
{
    if (!pProperties) {
        return;
    }
    
    pProperties->apiVersion = VK_MAKE_VERSION(1, 0, 0);
    pProperties->driverVersion = 0;
    pProperties->vendorID = 0;
    pProperties->deviceID = 0;
    pProperties->deviceType = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    strcpy(pProperties->deviceName, "vkGPU");
    //pProperties->pipelineCacheUUID // TODO: fill uuid
    //pProperties->limits            // TODO: Fill limits
    //pProperties->sparseProperties  // TODO: Fill sparse props
}

void vkPhysicalDeviceImpl::GetPhysicalDeviceQueueFamilyProperties(uint32_t& count, VkQueueFamilyProperties* pQueueFamilyProperties) const
{
    count = 1; // TODO:
    if (!pQueueFamilyProperties) {
        return;
    }
    
    for (uint32_t i = 0; i < count; ++i) {
        pQueueFamilyProperties[i].queueFlags = VK_QUEUE_GRAPHICS_BIT;
        pQueueFamilyProperties[i].queueCount = 16; // TODO:
        pQueueFamilyProperties[i].timestampValidBits = 64; // TODO
        pQueueFamilyProperties[i].minImageTransferGranularity = {1, 1, 1};
    }
}
