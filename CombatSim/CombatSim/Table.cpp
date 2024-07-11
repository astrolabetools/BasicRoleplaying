//
//  Table.cpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/9/24.
//

#include "Table.hpp"

Table<const char*, 100>::Entry meleeWeaponAttackFumblesEntries[] =
{
    { 01,   "Lose the next combat round and are effectively helpless."},
    { 16,   "Lose the next 1D3 combat rounds and are effectively helpless."},
    { 26,   "Fall prone."},
    { 41,   "Drop the weapon being used."},
    { 51,   "Throw weapon 1D10 meters away."},
    { 61,   "Lose 1D10 points of weapon’s hit points."},
    { 66,   "Vision obscured, modify all appropriate skills by –30% for 1D3 combat rounds."},
    { 76,   "Hit nearest ally for normal damage or use result 41–50 if no ally nearby."},
    { 86,   "Hit nearest ally for special damage or use result 51–60 if no ally nearby."},
    { 91,   "Hit nearest ally for critical damage or use result 61–65 if no ally nearby."},
    { 99,   "Blow it; roll twice more on this table (cumulative if this result is rolled again)."},
    {100,   "Blow it badly; roll three times more on this table (cumulative if this result is rolled again)."},
};
static const unsigned long kMeleeWeaponAttackFumblesCount = sizeof(meleeWeaponAttackFumblesEntries) / sizeof(meleeWeaponAttackFumblesEntries[0]);
Table<const char*, 100> meleeWeaponAttackFumbles(meleeWeaponAttackFumblesEntries, kMeleeWeaponAttackFumblesCount);

Table<const char*, 100>::Entry meleeWeaponParryFumblesEntries[] =
{
    { 01, "Lose the next combat round (or this one if no action has yet been taken), and are effectively helpless."},
    { 21, "Fall prone."},
    { 41, "Drop weapon being used."},
    { 51, "Throw weapon 1D10 meters away."},
    { 61, "Vision obscured; modify all appropriate skills by –30% for 1D3 combat rounds."},
    { 76, "Wide open; foe automatically hits with normal hit."},
    { 86, "Wide open; foe automatically hits with special hit."},
    { 91, "Wide open; foe automatically hits with critical hit."},
    { 94, "Blow it; roll twice more on this table (cumulative if this result is rolled again)."},
    { 99, "Blow it badly; roll three times more on this table (cumulative if this result is rolled again)."},
};
static const unsigned long kMeleeWeaponParryFumblesCount = sizeof(meleeWeaponParryFumblesEntries) / sizeof(meleeWeaponParryFumblesEntries[0]);
Table<const char*, 100> meleeWeaponParryFumbles(meleeWeaponParryFumblesEntries, kMeleeWeaponParryFumblesCount);

Table<const char*, 100>::Entry missileWeaponAttackFumblesEntries[] =
{
    { 01, "Lose the next attack or other activity."},
    { 16, "Lose the next 1D3 combat rounds or other activity."},
    { 26, "Fall prone."},
    { 41, "Vision obscured; modify all appropriate skills by –30% for 1D3 combat rounds."},
    { 56, "Drop weapon; which slides or bounces 1D6–1 meters away."},
    { 66, "Do 1D6 damage to weapon’s hit points (or use result 81–85 if the weapon has no hit points)."},
    { 81, "Break weapon; regardless of weapon’s current hit points."},
    { 86, "Hit nearest ally for normal damage or use result 56–65 if no ally nearby."},
    { 91, "Hit nearest ally for special damage or use result 66–80 if no ally nearby."},
    { 96, "Hit nearest ally for critical damage or use result 81–85 if no ally nearby."},
    { 99, "Blow it; roll twice more on this table (cumulative if this result is rolled again)."},
    {100, "Blow it badly; roll three times more on this table (cumulative if this result is rolled again)."},
};
static const unsigned long kMissileWeaponAttackFumblesCount = sizeof(missileWeaponAttackFumblesEntries) / sizeof(missileWeaponAttackFumblesEntries[0]);
Table<const char*, 100> missileWeaponAttackFumbles(missileWeaponAttackFumblesEntries, kMissileWeaponAttackFumblesCount);

Table<const char*, 100>::Entry naturalWeaponParryAndAttackFumblesEntries[] =
{
    { 01, "Lose the next combat round (or this one if no action has yet been taken)."},
    { 26, "Lose the next 1D3 combat rounds (this includes this one if no action has yet been taken)."},
    { 31, "Fall prone."},
    { 51, "Fall prone and twist ankle; modify MOV by –1 MOV for 1D10 full turns (and all combat turns within them)."},
    { 61, "Vision obscured; modify all appropriate skills by –30% for 1D3 combat rounds."},
    { 76, "Miss and strain something; lose 1 hit point (in the attacking limb if hit locations are being used)."},
    { 86, "Hit nearest ally for normal damage or use result 76–85 if no ally nearby."},
    { 91, "Hit nearest ally for special damage or use result 76–85 if no ally nearby."},
    { 95, "Hit hard surface; do normal damage to self (in the attacking limb if hit locations are being used)."},
    { 99, "Blow it; roll twice more on this table (cumulative if this result is rolled again)."},
    {100, "Blow it badly; roll three times more on this table (cumulative if this result is rolled again).};"},
};
static const unsigned long kNaturalWeaponParryAndAttackFumblesCount = sizeof(naturalWeaponParryAndAttackFumblesEntries) / sizeof(naturalWeaponParryAndAttackFumblesEntries[0]);
Table<const char*, 100> naturalWeaponParryAndAttackFumbles(naturalWeaponParryAndAttackFumblesEntries, kNaturalWeaponParryAndAttackFumblesCount);

