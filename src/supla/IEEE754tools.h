//
//    FILE: IEEE754tools.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.00
// PURPOSE: IEEE754 tools
//
// http://playground.arduino.cc//Main/IEEE754tools
//
// Released to the public domain
// not tested, use with care
//

#ifndef IEEE754tools_h
#define IEEE754tools_h

// IEEE754 float layout; 
struct IEEEfloat
{
    uint32_t m:23; 
    uint8_t e:8;
    uint8_t s:1;
};

// for packing and unpacking a float
typedef union _FLOATCONV
{
    IEEEfloat p;
    float f;
    uint8_t b[4];
} _FLOATCONV;

// Arduino UNO double layout: 
// the UNO has no 64 bit double, it is only able to map 23 bits of the mantisse
// a filler is added.
struct _DBL
{
    uint32_t filler:29;
    uint32_t m:23;
    uint16_t e:11;
    uint8_t  s:1;
};


// for packing and unpacking a double
typedef union _DBLCONV
{
    // IEEEdouble p;
    _DBL p;
    double d;           // !! is a 32bit float for UNO.
    uint8_t b[8];
} _DBLCONV;

//
// converts a float to a packed array of 8 bytes representing a 64 bit double
// restriction exponent and mantisse.
//
// float;  array of 8 bytes;  LSBFIRST; MSBFIRST
//
//void float2DoublePacked(float number, byte* bar, int byteOrder=LSBFIRST);


#endif
