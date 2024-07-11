//
//  Creature.cpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/4/24.
//

#include "Creature.hpp"
#include "SkillCheck.hpp"
#include "AstrolabeCompendium.h"
#include <Astrolabe/Expression.h>

Astrolabe::LinkedList<Species> Species::speciesList;



void Species::ImportURL(NSURL * _Nonnull url,
                        NSError * _Nullable __autoreleasing * _Nonnull error)
{
    NSMutableDictionary * d = AstrolabeCompendiumLoadFromURL( url, error);
    if( nil == d || *error )
        return;
    
    [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
        if( KeyIsMetadata(key) || NO == [obj isKindOfClass: NSDictionary.class])
            return;
        
        speciesList.Append( new Species([(NSString*) key cStringUsingEncoding: NSUTF8StringEncoding],
                                         (NSDictionary*) obj) );
    }];
}

static inline const char * __nullable ReadString( NSObject * __nullable obj)
{
    if([obj isKindOfClass: NSAttributedString.class])
        obj = [(NSAttributedString*)obj string];
    
    if([obj isKindOfClass: NSString.class])
        return [(NSString*) obj cStringUsingEncoding: NSUTF8StringEncoding];

    return NULL;
}

static inline void ReportParseError( const char * __nonnull name,
                                     const char * __nonnull str,
                                     const ConstExtent & region,
                                     const char * __nullable errString)
{
    ConstExtent originalExtent(str);
    bool overflow = false;
    assert( originalExtent.Contains( region));
    
    char buf[4096];
    Extent extent(buf, sizeof(buf));
    extent.Printf( "Syntax error parsing %s: \"%s\"\n", name, str);
    extent.Printf( "                     ");
    unsigned long strSize = strlen(str);
    for( unsigned long i = 0; i < strSize + strlen(": "); i++)
        extent.Write(' ', &overflow);
    
    unsigned long skipSpace = region.GetStart<char*>() - originalExtent.GetStart<char*>();
    for( unsigned long i = 0; i < skipSpace; i++)
        extent.Write(' ', &overflow);
    for( unsigned long i = 0; i < originalExtent.GetSize(); i++)
        extent.Write('^', &overflow);
    extent.Write('\n', &overflow);
    if( errString)
        extent.Printf("\t%s\n", errString);
    buf[sizeof(buf)-1] = '\0';
    
    fprintf( stderr, "%s", buf);
}


inline AstrolabeExpression::Expression * __nullable
    Species::ReadExpression( NSDictionary * __nonnull dict,
                              NSString * __nonnull key)
{
    using namespace AstrolabeExpression;
        
    const char * str = ReadString(dict[key]);
    if( NULL == str )
        return NULL;
        
    const char * outError = NULL;
    Astrolabe::ConstExtent extent(str);
    Expression * result = ExpressionCreate(extent, outError);
    if( NULL == outError)
        return result;
        
        
    NSString * name = [NSString stringWithFormat: @"%s[%@]", GetName(), key];
    ReportParseError( [name cStringUsingEncoding: NSUTF8StringEncoding],
                      str, extent, outError );
    return NULL;
}

AstrolabeExpression::Expression * __nullable Species::CreateExpression( const char * name,
                                                                         const char * __nullable str)
{
    const char * outError = NULL;
    Astrolabe::ConstExtent extent(str);
    Expression * result = ExpressionCreate(extent, outError);
    if( NULL == outError)
        return result;
    
    ExpressionDelete(result);
    char buf[256];  snprintf(buf, sizeof(buf), "%s[%s]", GetName(), name);
    ReportParseError( buf, str, extent, outError );
    return NULL;
}

