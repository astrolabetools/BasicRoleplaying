//
//  Division.hpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#ifndef Division_h
#define Division_h

#include "CombatSimTypes.h"

static inline long lcopysign( long a, long sign )
{
    long signMask = (a^sign) >> (sizeof(a) * 8 -1);
    return (a ^ signMask) - signMask;
}

static inline unsigned long Divide_RoundDown( unsigned long numerator, unsigned long denominator ){ return numerator / denominator; }
static inline unsigned long Divide_RoundZero( unsigned long numerator, unsigned long denominator ){ return numerator / denominator; }
static inline unsigned long Divide_RoundUp( unsigned long numerator, unsigned long denominator )
{
    unsigned long result = numerator / denominator;
    if( result * denominator != numerator)
        result = max( result, result + 1);
    return result;
}
static inline unsigned long Divide_RoundNearest( unsigned long numerator, unsigned long denominator )
{
    unsigned long rounded = numerator + denominator/2;
    if( EXPECT_FALSE(rounded < numerator))
    {
        unsigned long result = numerator / denominator;
        unsigned long remainder = numerator - denominator * result;
        rounded = result + (remainder * 2 >= denominator);
        return max(rounded, result);
    }
    
    return rounded / denominator;
}

static inline long Divide_RoundZero( long numerator, long denominator ){ return numerator / denominator; }
static inline long Divide_RoundDown( long numerator, long denominator )
{
    long signResult = numerator ^ denominator;
    long result = Divide_RoundZero( numerator, denominator);
    if( numerator != denominator * result && signResult < 0 )
        result = min( result, result - 1);
    return result;
}

static inline long Divide_RoundUp( long numerator, long denominator )
{
    long signResult = numerator ^ denominator;
    long result = Divide_RoundZero( numerator, denominator);
    if( numerator != denominator * result && signResult >= 0 )
        result = max( result, result + 1);
    return result;
}

static inline long Divide_RoundNearest( long numerator, long denominator )
{
    long rounded = numerator + lcopysign( denominator/2, numerator);
    if( EXPECT_FALSE( (rounded ^ numerator) < 0))
    { // rounded overflowed
        long result = numerator / denominator;
        long remainder = numerator - denominator * result;
        if( (unsigned long) labs(remainder) > (unsigned long) labs(denominator/2) )
            result += lcopysign(1L, numerator ^ denominator);
        return result;
    }
    
    return rounded / denominator;
}

static inline uint32_t Divide_RoundUp( uint32_t num, uint32_t denom){ return uint32_t(Divide_RoundUp((unsigned long) num, (unsigned long) denom)); }
static inline uint32_t Divide_RoundDown( uint32_t num, uint32_t denom){ return uint32_t(Divide_RoundDown((unsigned long) num, (unsigned long) denom)); }
static inline uint32_t Divide_RoundZero( uint32_t num, uint32_t denom){ return uint32_t(Divide_RoundZero((unsigned long) num, (unsigned long) denom)); }
static inline uint32_t Divide_RoundNearest( uint32_t num, uint32_t denom){ return uint32_t(Divide_RoundNearest((unsigned long) num, (unsigned long) denom)); }

static inline uint16_t Divide_RoundUp( uint16_t num, uint16_t denom){ return uint16_t(Divide_RoundUp((unsigned long) num, (unsigned long) denom)); }
static inline uint16_t Divide_RoundDown( uint16_t num, uint16_t denom){ return uint16_t(Divide_RoundDown((unsigned long) num, (unsigned long) denom)); }
static inline uint16_t Divide_RoundZero( uint16_t num, uint16_t denom){ return uint16_t(Divide_RoundZero((unsigned long) num, (unsigned long) denom)); }
static inline uint16_t Divide_RoundNearest( uint16_t num, uint16_t denom){ return uint16_t(Divide_RoundNearest((unsigned long) num, (unsigned long) denom)); }

static inline int32_t Divide_RoundUp( int32_t num, int32_t denom){ return uint32_t(Divide_RoundUp((long) num, (long) denom)); }
static inline int32_t Divide_RoundDown( int32_t num, int32_t denom){ return uint32_t(Divide_RoundDown((long) num, (long) denom)); }
static inline int32_t Divide_RoundZero( int32_t num, int32_t denom){ return uint32_t(Divide_RoundZero((long) num, (long) denom)); }
static inline int32_t Divide_RoundNearest( int32_t num, int32_t denom){ return uint32_t(Divide_RoundNearest((long) num, (long) denom)); }

static inline int16_t Divide_RoundUp( int16_t num, int16_t denom){ return uint16_t(Divide_RoundUp((long) num, (long) denom)); }
static inline int16_t Divide_RoundDown( int16_t num, int16_t denom){ return uint16_t(Divide_RoundDown((long) num, (long) denom)); }
static inline int16_t Divide_RoundZero( int16_t num, int16_t denom){ return uint16_t(Divide_RoundZero((long) num, (long) denom)); }
static inline int16_t Divide_RoundNearest( int16_t num, int16_t denom){ return uint16_t(Divide_RoundNearest((long) num, (long) denom)); }

#endif /* Division_h */
