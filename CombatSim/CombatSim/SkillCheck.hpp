//
//  SkillCheck.hpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#ifndef SkillCheck_h
#define SkillCheck_h

#include "Division.hpp"
#include <math.h>

#ifdef __cplusplus
extern "C"  {
#   define CONSTEXPR    constexpr
#else
#   define CONSTEXPR
#endif

#ifndef RANDOM
#   define RANDOM(_maxVal)      (arc4random_uniform(_maxVal))
//#   define RANDOM(_maxVal)      (rand(_maxVal))
#endif

/*! @abstract  Roll countDsides */
static inline long RollDice( long count, uint32_t sides)
{
    long result = 0;
    for( long i = 0; i < labs(count); i++)
        result += RANDOM(sides) + 1UL;
    return count < 0 ? -result : result;
}

/*! @abstract  Roll a D100*/
static inline long RollD100(){ return RANDOM(100) + 1UL; }

class Percentile
{
private:
    long    value;
    
public:
    Percentile() : value(RollD100()){}
    Percentile( long percentile) : value(percentile){}
    Percentile( double percentile) : value(lround(100.0 * percentile)){}
    
    inline long GetValue() const { return value; }
    
    inline Percentile & operator*=(double d){value = lround(value * d); return *this;}
    inline Percentile & operator/=(double d){value = lround(value * d); return *this;}
    inline Percentile & operator+=(const Percentile &p){value += p.value; return *this;}
    inline Percentile & operator-=(const Percentile &p){value -= p.value; return *this;}
};

typedef enum : int8_t
{
    SkillResultInvalid = -2,
    SkillResultFumble = -1,
    SkillResultFailure = 0,
    SkillResultSuccess,
    SkillResultSpecial,
    SkillResultCritical,
    
    SkillResultCount
}SkillResult;
inline SkillResult operator-(SkillResult active, SkillResult resisting){ return max( SkillResult((int)active - int(resisting)), SkillResultFumble); }
inline SkillResult operator+(SkillResult a, int b){ return SkillResult(int(a) + b);}
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

class Characteristic;
class Skill
{
private:
    long    value;
    
    Skill();
    
    // Page 110 -- Skill results table shows rounding to be round to +Infinity
    inline long GetCritical() const {return Divide_RoundUp(value, 20L);}
    inline long GetSpecial() const {return Divide_RoundUp(value, 5L);}
    inline long GetFumble() const {return 95L + (long) GetCritical(); }
    
public:
    Skill(long _value) : value(_value){}
    Skill(const Characteristic &c);
    
    inline Skill& operator+=( const Percentile & p){ value += p.GetValue(); return *this;}
    inline Skill& operator-=( const Percentile & p){ value -= p.GetValue(); return *this;}
    
    inline long GetValue() const { return value; }
    
    inline Skill Automatic() const { return Skill(1000000);}          // p 113
    inline Skill Easy() const { return Skill(value * 2);}             // p 113
    inline Skill Normal() const { return *this;}                      // p 113
    inline Skill Difficult() const { return Skill((value+1) >> 1);}   // p 113
    inline Skill Impossible() const { return Skill(1);}               // p 113
    
    /*! @abstract objects vastly different in size trigger a skill penalty or bonus */
    inline Skill SizePenalty(uint32_t mySize, uint32_t targetSize) const        // p 142
    { // Note: inanimate large objects are easy to hit, but not included here
        if(targetSize<=1) return Difficult();
        if( mySize >= 2 * targetSize)  return Skill( value - 20);
        if( targetSize >= 2 * mySize)  return Skill( value + 20);
        return *this;
    }
    
    /*! @abstract attacks or parries with a broken weapon are difficult*/
    inline Skill BrokenWeapon() const { return Difficult(); }         // p 142

    /*! @abstract Skill when attacking or defending against nvisible creatures. Defenders must also pass a listen, sense or spot roll */
    inline Skill InvisibleDifficulty( bool spotted, bool isAttacking) const { return (spotted ^ isAttacking) ? Skill( Divide_RoundUp(value, 5L)) :
                                                                                        (spotted ? Difficult() : Impossible());}   // p 62

    /*! @abstract Skill rating for a parry.  Count is the number of parries already completed this round */
    inline Skill ParryDifficulty(unsigned long count) const { return Skill( value - 30 * count); } // p 125
    
    inline Skill ImprovisedWeapon() const { return Difficult(); }

    /*! @abstract Classify a D100 roll as {fumble, failure, success, special, critical }*/
    inline SkillResult ClassifyRoll(const Percentile & p) const;                // p 110, 126
    
    /*! @abstract Alternative Method to calculate the skill result when opposing a skill rolll  p 112 */
    inline Skill OpposeSkillRollWithResistanceTable( const Skill & activeSkill ) const { return Skill(50 + (activeSkill.value - value)); }

};
static inline Skill operator+( const Skill & s, const Percentile & p){ return Skill( s.GetValue() + p.GetValue());}
static inline Skill operator-( const Skill & s, const Percentile & p){ return Skill( s.GetValue() - p.GetValue());}

class Characteristic
{
private:
    long    value;

    Characteristic();

public:
    Characteristic( long _value) : value(_value){}

    inline long GetValue() const { return value; }
    
    /*! @abstract Find the effective skill level for a contested characteristic check  e.g. CON vs poison POW   p 110*/
    inline Skill Resist( const Characteristic & active ) const { return Skill(50 + 5*(active.GetValue() - value)); }
};

typedef union CombatResult
{
    struct
    {
        uint16_t outcome : 8;       // int8_t SkillResult
        uint16_t attackerFumbles : 1;
        uint16_t attackWeaponDamage : 3;
        uint16_t defenderFumbles : 1;
        uint16_t parryWeaponDamage : 3;
    };
    uint16_t bits;
    
    CombatResult() : bits(0){outcome = SkillResultInvalid;}
    CombatResult(SkillResult _outcome, bool attackerFumble = 0, int damageToWeapon = 0, bool defenderFumble = 0, int damageToShield = 0) :
        outcome(int8_t(_outcome)), attackerFumbles(attackerFumble), attackWeaponDamage(damageToWeapon),
        defenderFumbles(defenderFumble), parryWeaponDamage(damageToShield){}
    CombatResult(SkillResult attacker, SkillResult defender = SkillResultFailure);
    CombatResult(const CombatResult & r) : bits(r.bits){}
    
    inline bool operator==(const CombatResult & r){ return bits == r.bits; }
    
    inline operator SkillResult() const { return SkillResult(int8_t(outcome)); }
    inline unsigned long GetDamageToWeapon() const { return attackWeaponDamage;}
    inline unsigned long GetDamageToParry() const { return parryWeaponDamage; }
    
    inline bool IsCritical() const { return SkillResult() == SkillResultCritical; }
    inline bool IsSpecial() const { return SkillResult() == SkillResultSpecial; }
    inline bool IsFumble() const { return SkillResult() == SkillResultFumble; }
    inline bool IsAKindOfFailure() const { return SkillResult() <= SkillResultFailure; }
    inline bool DidAttackerFumble() const { return attackerFumbles;}
    inline bool DidDefenderFumble() const { return defenderFumbles;}

    inline long GetParryWeaponDamage() const { return long(parryWeaponDamage);}
    inline long GetAttackWeaponDamage() const { return long(attackWeaponDamage);}
}CombatResult;

class Roll
{
protected:
    long roll;
    long skill;

    Roll( long _roll, long _skill) : roll(_roll), skill(_skill){};

public:
    Roll( const Skill & s) : roll(RollD100()), skill(s.GetValue()){};
    
    // Note: modifiers occur before difficult adjustments if the modifier is permanent, then
    //       it is before the difficulty. Otherwise it is after. p114
    inline Roll & operator+=(const Percentile &p){skill += p.GetValue(); return *this;}                                                      // for modifiers
    inline Roll & operator-=(const Percentile &p){skill -= p.GetValue(); return *this;}                                                      // for modifiers
    inline Roll & operator*=(const Percentile &p){skill = Divide_RoundNearest(skill * p.GetValue(), 100L); return *this;}                    // for modifiers
    inline Roll & operator/=(const Percentile &p){skill = Divide_RoundNearest(skill * 100, p.GetValue()); return *this;}                     // for modifiers
    inline Roll & operator*=(double d){skill = lround(skill * d); return *this;}                     // for modifiers
    inline Roll & operator/=(double d){skill = lround(skill / d); return *this;}                     // for modifiers
    
    inline bool operator>(const Roll & r) const { int delta = int(Classify()) - int(r.Classify()); if(delta) return delta > 0; return (skill - roll) > (r.skill - r.roll); }
    inline bool operator<(const Roll & r) const { int delta = int(Classify()) - int(r.Classify()); if(delta) return delta < 0; return (skill - roll) < (r.skill - r.roll); }
    inline bool operator==(const Roll & r) const { return Classify() == r.Classify() and  (skill - roll) == (r.skill - r.roll); }

    inline bool IsCritical() const { return roll <= Divide_RoundUp(skill, 20L);}               // pp 110, 126  see also table on 110
    inline bool IsSpecial()  const { return roll <= Divide_RoundUp(skill, 5L);}                // pp 110, 126  see also table on 110
    inline bool IsFumble()   const { return roll >= 95 + Divide_RoundUp(skill, 20L);}          // pp 110, 126  see also table on 110
    
    /*! @abstract Classify the roll as fumble, failure, success, special or critical*/
    inline SkillResult Classify() const;
    
    /*! @abstract Calculate the skill result when opposing a skill rolll */
    inline SkillResult OpposeSkillRoll( const Roll & activeRoll ) const;

    /*! @abstract Calculate the skill result when opposing a skill rolll */
    inline SkillResult OpposeSkillRollWithResistanceTable( const Roll & activeRoll ) const{ return Roll(roll, Skill(skill).OpposeSkillRollWithResistanceTable(Skill(activeRoll.skill)).GetValue()).Classify();}

    /*! @abstract Calculate the skill result when opposing a skill rolll */
    inline CombatResult OpposeCombatRoll( const Roll & activeRoll ) const { return CombatResult(activeRoll.Classify(), Classify()); }
    
    /*! @abstract Cooperative Skill Roll, p 112   If multiple helpers, take the best result or a fumble if one occurs. */
    inline void Cooperate( const Roll & helper){ const static int8_t kHelpTable[5] = {-50, -10, 20, 30, 50};   skill += kHelpTable[int(helper.Classify()) - int(SkillResultFumble)]; }
};



#pragma mark -

inline Skill::Skill(const Characteristic &c) : value(5L * c.GetValue()){}

inline SkillResult Roll::Classify() const
{
    if(roll > skill)
        return IsFumble() ? SkillResultFumble : SkillResultFailure;

    if( IsSpecial())
        return IsCritical() ? SkillResultCritical : SkillResultSpecial;

    return SkillResultSuccess;
}

inline SkillResult Roll::OpposeSkillRoll( const Roll & activeRoll ) const
{
    SkillResult activeResult = activeRoll.Classify();
    SkillResult opposedResult = Classify();
    if( activeResult == opposedResult)
        return roll < activeRoll.roll ? SkillResultSuccess : SkillResultFailure;

    return activeResult - opposedResult;        // operator- clamps
}


#ifdef __cplusplus
}
#endif


#endif /* SkillCheck_h */
