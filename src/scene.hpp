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
    
    VkResult Initialize();
    
private:
    VkInstance m_Instance;
    std::vector<VkPhysicalDevice*> m_PhysicalDevices;
};

#endif /* scene_hpp */
