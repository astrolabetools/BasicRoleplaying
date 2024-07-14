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
#include "SkillCheck.hpp"
#include <Astrolabe/Expression.h>

#ifdef __cplusplus

using namespace AstrolabeExpression;

class Creature;
class Attack
{
private:
    Astrolabe::LinkedListNode<Attack>           next;
    Astrolabe::AutoString                       name;
    AstrolabeExpression::Expression * __nonnull damage;         //strong
    Astrolabe::AutoString                       description;
    Skill                                       skill;
    
    Attack(const Attack & a);
    Attack & operator=(const Attack & a);
    
public:
    Attack( long _skill,
           AstrolabeExpression::Expression * __nonnull _damage,
           const char * __nonnull _name,
           const char * __nullable _description = NULL) :
            damage(_damage), name(_name), description(_description), skill(_skill){}
    ~Attack(){ AstrolabeExpression::ExpressionDelete(damage);}
    
    inline const Skill & GetSkill() const { return skill; }
    inline const char * __nullable GetDescription() const { return description; }
    AstrolabeExpression::Value GetDamage( ExpressionDiceMode diceMode, const Creature & attacker ) const;
    
    inline Attack * __nullable GetNext() const { return next; }
    inline Attack * __nullable WARN_UNUSED_RESULT ExchangeNext( Attack * __nullable newNext){ return next.ExchangeNext(newNext); }
    inline void  ReplaceNext( Attack * __nullable newNext){ return next.ReplaceNext(newNext); }
    
    bool Print( unsigned long indentLevel = 0, Extent * __nullable where = NULL ) const;
};

class Species
{
private:
    Context                                 context;
    Astrolabe::LinkedListNode<Species>      next;
    Astrolabe::LinkedList<Attack>           attacks;

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
    
protected:
    friend Creature;
    
    inline const Context & GetContext() const { return context; }
    
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
    void IterateAttacks( void(^ __nonnull block)(const Attack &)) const { return attacks.Iterate(^bool(Attack * _Nonnull node) { block(*node); return false; });}
    
    
    // Required methods for LinkedListNode owner
    inline Species * __nullable GetNext() const { return next; }
    inline Species * __nullable WARN_UNUSED_RESULT ExchangeNext( Species * __nullable newNext){ return next.ExchangeNext(newNext); }
    inline void  ReplaceNext( Species * __nullable newNext){ return next.ReplaceNext(newNext); }
    inline int Compare( const Species & o) const { return strcmp(GetName(), o.GetName()); }
    
    bool Print( unsigned long indentLevel = 0, Extent * __nullable where = NULL ) const;
};

class Creature
{
private:
    AstrolabeExpression::Context            context;
    const Attack * __nonnull                attack;
    const Species &                         species;
    Astrolabe::AutoString                   name;
    Astrolabe::LinkedListNode<Creature>     next;

public:
    Creature( const char * __nonnull name,  const Species & s);
    
    inline const AstrolabeExpression::Context & GetContext() const { return context; }
    inline const Attack & GetAttack() const { return *attack; }
    
    bool Print( unsigned long indentLevel = 0, Astrolabe::Extent * __nullable where = NULL ) const;
    inline const Species & GetSpecies() const{ return species;}
    inline const char * __nonnull GetSpeciesName() const { return species.GetContext().GetName(); }
    inline const char * __nonnull GetName() const { return name; }
    
    inline Creature * __nullable GetNext() const { return next; }
    inline Creature * __nullable WARN_UNUSED_RESULT ExchangeNext( Creature * __nullable newNext){ return next.ExchangeNext(newNext); }
    inline void  ReplaceNext( Creature * __nullable newNext){ return next.ReplaceNext(newNext); }
    inline int Compare( const Creature & o) const { int result = strcmp( GetSpeciesName(), o.GetSpeciesName()); if(result) return result; return strcmp(GetName(), o.GetName()); }

};

#endif /* __cplusplus */
#endif /* Creature_h */
