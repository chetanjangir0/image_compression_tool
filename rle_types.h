#ifndef RLE_TYPES_H
#define RLE_TYPES_H

typedef struct{
    unsigned char count;
    unsigned char value;

} RLEPair;

typedef struct {
    int width;
    int height;
    RLEPair *red;
    RLEPair *green;
    RLEPair *blue;
    int redSize;
    int greenSize;
    int blueSize;
} RLEData;

#endif