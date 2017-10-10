//
//  TestScene.hpp
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/10/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#ifndef TestScene_hpp
#define TestScene_hpp

#include "vulkan.h"

class Scene
{
public:
    Scene();
    ~Scene();
    
private:
    VkInstance m_Instance;
};

#endif /* TestScene_hpp */
