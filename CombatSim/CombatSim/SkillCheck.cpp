//
//  SkillCheck.cpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/5/24.
//

#include "SkillCheck.hpp"


CombatResult::CombatResult(SkillResult attacker, SkillResult defender) : bits(0)
{
    outcome = int16_t(attacker);
    if( attacker <= SkillResultFailure )
    {
        attackerFumbles = attacker == SkillResultFumble;
        return;
    }
    if( defender <= SkillResultFailure )
    {
        defenderFumbles = defender == SkillResultFumble;
        return;
    }

    SkillResult delta = SkillResult(int(attacker) - int(defender));
    if( delta < 0)
        attackWeaponDamage = -delta;
    else if(outcome > 0)
        parryWeaponDamage = 2 * delta;
    
    outcome = max(delta, SkillResultFailure);
}

#pragma mark -



