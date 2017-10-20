//
//  vkPhysicalDeviceImpl.hpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/20/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#ifndef vkPhysicalDeviceImpl_hpp
#define vkPhysicalDeviceImpl_hpp

#include "vkGlobal.h"

namespace vkgl {
    
class vkPhysicalDeviceImpl final {
public:
    vkPhysicalDeviceImpl();
    ~vkPhysicalDeviceImpl();
    
    void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties* pProperties) const;
    void GetPhysicalDeviceQueueFamilyProperties(uint32_t& count, VkQueueFamilyProperties* pQueueFamilyProperties) const;
    
private:
};
    
} // namespace vkgl

#endif /* vkPhysicalDeviceImpl_hpp */
