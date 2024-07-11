//
//  Table.h
//  CombatSim
//
//  Created by Ian Ollmann on 7/6/24.
//

#ifndef Table_h
#define Table_h

#include <stdlib.h>
#include "SkillCheck.hpp"
#ifdef __cplusplus


typedef enum : uint8_t
{
    HitLocationRightLeg,
    HitLocationLeftLeg,
    HitLocationAbdomen,
    HitLocationChest,
    HitLocationRightArm,
    HitLocationLeftArm,
    HitLocationHead
}HitLocation;

/*! @abstract The table implements a table, such as the Random Mutations table on page 66
 *  @template T    The type of the object looked up from the table. Often a const char
 *  @seealso Tests.mm for sample usage  */
template <typename T, int dieSize>
    class Table
    {
    public:
        typedef struct
        {
            long            startIndex;     // The first index covered by the table entry. Yes, longs are big, but so are char* value and the compiler will align it.
            T               value;          // the contents of the table entry, often a const char[]
        }Entry;
        
    private:
        const Entry * entries;
        unsigned long count;
        
    public:
        Table( const Table<T, dieSize>::Entry * _data, unsigned long _count ) : entries(_data), count(_count){}

        inline const T & GetValue( long dieRoll = RollDice(1, dieSize) ) const;        // could use operator[] here, but seems deceptive
        inline unsigned long GetEntryCount() const {return count;}
    };

extern Table<const char*, 100> meleeWeaponAttackFumbles;              // p128
extern Table<const char*, 100> meleeWeaponParryFumbles;               // p128
extern Table<const char*, 100> missileWeaponAttackFumbles;            // p128
extern Table<const char*, 100> naturalWeaponParryAndAttackFumbles;    // p128
extern Table<const char*, 100> majorWouldTable;                       // p135
extern Table<HitLocation, 20>  hitLocationTable;                      // p126
extern Table<const char *, 100> randomMutationTable;                  // p66

#pragma mark -

template <typename T, int dieSize>
const T & Table<T, dieSize>::GetValue( long where ) const
{ // log2 search pattern
    assert(count > 0);
    unsigned long stride = count >> 1;
    unsigned long i = stride;
    while( stride )
    {
        stride = (stride + 1) >> 1;            // Rounding up can overshoot the ends of the table but we find the result faster than for just truncation by maybe an extra loop iteration.
                                               // I also tried flipping the polarity of the rounding figure, but that delivered an incorrect results sometimes without similar constraints. (See below)
        
        stride = max(stride, 1UL);              // make sure the stride is at least 1
        if( where < entries[i].startIndex )
        {
            i -= min(stride, i);                // avoid going past 0
            continue;
        }

        if( i+1 < count && where >= entries[i+1].startIndex )
        {
            i += stride;
            i = min( i, count-1);               // avoid going past count-1
            continue;
        }

        break;
    }

    return entries[i].value;
}


#endif /* __cplusplus */

#endif /* Table_h */
