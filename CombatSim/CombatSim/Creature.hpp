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


class Species
{
private:
    Context                                 context;
    Astrolabe::LinkedListNode<Species>      next;

    Species();
    Species( const Species &);
    Species & operator=(const Species &);
    
    // Expressions
    AstrolabeExpression::Expression * __nullable ReadExpression( NSDictionary * __nonnull dict,
                                                                 NSString * __nonnull key);

    AstrolabeExpression::Expression * __nullable CreateExpression( const char * __nonnull name, const char * __nullable expressionStr);

    void AddCharacteristic( const char * __nullable name,
                            AstrolabeExpression::Expression * __nullable e);


    // Numbers
    AstrolabeExpression::Value ReadNumber( NSDictionary * __nonnull dict,
                                           NSString * __nonnull key,
                                           AstrolabeExpression::Value defaultValue );

    void AddCharacteristic( const char * __nullable name,
                            AstrolabeExpression::Value value );
    
    // lists
    void ReadAttacks( NSObject * __nonnull attacks);
    void ReadSkills( NSObject * __nonnull skills);
    void ReadModifiers( NSObject * __nonnull modifiers);

    static Astrolabe::LinkedList<Species>   speciesList;

public:
    static const Astrolabe::LinkedList<Species> & GetList(){ return Species::speciesList; }
    
#ifdef __OBJC__
    static void ImportURL( NSURL * __nonnull path,
                           NSError * __nullable __autoreleasing * __nonnull error );
    Species( const char * __nonnull name, NSDictionary * __nonnull d);
#endif
    
    inline const char * __nonnull GetName() const {return context.GetName();}
    inline const char * __nonnull GetDescription() const { return context.GetDescription();}
    inline Value GetValue( const char * __nonnull name) const { return context.GetVariable(name)->GetValue();}

    // Required methods for LinkedListNode owner
    inline Species * __nullable GetNext() const { return next; }
    inline Species * __nullable WARN_UNUSED_RESULT ExchangeNext( Species * __nullable newNext){ return next.ExchangeNext(newNext); }
    inline void  ReplaceNext( Species * __nullable newNext){ return next.ReplaceNext(newNext); }
    inline int Compare( const Species & o) const { return strcmp(GetName(), o.GetName()); }
};

class Creature
{
private:
    
    
public:
    Creature( const Species & s);
    
};

#endif /* __cplusplus */
#endif /* Creature_h */
