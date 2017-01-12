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

typedef struct {
    VkImage image;
    VkImageView view;
    uint32_t width;
    uint32_t height;
} SwapchainBuffers;

class Scene {
public:
    Scene();
    ~Scene();

    VkResult Initialize(void* view);
    std::tuple<VkQueueFamilyProperties, uint32_t> FindQueueProperties(VkQueueFlags flag,
        const std::vector<VkBool32>& supportsPresent);
    std::tuple<VkCommandPool, std::vector<VkCommandBuffer> > CreateCommandPool(const VkDevice& logicalDevice,
        uint32_t graphicQueueIndex, size_t count);
    std::tuple<VkSwapchainKHR, std::vector<SwapchainBuffers> > CreateBuffers(const VkPhysicalDevice& physicalDevice,
        const VkDevice& logicalDevice,
        const VkSurfaceKHR& surface,
        const VkSurfaceFormatKHR& format);
    std::vector<VkFramebuffer> CreateFrameBuffers(const VkDevice& logicalDevice,
        const std::vector<SwapchainBuffers>& swapchainBuffers,
        const VkRenderPass& renderPass);
    VkRenderPass CreateRenderPass(const VkDevice& logicalDevice, const VkSurfaceFormatKHR& surfaceFormat);
    
    void Update();

private:
    VkInstance m_Instance;
    std::vector<VkPhysicalDevice> m_PhysicalDevices;
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties m_MemoryProperties;

    std::vector<VkQueueFamilyProperties> m_QueueProperties;
    VkDevice m_LogicalDevice;

    VkSurfaceKHR m_Surface;
    std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;

    VkSwapchainKHR m_Swapchain;
    std::vector<SwapchainBuffers> m_SwapchainBuffers;

    VkRenderPass m_RenderPass;
    std::vector<VkFramebuffer> m_FrameBuffers;
    
    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
    
    VkQueue m_Queue;
    VkFence m_Fence;
    
public:
    bool m_IsInitialized;
};

#endif /* scene_hpp */