AstrolabeExpression::Value Species::ReadNumber( NSDictionary * __nonnull dict,
                                                NSString * __nonnull key,
                                                AstrolabeExpression::Value defaultValue )
{
    NSObject * obj = dict[key];
    if( NULL == obj)
        return defaultValue;
    
    if( [obj isKindOfClass: NSNumber.class])
        return [(NSNumber*) obj doubleValue];
    
    if( [obj isKindOfClass: NSAttributedString.class])
        obj = [(NSAttributedString*) obj string];

    char buf[512];  snprintf(buf, sizeof(buf), "%s[%s]", this->GetName(), ReadString(key));
    if( [obj isKindOfClass: NSString.class])
    {
        AstrolabeExpression::Expression * e = CreateExpression( buf, [(NSString*) obj cStringUsingEncoding: NSUTF8StringEncoding]);
        if( NULL == e)
            return defaultValue;
        return AstrolabeExpression::ExpressionEvaluate(e, context);
    }
    
    const char * description = [[obj debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
    ConstExtent extent(description);
    ReportParseError( buf, description, extent, "Failed to parse object as a number or numerical expression");
    return defaultValue;
}


static const char * kBRPIntegerTypeName = "Integer";
static const char * kBRPRealTypeName = "Real";
static const char * kBRPBooleanTypeName = "Boolean";

Species::Species( const char * __nonnull name, NSDictionary * __nonnull d) :
        context(Type::RoundToNearestInteger, name)
{
    // Set up some types
    Type::Create(context, kBRPIntegerTypeName, Type::Signed, Type::RoundToNearestInteger);
    Type::Create(context, kBRPRealTypeName, Type::Real, Type::RoundingNone);
    Type::Create(context, kBRPBooleanTypeName, Type::Boolean, Type::RoundDown);

    @autoreleasepool {
        context.SetDescription( ReadString( d[@"Description"]));
        AddCharacteristic( "STR", ReadExpression(d, @"STR"));
        AddCharacteristic( "CON", ReadExpression(d, @"CON"));
        AddCharacteristic( "SIZ", ReadExpression(d, @"SIZ"));
        AddCharacteristic( "SIZ", ReadExpression(d, @"SIZ"));
        AddCharacteristic( "INT", ReadExpression(d, @"INT"));
        AddCharacteristic( "POW", ReadExpression(d, @"POW"));
        AddCharacteristic( "DEX", ReadExpression(d, @"DEX"));
        AddCharacteristic( "CHA", ReadExpression(d, @"CHA"));
        AddCharacteristic( "MOV", ReadExpression(d, @"MOV"));
        AddCharacteristic( "Armor", ReadNumber(d, @"Armor", 0));

        ReadAttacks( d[@"Attacks"]);
        ReadSkills( d[@"Skills"]);
        ReadSkills( d[@"Modifiers"]);
    }
}


void Species::AddCharacteristic( const char * __nullable name,
                                 AstrolabeExpression::Expression * __nullable e)
{
    if( NULL == name || NULL == e)
        return;
    
    char funcName[256];   snprintf( funcName, sizeof(funcName), "%s_definition", name);
    Type * type = context.GetType(kBRPIntegerTypeName);
    Function * f = Function::Create( funcName, type, e);    // added to context using type.context
    Variable::Create( name, NAN, type, f);                  // added to context using type.context
}

void Species::AddCharacteristic( const char * __nullable name,
                                 AstrolabeExpression::Value value)
{
    if( NULL == name )
        return;
    
    Type * type = context.GetType(kBRPIntegerTypeName);
    Variable::Create( name, value, type);                  // added to context using type.context
}

void Species::ReadAttacks( NSObject * __nonnull attacks)
{
    char buf[512];
    
    if( [attacks isKindOfClass: NSDictionary.class])
    {
        NSDictionary * d = (NSDictionary*) attacks;
        [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) 
        {
            char name[512];  snprintf( name, sizeof(name), "Attack::%s", [key cStringUsingEncoding: NSUTF8StringEncoding]);
            if( [obj isKindOfClass: NSNumber.class])
                return AddCharacteristic( name, [(NSNumber*)obj doubleValue]);

            if( [obj isKindOfClass: NSAttributedString.class])
                obj = [(NSAttributedString*) obj string];
            if( [obj isKindOfClass: NSString.class])
            {
                AstrolabeExpression::Expression * e = CreateExpression(name, [(NSString*) obj cStringUsingEncoding: NSUTF8StringEncoding]);
                return AddCharacteristic(name, e);
            }
            
            const char * description = [[obj debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
            ConstExtent extent(description);
            ReportParseError( name, description, extent, "Unknown field type / not usable for encoding an attack");
        }];
        return;
    }
    
    if( [attacks isKindOfClass: NSAttributedString.class])
        attacks = [(NSAttributedString*) attacks string];
    
    if( [attacks isKindOfClass: NSString.class])
    {
        AstrolabeExpression::Expression * e = CreateExpression("<anonymous attack>", [(NSString*) attacks cStringUsingEncoding: NSUTF8StringEncoding]);
        return AddCharacteristic("<anonymous attack>", e);
    }
    
    const char * description = [[attacks debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
    ConstExtent extent(description);
    ReportParseError( GetName(), description, extent, "Unknown field type / not usable for encoding an attack");
}

void Species::ReadSkills( NSObject * __nonnull skills)
{
    char buf[512];
    
    if( [skills isKindOfClass: NSDictionary.class])
    {
        NSDictionary * d = (NSDictionary*) skills;
        [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop)
        {
            char name[512];  snprintf( name, sizeof(name), "Skill::%s", [key cStringUsingEncoding: NSUTF8StringEncoding]);
            if( [obj isKindOfClass: NSNumber.class])
                return AddCharacteristic( name, [(NSNumber*)obj doubleValue]);

            if( [obj isKindOfClass: NSAttributedString.class])
                obj = [(NSAttributedString*) obj string];
            if( [obj isKindOfClass: NSString.class])
            {
                AstrolabeExpression::Expression * e = CreateExpression(name, [(NSString*) obj cStringUsingEncoding: NSUTF8StringEncoding]);
                return AddCharacteristic(name, e);
            }
            
            const char * description = [[obj debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
            ConstExtent extent(description);
            ReportParseError( name, description, extent, "Unknown field type / not usable for encoding a skill");
        }];
        return;
    }
        
    const char * description = [[skills debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
    ConstExtent extent(description);
    ReportParseError( GetName(), description, extent, "Unknown field type / not usable for encoding an skill");
}

void Species::ReadModifiers( NSObject * __nonnull modifiers)
{
    char buf[512];
    
    if( [modifiers isKindOfClass: NSDictionary.class])
    {
        NSDictionary * d = (NSDictionary*) modifiers;
        [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop)
        {
            char name[512];  snprintf( name, sizeof(name), "Skill::%s", [key cStringUsingEncoding: NSUTF8StringEncoding]);
            if( [obj isKindOfClass: NSNumber.class])
                return AddCharacteristic( name, [(NSNumber*)obj doubleValue]);

            if( [obj isKindOfClass: NSAttributedString.class])
                obj = [(NSAttributedString*) obj string];
            if( [obj isKindOfClass: NSString.class])
            {
                const char * str =  [(NSString*) obj cStringUsingEncoding: NSUTF8StringEncoding];
                AstrolabeExpression::Expression * e = CreateExpression(name, str);
                Modifier * modifier = new Modifier( e, Modifier::ModifierTypeBRPSituational);
                Variable * v = context.GetVariable(name);
                if(v)
                    v->AddModifier(modifier);
                else
                {
                    snprintf(name, sizeof(name), "Modifier::%s", [key cStringUsingEncoding: NSUTF8StringEncoding]);
                    
                    ConstExtent extent(str);
                    ReportParseError( name,str, extent, "Could not find matching skill for name");
                    delete modifier;
                }
                return;
            }
            
            const char * description = [[obj debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
            ConstExtent extent(description);
            ReportParseError( name, description, extent, "Unknown field type / not usable for encoding a modifier");
        }];
        return;
    }
        
    const char * description = [[modifiers debugDescription] cStringUsingEncoding: NSUTF8StringEncoding];
    ConstExtent extent(description);
    ReportParseError( GetName(), description, extent, "Unknown field type / not usable for encoding an modifier");
}
