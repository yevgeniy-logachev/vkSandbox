//
//  scene.hpp
//  vkSandbox
//
//  Created by Yevgeniy Logachev on 11/14/16.
//  Copyright © 2016 Yevgeniy Logachev. All rights reserved.
//

#ifndef scene_hpp
#define scene_hpp

#include <vector>
#include <vulkan/vulkan.h>


class Scene {
public:
    Scene();
    ~Scene();
    
    VkResult Initialize(void* view);
    std::tuple<VkQueueFamilyProperties, uint32_t> FindQueueProperties(VkQueueFlags flag);
    
private:
    VkInstance m_Instance;
    std::vector<VkPhysicalDevice> m_PhysicalDevices;
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties m_MemoryProperties;
    
    std::vector<VkQueueFamilyProperties> m_QueueProperties;
    VkDevice m_LogicalDevice;
    
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_CommandBuffer;
    
    VkSurfaceKHR m_Surface;
};

#endif /* scene_hpp */