Table<const char*, 100>::Entry majorWouldTableEntries[] =
{
    {  01, "Severed leg tendons cause limping; fused ankle bones cause limping; back muscles or spinal nerve damage bend the torso to the left or right; a shattered knee cannot bend; or make up a new wound effect. Lose 1D3 DEX and reduce MOV by the same amount. The character is still able to fight."},
    {  11, "Much of the nose has been sliced away; multiple scars deface both hands; an ear has been cut off; a livid scar lends an evil cast to appearance; or make up a new wound effect. Lose 1D3 CHA. The visible and unappealing deformity cannot be disguised. The character is still able to fight."},
    {  21, "Wrist or hand damage; a slab of arm or shoulder muscle has been cut away; a chunk was hewn from thigh or calf muscles; spinal nerves are damaged; several fingers or toes are severed; or make up a new wound effect. Lose 1D3 STR; this loss may change what weapons can be used. The character is still able to fight with a weapon, but not a shield."},
    {  31, "A punctured lung leads to a weakened respiratory system; deep stomach wounds become chronically reinfected, or belly wounds weaken digestion and general health; kidneys and liver are damaged; or make up a new wound effect. Lose 1D3 CON; reduce MOV by the same amount, and hit points may be lowered. The character is still able to fight."},
    {  41, "Concussion damages hearing and limits Listen and Insight to maximums of 65%; injury to the head thereafter require Luck rolls each time to use any Mental skills; blows or cuts which affect depth perception leave missile weapon skill ratings at a maximum of 65%; multiple wounds to the face and neck limit the skills of any Communication skills to a maximum of 65%; or make up a new wound effect. Lose 1D3 INT; this loss may affect the character’s ability to use any powers, such as sorcery. The character is still able to fight."},
    {  51, "Refer to 01–10, but expanded to the loss of one or both arms or legs. Lose 1D6 DEX and reduce maximum MOV by that same amount. The character is unable to fight."},
    {  61, "Refer to 11–20, but the major wound now includes worse mutilations. Lose 1D6 CHA; it creates one or more visible deformities that cannot be disguised. The character is still able to fight."},
    {  71, "Refer to 21–30 for examples of mutilating cuts and losses. Lose 1D6 STR, perhaps adjusting damage modifier. The character is still able to fight."},
    {  81, "Refer to 31–40 for various wounds to the vitals. Lose 1D6 CON; may affect hit points and damage modifier and reduces MOV equal to the CON loss. The character is unable to fight."},
    {  91, "Bad facial and vocal cord injuries. Lose 1D6 CHA. The character is still able to fight."},
    {  93, "Broken bones and severed ganglia. Lose 1D6 DEX; from now on the character can only use one-handed melee weapons. The character is still able to fight using their remaining arm."},
    {  95, "Nerve damage to left or right arm. Lose 1D6 DEX; hereafter your character can only wield weapons or equipment in their undamaged arm. The character is still able to fight using their remaining arm."},
    {  97, "Nerve damage to both arms. Lose 1D6 DEX; though the legs are fine, neither arms nor hands can wield anything. The character is unable to fight, unless using their legs or head butts."},
    {  99, "Mutilated with vicious wounds. Lose 1D3 points each from CHA, DEX, and CON. The character is unable to fight."},
    { 100, "Deliberately mutilated after collapsing. Remove 1D4 points each from four characteristics (gamemaster’s discretion). The character is unable to fight."},
};
static const unsigned long kMajorWoundTableCount = sizeof(majorWouldTableEntries) / sizeof(majorWouldTableEntries[0]);
Table<const char*, 100> majorWouldTable(majorWouldTableEntries, kMajorWoundTableCount);


Table<HitLocation, 20>::Entry hitLocationTableEntries[] = {          // p126
    {1,  HitLocationRightLeg },
    {5,  HitLocationLeftLeg },
    {9,  HitLocationAbdomen },
    {12,  HitLocationChest },
    {13,  HitLocationRightArm },
    {16,  HitLocationLeftArm },
    {19,  HitLocationHead },
};
static const unsigned long kHitLocationTableCount = sizeof(hitLocationTableEntries) / sizeof(hitLocationTableEntries[0]);
Table<HitLocation, 20> hitLocationTable(hitLocationTableEntries, kHitLocationTableCount);

Table<const char *, 100>::Entry randomMutationTableEntries[] = {          // p126
    {01, "Adaptability"},
    {04, "Allergy"},
    {06, "Biped (Quadruped)"},
    {10, "Camouflage"},
    {13, "Coloration"},
    {16, "Congenital Disease"},
    {20, "Decreased Characteristic"},
    {23, "Disease Carrier"},
    {26, "Group Intelligence"},
    {30, "Hands"},
    {31, "Hardy"},
    {34, "Hybrid"},
    {36, "Imitation"},
    {38, "Increased Characteristic"},
    {45, "Keen Sense"},
    {49, "Luminescence"},
    {51, "Metabolic Improvement"},
    {54, "Metabolic Weakness"},
    {58, "Natural Armor"},
    {61, "Natural Weaponry"},
    {66, "Pain Sensitivity"},
    {68, "Pheromone"},
    {69, "Reduced Sense"},
    {71, "Regeneration"},
    {73, "Sensitivity"},
    {75, "Speech (Mimicry)"},
    {77, "Structural Improvement"},
    {82, "Structural Weakness"},
    {87, "Venom90 Wings"},
    {91, "Gamemaster or Player Choice"},
};
static const unsigned long kRandomMutationTableCount = sizeof(randomMutationTableEntries) / sizeof(randomMutationTableEntries[0]);
Table<const char *, 100> randomMutationTable(randomMutationTableEntries, kRandomMutationTableCount);

