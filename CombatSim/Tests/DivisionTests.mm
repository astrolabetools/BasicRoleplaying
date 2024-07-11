//
//  DivisionTests.m
//  Tests
//
//  Created by Ian Ollmann on 7/4/24.
//

#import <XCTest/XCTest.h>
#include "Division.hpp"

@interface DivisionTests : XCTestCase

@end

@implementation DivisionTests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}


// Signed
static inline bool TestDivision( long num, long denom)
{
    // Don't test undefined behavior
    if( denom == 0)
        return true;

    // Signed
    long result = num / denom;
    bool isExact = num == result * denom;
    
    long toZero = Divide_RoundZero( num, denom);
    long toUp = Divide_RoundUp( num, denom);
    long toDown = Divide_RoundDown( num, denom);
    long toNearest = Divide_RoundNearest( num, denom);
    
    if( isExact )
    {
        XCTAssert( toZero == toUp);
        XCTAssert( toZero == toDown);
        XCTAssert( toZero == toNearest);
        if( toZero != toUp || toZero != toDown || toZero != toNearest )
        {
            toZero = Divide_RoundZero( num, denom);
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }
    }
    else
    {
        XCTAssert(toUp == toDown + 1);
        XCTAssert(toNearest == toUp || toNearest == toDown);
        long remainder = num - toNearest*denom;
        XCTAssert( labs(remainder) <= labs(denom/2));
        
        if( toUp != toDown + 1 ||
           labs(remainder) > labs(denom/2))
        {
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }
        
        if( toNearest != toUp && toNearest != toDown)
        {
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }
    }

    return true;
}

// Unsigned
static inline bool TestDivision( unsigned long num, unsigned long denom)
{
    // Don't test undefined behavior
    if( denom == 0)
        return true;

    unsigned long result = num / denom;
    bool isExact = num == result * denom;
    
    unsigned long toZero = Divide_RoundZero( num, denom);
    unsigned long toUp = Divide_RoundUp( num, denom);
    unsigned long toDown = Divide_RoundDown( num, denom);
    unsigned long toNearest = Divide_RoundNearest( num, denom);
    
    if( isExact )
    {
        XCTAssert( toZero == toUp);
        XCTAssert( toZero == toDown);
        XCTAssert( toZero == toNearest);
        if( toZero != toUp || toZero != toDown || toZero != toNearest )
        {
            toZero = Divide_RoundZero( num, denom);
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }
    }
    else
    {
        XCTAssert(toUp == toDown + 1);
        XCTAssert(toNearest == toUp || toNearest == toDown);
        long remainder = num - toNearest*denom;
        XCTAssert( labs(remainder) <= denom/2);
        
        if( toUp != toDown + 1 || labs(remainder) > denom/2)
        {
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }

        if( toNearest != toUp && toNearest != toDown)
        {
            toUp = Divide_RoundUp( num, denom);
            toDown = Divide_RoundDown( num, denom);
            toNearest = Divide_RoundNearest( num, denom);
            return false;
        }
    }

    return true;
}

- (void)testDivisionRounding
{
    static const long kTable[] =
    {
        1,2,3,4,5,7,11,13,19,72,
        -1,-2,-3,-4,-5,-7,-11,-13,-19,-72,
        0, LONG_MIN, LONG_MIN+1, LONG_MAX, LONG_MAX-1
    };
    static const unsigned long count = sizeof(kTable) / sizeof(kTable[0]);
    
    for( unsigned long i = 0; i < count; i++)
    {
        long numerator = kTable[i];
        for( unsigned long k = 0; k < count; k++)
        {
            long denominator = kTable[k];
            
            XCTAssert( false != TestDivision( numerator, denominator));
            XCTAssert( false != TestDivision( (unsigned long) numerator, (unsigned long) denominator));
        }
    }
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
