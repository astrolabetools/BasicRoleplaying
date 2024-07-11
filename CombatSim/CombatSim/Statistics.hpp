//
//  Statistics.hpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/8/24.
//

#ifndef Statistics_h
#define Statistics_h

#include <stdint.h>
#include <assert.h>
#include <math.h>

typedef struct Statistics
{
public:
    double          mean;                   // an estimate of the mean value of the data
    double          stdDeviation;           // a measurement of the error in the data
    double          stdDeviationOfMean;     // an estimate of the error in the mean
    
    Statistics() : mean(NAN), stdDeviationOfMean(NAN){}
    Statistics( uint64_t sum, uint64_t sumSquares, uint64_t N );
}Statistics;


class StatAccumulator
{
private:
    typedef struct Sums
    {
    private:
        // The algorithm Statistics::Statistics() uses is very sensitive to roundoff error, so we use integers here to avoid that.
        uint64_t        sum;            // sum of samples
        uint64_t        sumSquares;     // some of sample * sample; For some problems, this might have to be __uint128_t
        uint64_t        count;          // # of samples
        
    public:
        Sums() : sum(0), sumSquares(0), count(0){}
        
        inline Sums & operator+=( uint32_t value)
        {   // check to avoid overflow                      operation
            uint64_t square = value * value;
            assert( sum + value >= sum);                    sum += value;
            assert( sumSquares + square >= sumSquares);     sumSquares += square;
                                                            count++;
            return *this;
        }
        
        inline operator Statistics() const {return Statistics( sum, sumSquares, count);}
    }Sums;
    
    Sums  * sums;
    unsigned long count;
    
    StatAccumulator( const StatAccumulator & stats);                    // prevent copying for now
    StatAccumulator & operator=( const StatAccumulator & stats);        // prevent copying for now

public:
    StatAccumulator( unsigned long creatureCount) : count(creatureCount), sums(new Sums[creatureCount]){}
    ~StatAccumulator(){ delete [] sums; sums = NULL; count = 0;}
    
    inline Sums & operator[](unsigned long index){ return sums[index];}
    inline const Sums & operator[](unsigned long index) const { return sums[index];}
    
    inline Statistics GetStatistics(unsigned long index){ assert(index < count); return Statistics( sums[index]); }
};




#endif /* Statistics_h */
