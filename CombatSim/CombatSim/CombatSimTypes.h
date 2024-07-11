//
//  CombatSimTypes.h
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#ifndef CombatSimTypes_h
#define CombatSimTypes_h


#ifndef __has_feature         // Optional of course.
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
  #define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif
#ifndef __has_attribute         // Optional of course.
  #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_builtin         // Optional of course.
  #define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif


#ifdef __OBJC__
#   if __has_feature(objc_arc)
#      ifndef OBJC_RETAIN
#          define OBJC_RETAIN(_a)       ({ __typeof__ (_a) __attribute__((__unused__)) _temp = (_a); /* return*/ _temp;})
#      endif
#      ifndef OBJC_RELEASE
#         define OBJC_RELEASE(_a)      {_a = nil;}
#      endif
#      ifndef OBJC_AUTORELEASE
#          define OBJC_AUTORELEASE(_a)  ({ __typeof__ (_a) __attribute__((__unused__)) _temp = (_a); /* return*/ _temp;})
#      endif
#   else
#      ifndef OBJC_RETAIN
#          define OBJC_RETAIN(_a)       [(_a) retain]
#      endif
#      ifndef OBJC_RELEASE
#          define OBJC_RELEASE(_a)      {[(_a) release]; _a = nil;}
#      endif
#      ifndef OBJC_AUTORELEASE
#          define OBJC_AUTORELEASE(_a)  [(a) autorelease]
#      endif
#   endif
#   ifdef __IPHONE_OS_VERSION_MIN_REQUIRED
#       define USING_IOS   1
#       import <UIKit/UIKit.h>
#   elif defined __MAC_OS_X_VERSION_MIN_REQUIRED
#       define USING_MACOS   1
#       import <AppKit/AppKit.h>
#   else
#       error   Unhandled platform
#   endif
#endif /* __OBJC__ */

#include <Astrolabe/Types.h>
#include <assert.h>

#if ! defined (EXPECT_TRUE)
#   if __has_builtin(__builtin_expect)
#       define EXPECT_TRUE(_a)      __builtin_expect(_a, 1L)
#   else
#       define EXPECT_TRUE(_a)      (_a)
#   endif
#endif

#if  ! defined (EXPECT_FALSE)
#   if __has_builtin(__builtin_expect)
#       define EXPECT_FALSE(_a)     __builtin_expect(_a, 0L)
#   else
#       define EXPECT_FALSE(_a)     (_a)
#   endif
#endif



#endif /* CombatSimTypes_h */
