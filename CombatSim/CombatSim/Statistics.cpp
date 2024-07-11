//
//  Statistics.cpp
//  CombatSim
//
//  Created by Ian Ollmann on 7/9/24.
//

#include "Statistics.hpp"


Statistics::Statistics( uint64_t sum,           // SUM(Xi)
                        uint64_t sumSquares,    // SUM(Xi^2)
                        uint64_t N )            // number of samples
{
#ifdef __SIZEOF_INT128__
    typedef __uint128_t LargeUInt;
#else
    assert( sum < 0x100000000ULL);            // check for evidence of impending overflow
    assert( N * sumSquares >= sum * sum);     // check for evidence of impending overflow
    typedef uint64_t LargeUInt;
#endif
    
    // Standard deviation of the mean = standard deviation / sqrt(N)
    //          https://en.wikipedia.org/wiki/Standard_deviation
    //
    //                              1      N
    // standard deviation = sqrt( -----   SUM( (Xi - mean(X))^2 )
    //                            N - 1   i=1
    //
    //                              1      N
    // standard deviation = sqrt( -----   SUM( Xi^2 - 2Xi*mean(x) + mean(X)^2 )
    //                            N - 1   i=1
    //
    //                              1      N
    // standard deviation = sqrt( -----   SUM( Xi^2 - 2Xi*mean(x) + mean(X)^2 )
    //                            N - 1   i=1
    //
    //   N                                          N
    //  SUM( -2Xi*mean(x) + mean(X)^2) = mean(x) * SUM( -2Xi + mean(X))
    //  i=1                                        i=1
    //
    //   N                                                         N
    //  SUM( -2Xi*mean(x) + mean(X)^2) = mean(x) * (N*mean(x) - 2*SUM(Xi))
    //  i=1                                                       i=1
    //
    //   N                               SUM(Xi)
    //  SUM( -2Xi*mean(x) + mean(X)^2) = ------- * (SUM(Xi) - 2*SUM(Xi))
    //  i=1                                 N
    //
    //
    //   N
    //  SUM( -2Xi*mean(x) + mean(X)^2) = -SUM(Xi)^2
    //  i=1
    //
    //                              1      N           SUM(Xi)^2
    // standard deviation = sqrt( -----   SUM( Xi^2 - ----------- )
    //                            N - 1   i=1              N
    //
    //                               1        N
    // standard deviation = sqrt( --------   SUM( N * Xi^2 - SUM(Xi)^2 )
    //                            N(N - 1)   i=1
    //
    //                            N*SUM(Xi^2) - SUM(Xi)^2
    // standard deviation = sqrt( ----------------------- )
    //                                    N(N - 1)
    //
    // Which means we need only a running sum of samples and samples squared to calculate the value
    // of standard deviation and standard deviation of the mean as we go along.  We do not need to
    // keep the entire list of all Xi in order to do a two pass algorithm.
    //
    // Noting, of course that N*SUM(Xi^2) - SUM(Xi) may be the subtraction of two very similar
    // quantities when the error is small, and so possibly the victim of catastrophic cancellation
    // in IEEE-754 floating-point arithmetic losing all precision and possibly ending up with a
    // negative number due to unfortunate rounding in the summation process. Sqrt(-x) is sign of
    // a problem. To avoid this, we only use this method with integer Xi, and make sure the computation types
    // are large enough to avoid overflow.  (Strictly speaking, this can be done with double precision
    // floating-point numbers too as long as the values are integers and the sum of squares is never
    // larger than 2^54. Since the cutoff for uint64_t is 2^64, we get more headroom before overflow
    // using that type.)
    
    LargeUInt product = N;    product *= sumSquares;            // N * SUM(Xi^2);               overflow risk
    LargeUInt square = sum;   square *= sum;                    // SUM(Xi)^2                    overflow risk
    LargeUInt difference = product - square;                    // N * SUM(Xi^2) - SUM(Xi)^2;   Catastrophic cancellation danger is at this step when variance is small
    
    double v = double(difference) / (N*N - N);                 // (N * SUM(Xi^2) - SUM(Xi)^2) / N(N-1)
                                                                // for N=0, we expect NAN
    mean = double(sum) / N;                                     // also 0/0 = NAN for N=0
    stdDeviation = sqrt(v);
    stdDeviationOfMean = sqrt(v / N);
}


