//
//  Table.h
//  CombatSim
//
//  Created by Ian Ollmann on 7/6/24.
//

#ifndef Table_h
#define Table_h


#ifdef __cplusplus




/*! @abstract The table implements a table, such as the Random Mutations table on page 66
 *  @param T    The type of the object looked up from the table. Often a const char *  */
template <typename T>
    class Table
    {
    public:
        typedef struct
        {
            long            startIndex;     // The first index covered by the entry
            T               value;
        }Entry;
        
    private:
        const Entry * entries;
        unsigned long count;
        
    public:
        Table( const Table<T>::Entry * _data, unsigned long _count ) : entries(_data), count(_count){}

        inline const T & GetValue( long index );
    };



#pragma mark -

template <typename T>
const T & GetValue( long where )
{ // log2 search pattern
    assert(count > 0);
    unsigned long stride = count >> 1;
    unsigned long i = stride;
    unsigned long hops = 0;
    while( stride )
    {
        printf( "%lu) trying index %lu (stride=%lu)\n", hops++, index, stride);
        stride = (stride + 1) >> 1;
        if( where < entries[index].startIndex )
        {
            i -= max(stride, 1UL);
            continue;
        }

        if( where > entries[index].startIndex )
        {
            i += max(stride, 1UL);
            continue;
        }
        
        break;
    }
    
    return list[i];
}

#endif __cplusplus

#endif /* Table_h */
