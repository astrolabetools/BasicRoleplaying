//
//  SkillCheck.h
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#ifndef SkillCheck_h
#define SkillCheck_h

#include "CombatSimTypes.h"

#ifdef __cplusplus
extern "C"  {
#   define CONSTEXPR    constexpr
#else
#   define CONSTEXPR
#endif

static inline long RollDice( long count, uint32_t sides)
{
    long result = 0;
    for( long i = 0; i < labs(count); i++)
        result += arc4random_uniform(sides) + 1UL;
    return count < 0 ? -result : result;
}
static inline long RollD100(){ return RollDice(1,100); }

typedef enum : int8_t
{
    SkillResultInvalid = -1,
    SkillResultFumble,
    SkillResultFailure,
    SkillResultSuccess,
    SkillResultSpecial,
    SkillResultCritical,
    
    SkillResultCount
}SkillResult;
inline static const char * GetName( SkillResult s)
{
    if( s >= SkillResultCount || s < SkillResultInvalid)
        return "<Invalid Skill Result>";
    
    static const char * kSkillResultNames[] =
    {
        "Invalid",
        "Fumble",
        "Failure",
        "Success",
        "Special",
        "Critical"
    };
    
    return kSkillResultNames[s-SkillResultInvalid];
}

static inline long CONSTEXPR divide_rtz(long numerator, long denominator){ return numerator / denominator;}


// Divide, round to nearest integer, ties away from zero
static inline long CONSTEXPR divide_rta(long numerator, long denominator)
{
    long round = denominator / 2;
    if( EXPECT_FALSE(numerator<0)))
        round = -round;
    long rounded = numerator + round;
    assert( (rounded ^ numerator) >= 0);
    
    return rounded / denominator;
}

// Divide, round toward +Infinity
static inline long CONSTEXPR divide_rtp(long numerator, long denominator)
{
    
}


// Page 110 -- Skill results table shows rounding to be round to +Infinity
static inline long GetCritical( long skill ){ return divide_rtp(skill, 20); }
static inline long GetSpecial( long skill ){ return divide_rtp(skill, 5); }
static inline long GetFumble( long skill ){ return min(101 - GetCritical(skill), 100); }

static inline SkillResult SkillCheck( long skill )
{
    long roll = RollD100();
    if( roll <)
}


#ifdef __cplusplus
}
#endif


#endif /* SkillCheck_h */
