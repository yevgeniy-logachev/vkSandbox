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

    bool Initialize(void* view);
    bool IsInitialized() const;

    std::tuple<VkResult, VkInstance> CreateInstance(const std::string& appName, const std::string& engineName);
    std::tuple<VkResult, std::vector<VkPhysicalDevice> > GetPhysicalDevices(const VkInstance& instance);
    std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueProps(const VkInstance& instance,
        const VkPhysicalDevice physicalDevice);
    std::tuple<VkResult, VkSurfaceKHR, std::vector<VkSurfaceFormatKHR> > CreateSurface(const VkInstance& instance,
        const VkPhysicalDevice& physicalDevice,
        void* view);
    std::tuple<VkResult, VkDevice> CreateLogicalDevice(const VkPhysicalDevice& physicalDevice /* debug layer */);
    std::tuple<VkResult, VkCommandPool, std::vector<VkCommandBuffer> > CreateCommandPool(const VkDevice& logicalDevice,
        uint32_t graphicQueueIndex, size_t count);
    std::tuple<VkResult, VkSwapchainKHR, std::vector<SwapchainBuffers> > CreateBuffers(const VkPhysicalDevice& physicalDevice,
        const VkDevice& logicalDevice,
        const VkSurfaceKHR& surface,
        const VkSurfaceFormatKHR& format);
    std::tuple<VkResult, VkRenderPass> CreateRenderPass(const VkDevice& logicalDevice,
        const VkSurfaceFormatKHR& surfaceFormat);
    std::tuple<VkResult, std::vector<VkFramebuffer> > CreateFrameBuffers(const VkDevice& logicalDevice,
        const std::vector<SwapchainBuffers>& swapchainBuffers,
        const VkRenderPass& renderPass);
    std::tuple<bool, VkQueueFamilyProperties, uint32_t> FindQueueProperties(VkQueueFlags flag,
        const std::vector<VkBool32>& supportsPresent);

    void Update();

private:
    VkInstance m_Instance;
    std::vector<VkPhysicalDevice> m_PhysicalDevices;

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

