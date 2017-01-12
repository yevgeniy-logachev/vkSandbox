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
: m_IsInitialized(false)
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
    float queuePriorities[1] = { 0.0 };
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

    uint32_t formatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevices[0], m_Surface, &formatCount, nullptr);
    assert(result == VK_SUCCESS);
    m_SurfaceFormats.resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevices[0], m_Surface, &formatCount, &m_SurfaceFormats[0]);
    assert(result == VK_SUCCESS);

    assert(formatCount >= 1);
    if (formatCount == 1 && m_SurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        m_SurfaceFormats[0].format = VK_FORMAT_B8G8R8A8_UNORM;
    }

    //
    VkSurfaceFormatKHR currentSurfaceFormat = m_SurfaceFormats[0];

    std::tuple<VkSwapchainKHR, std::vector<SwapchainBuffers> > buffers = CreateBuffers(m_PhysicalDevices[0],
        m_LogicalDevice, m_Surface, currentSurfaceFormat);
    m_Swapchain = std::get<0>(buffers);
    m_SwapchainBuffers = std::get<1>(buffers);

    //
    m_RenderPass = CreateRenderPass(m_LogicalDevice, currentSurfaceFormat);
    m_FrameBuffers = CreateFrameBuffers(m_LogicalDevice, m_SwapchainBuffers, m_RenderPass);
    
    //
    std::vector<VkBool32> supportsPresent(m_QueueProperties.size());
    for (int i = 0; i < m_QueueProperties.size(); i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevices[0], i, m_Surface, &supportsPresent[i]);
    }
    
    std::tuple<VkQueueFamilyProperties, uint32_t> graphicQueueProps = FindQueueProperties(VK_QUEUE_GRAPHICS_BIT,
                                                                                          supportsPresent);
    vkGetDeviceQueue(m_LogicalDevice, std::get<1>(graphicQueueProps), 0, &m_Queue);
    std::tuple<VkCommandPool, std::vector<VkCommandBuffer> > cmdPool = CreateCommandPool(m_LogicalDevice,
                                                                           std::get<1>(graphicQueueProps),
                                                                           m_SwapchainBuffers.size());
    m_CommandPool = std::get<0>(cmdPool);
    m_CommandBuffers = std::get<1>(cmdPool);
    
    //
    for (uint32_t i = 0; i < m_SwapchainBuffers.size(); ++i) {
        
        VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .pNext = nullptr,
            .renderPass = m_RenderPass,
            .subpass = 0,
            .framebuffer = m_FrameBuffers[i],
            .occlusionQueryEnable = VK_FALSE,
            .queryFlags = 0,
            .pipelineStatistics = 0
        };
        
        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr//&commandBufferInheritanceInfo
        };
        
        vkBeginCommandBuffer(m_CommandBuffers[i], &commandBufferBeginInfo);
        {
            VkClearValue clearValue = {
                .color.float32[0] = 1.0f,
                .color.float32[1] = 0.0f,
                .color.float32[2] = 0.0f,
                .color.float32[3] = 1.0f
            };
            
            VkRenderPassBeginInfo renderPassBeginInfo = {
                .sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,
                .renderPass = m_RenderPass,
                .framebuffer = m_FrameBuffers[i],
                .renderArea = VkRect2D{
                    VkOffset2D{0, 0},
                    VkExtent2D{
                        m_SwapchainBuffers[i].width,
                        m_SwapchainBuffers[i].height
                    }
                },
                .clearValueCount = 1,
                .pClearValues = &clearValue
            };
            
            vkCmdBeginRenderPass(m_CommandBuffers[i],
                                 &renderPassBeginInfo,
                                 VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
            //vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pip);
            vkCmdEndRenderPass(m_CommandBuffers[i]);
            
            VkImageMemoryBarrier imageMemoryBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = m_SwapchainBuffers[i].image,
                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
            };
            
            vkCmdPipelineBarrier(m_CommandBuffers[i],
                                 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &imageMemoryBarrier);
        }
        vkEndCommandBuffer(m_CommandBuffers[i]);
    }
    
    
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };
    
    result = vkCreateFence(m_LogicalDevice, &fenceCreateInfo, nullptr, &m_Fence);
    assert(result == VK_SUCCESS);
    
    m_IsInitialized = (result == VK_SUCCESS);
    
    return result;
}

std::tuple<VkCommandPool, std::vector<VkCommandBuffer> > Scene::CreateCommandPool(const VkDevice& logicalDevice,
    uint32_t graphicQueueIndex, size_t count)
{
    const VkCommandPoolCreateInfo cmdPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .queueFamilyIndex = graphicQueueIndex,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };

    VkCommandPool commandPool;
    VkResult result = vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &commandPool);
    assert(result == VK_SUCCESS);
    
    result = vkResetCommandPool(logicalDevice, commandPool,
                                VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    assert(result == VK_SUCCESS);

    const VkCommandBufferAllocateInfo cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)count,
    };

    std::vector<VkCommandBuffer> commandBuffers(count);
    result = vkAllocateCommandBuffers(logicalDevice, &cmd, &commandBuffers[0]);
    assert(result == VK_SUCCESS);

    std::tuple<VkCommandPool, std::vector<VkCommandBuffer> > cmdResult(commandPool, commandBuffers);
    return std::move(cmdResult);
}

