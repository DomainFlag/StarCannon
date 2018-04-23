#ifndef __SIMPLEX_H__
#define __SIMPLEX_H__

#include <cstdint>

class Grad {
public:
    Grad( int8_t x, int8_t y, int8_t z ) : x(x), y(y), z(z) {}
    int8_t x, y, z;
};

class Simplex {
public:
    // Initialize permutation arrays
    static void init();
    // 2D simplex noise
    static double noise( double xin, double yin );

private:
    static int32_t fastFloor( double x );
    static double dot( const Grad& g, double x, double y );
    static const double F2;
    static const double G2;
    static const Grad grad3[12];
    static const uint8_t p[256];
    static uint8_t perm[512];
    static uint8_t permMod12[512];
};

#endif