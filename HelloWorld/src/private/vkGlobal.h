//
//  vkGlobal.h
//  HelloWorldiOS
//
//  Created by Yevgeniy Logachev on 10/10/17.
//  Copyright © 2017 Yevgeniy Logachev. All rights reserved.
//

#ifndef vkGlobal_h
#define vkGlobal_h

#include <functional>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <future>
#include <thread>
#include <mutex>
#include <random>
#include <typeindex>
#include <memory>
#include <tuple>
#include <utility>
#include <math.h>
#include <assert.h>
#include <fstream>

#include "vulkan.h"

// LINUX
#if defined(OS_LINUX)
#    include <EGL/egl.h>
#    include <GLES2/gl2.h>
#   include <sys/time.h>
#define LOGI(...)
// WINDOWS
#elif defined(OS_WINDOWS)
#    define NOMINMAX
#    include <windows.h>
#    include <EGL/egl.h>
#    include <GLES2/gl2.h>
#    include <GLES2/gl2ext.h>
#    include <GLES2/gl2platform.h>
#define LOGI(...)
// MACOS
#elif defined(OS_MAC)
#    include <OpenGL/gl.h>
#    include <OpenGL/glext.h>
#    include <OpenGL/gl3.h>
#    include <OpenGL/gl3ext.h>
#   include <sys/time.h>
#if defined(__OBJC__)
#   import <Cocoa/Cocoa.h>
#endif
#define LOGI(...)
// IPHONE
#elif defined(OS_IPHONE)
#   include <OpenGLES/ES1/gl.h>
#   include <OpenGLES/ES1/glext.h>
#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>
//#   include <OpenGLES/ES3/gl.h>
//#   include <OpenGLES/ES3/glext.h>
#    include <sys/time.h>
#if defined(__OBJC__)
#   import <Foundation/Foundation.h>
#   import <UIKit/UIKit.h>
#endif
#define LOGI(...)
// ANDROID
#elif defined(OS_ANDROID)
#   include <stdlib.h>
#   include <jni.h>
#   include <android/log.h>
#    include <GLES2/gl2.h>
#    define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "INFO: ", __VA_ARGS__))
#elif defined(OS_KOS)
#    include <kos.h>
#    include <GL/gl.h>
#    include <GL/glu.h>
#    include <GL/glut.h>
#    include <GL/glext.h>
#    include <sys/time.h>
#endif

#define CLASS_PTR(_class) typedef std::shared_ptr<class _class> _class##Ptr;\
                          typedef std::weak_ptr<class _class> _class##Weak;

#if defined(OS_WINDOWS) // TODO
#define INL
#else
#define INL inline __attribute__ ((always_inline))
#endif

#define MULTI_LINE_STRING(a) #a

#if LOGS_ENABLED
#   define VKGL_LOG(...) printf(__VA_ARGS__)
#else
#   define VKGL_LOG(...)
#endif

#endif /* vkGlobal_h */