std::tuple<VkSwapchainKHR, std::vector<SwapchainBuffers> > Scene::CreateBuffers(const VkPhysicalDevice& physicalDevice,
    const VkDevice& logicalDevice,
    const VkSurfaceKHR& surface,
    const VkSurfaceFormatKHR& surfaceFormat)
{
    VkSurfaceCapabilitiesKHR surfCapabilities;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCapabilities);
    assert(result == VK_SUCCESS);

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    assert(result == VK_SUCCESS);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    assert(presentModeCount > 0);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, &presentModes[0]);
    assert(result == VK_SUCCESS);

    VkExtent2D swapchainExtent;
    // width and height are either both -1, or both not -1.
    if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = 1136; // TODO
        swapchainExtent.height = 640;
    } else {
        swapchainExtent = surfCapabilities.currentExtent;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use in the swap chain (we desire to
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32_t desiredNumberOfSwapchainImages = surfCapabilities.minImageCount + 1;
    if ((surfCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount)) {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapchainImages = surfCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    const VkSwapchainCreateInfoKHR swapchainInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .surface = surface,
        .minImageCount = desiredNumberOfSwapchainImages,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = {
            .width = swapchainExtent.width, .height = swapchainExtent.height,
        },
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = preTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .imageArrayLayers = 1,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .presentMode = swapchainPresentMode,
        .oldSwapchain = VK_NULL_HANDLE,
        .clipped = true,
    };

    VkSwapchainKHR swapchain;
    result = vkCreateSwapchainKHR(logicalDevice, &swapchainInfo, nullptr, &swapchain);
    assert(result == VK_SUCCESS);

    uint32_t swapchainImageCount;
    result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, nullptr);
    assert(result == VK_SUCCESS);

    std::vector<VkImage> swapchainImages(swapchainImageCount);
    assert(swapchainImageCount > 0);
    result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &swapchainImageCount, &swapchainImages[0]);
    assert(result == VK_SUCCESS);

    std::vector<SwapchainBuffers> buffers(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .format = surfaceFormat.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_R,
                .g = VK_COMPONENT_SWIZZLE_G,
                .b = VK_COMPONENT_SWIZZLE_B,
                .a = VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 },
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .flags = 0,
        };

        buffers[i].image = swapchainImages[i];
        buffers[i].width = swapchainExtent.width;
        buffers[i].height = swapchainExtent.height;
        color_image_view.image = swapchainImages[i];

        result = vkCreateImageView(logicalDevice, &color_image_view, nullptr, &buffers[i].view);
        assert(result == VK_SUCCESS);
    }

    std::tuple<VkSwapchainKHR, std::vector<SwapchainBuffers> > resultBuffers(swapchain, buffers);
    return std::move(resultBuffers);
}

VkRenderPass Scene::CreateRenderPass(const VkDevice& logicalDevice, const VkSurfaceFormatKHR& surfaceFormat)
{
    const VkAttachmentDescription attachments[1] = {
            [0] = {
                .flags = 0,
                .format = surfaceFormat.format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }
    };
    const VkAttachmentReference colorReference = {
        .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    const VkAttachmentReference depthReference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    const VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .flags = 0,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };
    const VkRenderPassCreateInfo renderpassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };

    VkRenderPass renderPass;
    VkResult result = vkCreateRenderPass(logicalDevice, &renderpassInfo, NULL, &renderPass);
    assert(result == VK_SUCCESS);

    return std::move(renderPass);
}

std::vector<VkFramebuffer> Scene::CreateFrameBuffers(const VkDevice& logicalDevice,
    const std::vector<SwapchainBuffers>& swapchainBuffers,
    const VkRenderPass& renderPass)
{
    std::vector<VkFramebuffer> framebuffers(swapchainBuffers.size());
    for (uint32_t i = 0; i < swapchainBuffers.size(); ++i) {
        const SwapchainBuffers& swapchainBuffer = swapchainBuffers[i];

        const VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = &swapchainBuffer.view,
            .width = swapchainBuffer.width,
            .height = swapchainBuffer.height,
            .layers = 1,
        };

        VkResult result = vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]);
        assert(result == VK_SUCCESS);
    }

    return std::move(framebuffers);
}

std::tuple<VkQueueFamilyProperties, uint32_t> Scene::FindQueueProperties(VkQueueFlags flag,
    const std::vector<VkBool32>& supportsPresent)
{
    std::tuple<VkQueueFamilyProperties, uint32_t> queueProps;
    int index = 0;
    std::all_of(m_QueueProperties.begin(), m_QueueProperties.end(), [&](VkQueueFamilyProperties props) {
        if (index < supportsPresent.size() && !supportsPresent[index]) {
            ++index;
            return false;
        }

        if (props.queueFlags & flag) {
            std::get<0>(queueProps) = props;
            std::get<1>(queueProps) = index;
            return true;
        }
        ++index;

        return false;
    });
    return std::move(queueProps);
}

void Scene::Update()
{
    VkResult result;
    uint32_t swapchainImage;
    {
        result = vkAcquireNextImageKHR(m_LogicalDevice, m_Swapchain, UINT64_MAX, VK_NULL_HANDLE, m_Fence, &swapchainImage);
        assert(result == VK_SUCCESS);
    }
    
    result = vkWaitForFences(m_LogicalDevice, 1, &m_Fence, VK_TRUE, UINT64_MAX);
    assert(result == VK_SUCCESS);
    result = vkResetFences(m_LogicalDevice, 1, &m_Fence);
    assert(result == VK_SUCCESS);
    
    const VkCommandBuffer& commandBuffer = m_CommandBuffers[swapchainImage];
    
    VkPipelineStageFlags waitMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = &waitMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };
    
    result = vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE);
    assert(result == VK_SUCCESS);
    
    result = vkQueueWaitIdle(m_Queue);
    assert(result == VK_SUCCESS);
    
    VkPresentInfoKHR presentInfoKHR = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .swapchainCount = 1,
        .pSwapchains = &m_Swapchain,
        .pImageIndices = &swapchainImage,
        .pResults = &result
    };
    assert(result == VK_SUCCESS);
    
    result = vkQueuePresentKHR(m_Queue, &presentInfoKHR);
    assert(result == VK_SUCCESS);
}
