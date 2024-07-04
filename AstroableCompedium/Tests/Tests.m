//
//  Tests.m
//  Tests
//
//  Created by Ian Ollmann on 7/3/24.
//

#import <XCTest/XCTest.h>
#include "AstrolabeCompendium.h"

#if __has_feature(objc_arc)
#   define IF_NOT_ARC(_a)
#   define IF_ARC(_a)      (_a)
#   define RELEASE(_a)     {(_a) = nil;}
#   define RETAIN(_a)      ({ __typeof__(_a) __a = _a; /*return*/ __a;})
#   define AUTORELEASE(_a) ({ __typeof__(_a) __a = _a; /*return*/ __a;})
#else
#   define IF_NOT_ARC(_a)  (_a)
#   define IF_ARC(_a)
#   define RELEASE(_a)     [(_a) release]
#   define RETAIN(_a)      [(_a) retain]
#   define AUTORELEASE(_a) [(_a) autorelease]
#endif

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

static void Done()
{
    printf( "Done.\n");
}

- (void)testExample {
    atexit(Done);
    
    NSString * resources = [[NSBundle bundleForClass:[self class]] resourcePath];
    NSString * jpegPath = [NSString stringWithFormat: @"%@/KiddiePool.jpeg", resources ];
    
    NSError * error = nil;
    NSImage * image = AstrolabeCompendiumCreateImageFromURL( [NSURL fileURLWithPath: jpegPath], &error);
    XCTAssert( nil == error);
    XCTAssert( nil != image);
    
    error = nil;
    NSString * plistPath = [NSString stringWithFormat: @"%@/Items.plist", resources ];
    NSMutableDictionary * compendium = AstrolabeCompendiumLoadFromURL( [NSURL fileURLWithPath: plistPath], &error);
    XCTAssert( nil == error);
    XCTAssert( nil != compendium);

    NSString * imageKey = @"<testImage>";
    compendium[imageKey] = image;
    RELEASE(image);
    
    error = nil;
    NSURL * testURL = [NSURL fileURLWithPath: @"/tmp/test.plist"];
    AstrolabeCompendiumWriteToURL( compendium, testURL, &error);
    XCTAssert( nil == error);
    XCTAssert( nil != compendium);
    RELEASE(compendium);
    
    // no leaks to this point
    
    error = nil;
    NSMutableDictionary * compendium2 = AstrolabeCompendiumLoadFromURL( testURL, &error);
    XCTAssert( nil == error);
    XCTAssert( nil != compendium2);

    NSObject * image2 = compendium2[imageKey];
    XCTAssert( [image2 isKindOfClass: NSImage.class] );

    RELEASE(compendium2);
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
