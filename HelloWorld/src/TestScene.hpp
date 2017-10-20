//
//  TestScene.hpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/10/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#ifndef TestScene_hpp
#define TestScene_hpp

#include "vkGlobal.h"

class Scene
{
public:
    Scene();
    ~Scene();
    
    std::tuple<VkResult, VkInstance> CreateInstance(const std::string& appName, const std::string& engineName);
    std::tuple<VkResult, std::vector<VkPhysicalDevice> > GetPhysicalDevices(const VkInstance& instance);
    std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueProps(const VkInstance& instance,
                                                                     const VkPhysicalDevice physicalDevice);
    
private:
    VkInstance m_Instance;
    std::vector<VkPhysicalDevice> m_PhysicalDevices;
    
    std::vector<VkQueueFamilyProperties> m_QueueProperties;
    VkDevice m_LogicalDevice;
    
    VkSurfaceKHR m_Surface;
    std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
    
    //VkSwapchainKHR m_Swapchain;
    //std::vector<SwapchainBuffers> m_SwapchainBuffers;
    
    VkRenderPass m_RenderPass;
    std::vector<VkFramebuffer> m_FrameBuffers;
    
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    
    VkQueue m_Queue;
    VkFence m_Fence;
};

#endif /* TestScene_hpp */
