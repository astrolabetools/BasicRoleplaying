//
//  Tests.m
//  Tests
//
//  Created by Ian Ollmann on 7/4/24.
//

#import <XCTest/XCTest.h>
#import "Table.hpp"
#include "Creature.hpp"

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

const char * GetCorrectEntry( ::Table<const char*, 100>::Entry * table, unsigned long count, unsigned long index)
{
    for( long i = count -1; i >= 0; i--)
    {
        if( table[i].startIndex <= index)
            return table[i].value;
    }
    
    return NULL;
}

- (void)testTable {
    
    ::Table<const char*, 100>::Entry data[] =
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
    unsigned long count = sizeof(data) / sizeof(data[0]);
    
    for( unsigned long tableSize = 1; tableSize <= count; tableSize ++)
    {
        ::Table<const char*, 100> table(data, tableSize);
        
        for( unsigned long i = 1; i <= 100; i++)
        {
            const char * test = table.GetValue(i);
            const char * correct = GetCorrectEntry(data, count, i);
            
            if(strcmp(test, correct))
            {
                test = table.GetValue(i);
                correct = GetCorrectEntry(data, tableSize, i);
            }
            XCTAssert(0 == strcmp(test, correct));
        }
    }
}

-(void) testCombatResult
{
    static const CombatResult kExpectedResult[5][5] =
    {
        {   // attacker critical
            CombatResult(SkillResultFailure,  false, 0, false, 0),  // Defender parries or dodges damage, no other result.
            CombatResult(SkillResultSuccess,  false, 0, false, 2),  // Attack partially deflected or dodged and achieves a success. Attacker strikes defender and rolls damage normally. Defender’s armor value subtracted from damage. Parrying weapon or shield takes 2 points of damage.*
            CombatResult(SkillResultSpecial,  false, 0, false, 4),  // Attack marginally deflected and achieves a special success. Attack does full damage† plus normal damage modifier and appropriate special result. Defender’s armor value subtracted from damage. Parrying weapon or shield takes 4 points of damage.*
            CombatResult(SkillResultCritical, false, 0, false, 0),  // Attack achieves a critical success. Attack does full damage† plus normal damage modifier (or attacker may choose a special success instead). Defender’s armor value is bypassed.
            CombatResult(SkillResultCritical, false, 0, true,  0),  // Attack achieves a critical success. Attack does full damage** plus normal damage modifier (or attacker may choose a special success instead). Defender’s armor value does not apply. Defender rolls on the appropriate fumble table.
        },
        {   // attacker special
            CombatResult(SkillResultFailure, false, 1, false, 0),   // Defender parries or dodges attack; no other result. If attack is parried, attacking weapon takes 1 point of damage.*
            CombatResult(SkillResultFailure, false, 0, false, 0),   // Defender parries or dodges attack, no other result.
            CombatResult(SkillResultSuccess, false, 0, false, 2),   // Attack partially parried or dodged and achieves a normal success. Defender’s armor value subtracted from damage. Parrying weapon or shield takes 2 points of damage.*
            CombatResult(SkillResultSpecial, false, 0, false, 0),   // Attack achieves a special success. Attack does full damage** plus normal damage modifier and appropriate special result. Defender’s armor value subtracted from damage.
            CombatResult(SkillResultSpecial, false, 0, true,  0),   // Attack achieves a special success. Attack does full damage** plus normal damage modifier and appropriate special result. Defender’s armor value subtracted from damage. Defender rolls on the appropriate fumble table.
        },
        {   // attacker success
            CombatResult(SkillResultFailure, false, 2, false, 0),   //  Defender blocks or dodges damage; no other result. If parried in melee combat, attacker’s weapon takes 2 points of damage.*
            CombatResult(SkillResultFailure, false, 1, false, 0),   //  Defender blocks or dodges damage; no other result. If parried in melee combat, attacker’s weapon takes 1 point of damage.*
            CombatResult(SkillResultFailure, false, 0, false, 0),   //  Defender blocks or dodges damage, no other result.
            CombatResult(SkillResultSuccess, false, 0, false, 0),   //  Attack strikes defender and rolls damage normally. Defender’s armor value subtracted from damage.
            CombatResult(SkillResultSuccess, false, 0, true,  0),   //  Attack strikes defender and rolls damage normally. Defender’s armor value subtracted from damage. Defender rolls on the appropriate fumble table.
        },
        {   // attacker failure
            CombatResult(SkillResultFailure, false, 0, false, 0),   // No damage; no effect.
            CombatResult(SkillResultFailure, false, 0, false, 0),   // No damage; no effect.
            CombatResult(SkillResultFailure, false, 0, false, 0),   // No damage; no effect.
            CombatResult(SkillResultFailure, false, 0, false, 0),   // No damage; no effect.
            CombatResult(SkillResultFailure, false, 0, false, 0),   // No damage; no effect.
        },
        {   // attacker fumble
            CombatResult(SkillResultFumble, true, 0, false, 0),    // Attack misses completely and attacker rolls on the appropriate fumble table. Defender unharmed.
            CombatResult(SkillResultFumble, true, 0, false, 0),    // Attack misses completely and attacker rolls on the appropriate fumble table. Defender unharmed.
            CombatResult(SkillResultFumble, true, 0, false, 0),    // Attack misses completely and attacker rolls on the appropriate fumble table. Defender unharmed.
            CombatResult(SkillResultFumble, true, 0, false, 0),    // Attack misses completely and attacker rolls on the appropriate fumble table. Defender unharmed.
            CombatResult(SkillResultFumble, true, 0, false, 0),    // Attack misses completely and attacker rolls on the appropriate fumble table. Defender unharmed.
        },
    };
    
    for( SkillResult attacker = SkillResultFumble; attacker <= SkillResultCritical; attacker = attacker + 1)
    {
        for( SkillResult defender = SkillResultFumble; defender <= SkillResultCritical; defender = defender + 1)
        {
            CombatResult correct = kExpectedResult[int(SkillResultCritical) - int(attacker)][int(SkillResultCritical) - int(defender)];
            CombatResult test(attacker, defender);
            XCTAssert( correct.operator==(test) );
            if( ! correct.operator==(test) )
            {
                CombatResult correct2 = kExpectedResult[int(SkillResultCritical) - int(attacker)][int(SkillResultCritical) - int(defender)];
                CombatResult test2(attacker, defender);
            }
        }
    }
}

- (void) testLoadCreatures
{
    NSBundle * bundle = [NSBundle bundleForClass:[self class]];
    NSURL *url = [bundle  URLForResource: @"Creatures" withExtension:@"plist" ];

    NSError * error = nil;
    Species::ImportURL(url, &error);
    
    const Astrolabe::LinkedList<Species> & speciesList = Species::GetList();
    speciesList.Iterate(^bool(Species * _Nonnull node) {
        node->Print();
        return false;
    });
    
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}


@end
