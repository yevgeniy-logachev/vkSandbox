//
//  GameViewController.m
//  vkSandbox
//
//  Created by Yevgeniy Logachev on 11/14/16.
//  Copyright © 2016 Yevgeniy Logachev. All rights reserved.
//

#import "GameViewController.h"
#include "scene.hpp"


@implementation GameViewController
{
    // view
    MTKView *_view;
    CADisplayLink* _displayLink;
    Scene *_scene;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self _setupView];
}

- (void)_setupView
{
    _view = (MTKView *)self.view;
    //_view.delegate = self;
    
    _scene = new Scene();
    _scene->Initialize((__bridge void *)_view);
    
    // Setup the render target, choose values based on your app
    _view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    
    uint32_t fps = 60;
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderLoop)];
    [_displayLink setPreferredFramesPerSecond:fps];
    [_displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
}

-(void) renderLoop {
    if (_scene && _scene->m_IsInitialized) {
        _scene->Update();
    }
}


#pragma mark MTKViewDelegate

// Called whenever view changes orientation or layout is changed
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    
}

// Called whenever the view needs to render
- (void)drawInMTKView:(nonnull MTKView *)view
{
    
}

@end

