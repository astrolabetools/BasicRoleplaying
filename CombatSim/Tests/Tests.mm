//
//  Tests.m
//  Tests
//
//  Created by Ian Ollmann on 7/4/24.
//

#import <XCTest/XCTest.h>
#import "Table.h"

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testTable {
    
    Table<const char*>::Entry data[] =
    {
        {01, "Adaptability"},
        {04, "Allergy"},
        {06, "Biped (Quadruped)*"},
        {10, "Camouflage"},
        {13, "Coloration"},
        {16, "Congenital Disease*"},
        {20, "Decreased Characteristic*"},
        {23, "Disease Carrier*"},
        {26, "Group Intelligence"},
        {30, "Hands"},
        {31, "Hardy"},
        {34, "Hybrid"},
        {36, "Imitation"},
        {38, "Increased Characteristic"},
        {45, "Keen Sense"},
        {49, "Luminescence"},
        {51, "Metabolic Improvement"},
        {54, "Metabolic Weakness*"},
        {58, "Natural Armor"},
        {61, "Natural Weaponry"},
        {66, "Pain Sensitivity*"},
        {68, "romone"},
        {69, "Reduced Sense*"},
        {71, "Regeneration"},
        {73, "Sensitivity"},
        {75, "Speech (Mimicry)"},
        {77, "Structural Improvement"},
        {82, "Structural Weakness*"},
        {87, "Venom90 Wings"},
        {91, "Gamemaster or Player Choice"},
    };
    
    Table table(
    
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
