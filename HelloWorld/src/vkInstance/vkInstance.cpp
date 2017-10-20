//
//  vkInstance.cpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/10/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#include "vkGlobal.h"
#include "vkInstanceImpl.hpp"

using namespace vkgl;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                const VkAllocationCallbacks* pAllocator,
                                                VkInstance* pInstance)
{
    if (!pCreateInfo ||
        pCreateInfo->sType != VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO ||
        pCreateInfo->pNext != NULL ||
        pCreateInfo->flags != 0) {
        VKGL_LOG("Cannot create VkInstance with incorrect parameters of VkInstanceCreateInfo");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    const VkApplicationInfo *pApplicationInfo = pCreateInfo->pApplicationInfo;
    if (pApplicationInfo &&
        (pApplicationInfo->sType != VK_STRUCTURE_TYPE_APPLICATION_INFO ||
         pApplicationInfo->pNext != NULL)) {
        VKGL_LOG("Cannot create VkInstance with incorrect parameters of VkApplicationInfo");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (pCreateInfo->enabledLayerCount > 0) {
        VKGL_LOG("Cannot create VkInstance, layers don't supported yet");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }
    
    if (pCreateInfo->enabledExtensionCount > 0) {
        VKGL_LOG("Cannot create VkInstance, extensions don't supported yet");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    
    *pInstance = (VkInstance)new vkInstanceImpl();
    
    return VK_SUCCESS;
}

void vkDestroyInstance(VkInstance instance,
                       const VkAllocationCallbacks* pAllocator)
{
    if (!instance) {
        VKGL_LOG("Cannot destroy VkInstance with NULL object");
        return;
    }
    
    delete (vkInstanceImpl *)instance;
}
