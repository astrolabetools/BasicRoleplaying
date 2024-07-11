//
//  Creature.h
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#ifndef Creature_h
#define Creature_h

#include <stdio.h>
#include <assert.h>
#include "CombatSimTypes.h"
#include <Astrolabe/Expression.h>

#ifdef __cplusplus

using namespace AstrolabeExpression;




class Creature
{
private:
    Context     context;
    
public:
    Creature( NSString * __nonnull name, NSDictionary * __nonnull d) :
    context(Type::RoundToNearestInteger, [name cStringUsingEncoding: NSUTF8StringEncoding]){}
    
    Value GetValue( const char * __nonnull name) const { return context.GetVariable(name)->GetValue();}
    
    
    
};


#endif /* __cplusplus */
#endif /* Creature_h */
